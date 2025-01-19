#ifdef WIN32
#include <SDL.h>
#undef main
#else
#include <SDL2/SDL.h>
#endif

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

#include <GL/glew.h>

#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include "stb_image_write.h"

#include <chrono>
#include <cmath>

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <vector>

#include "shaderload.h"

#include "metaball.hpp"
#include "field.hpp"
#include "grid3d.hpp"
#include "camera.hpp"
#include "envmap.hpp"
#include "box_drawer.hpp"
#include "lighting_settings.hpp"
#include "recording.hpp"

using namespace metaballs;

std::string to_string(std::string_view str) { return std::string(str.begin(), str.end()); }

void sdl2_fail(std::string_view message) { throw std::runtime_error(to_string(message) + SDL_GetError()); }

void glew_fail(std::string_view message, GLenum error) {
    throw std::runtime_error(to_string(message) + reinterpret_cast<const char *>(glewGetErrorString(error)));
}

void sdl_init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        sdl2_fail("SDL_Init: ");
}

void set_gl_attributes() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
}

SDL_Window *create_window() {
    SDL_Window *window = SDL_CreateWindow("Graphics course practice 5", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

    if (!window)
        sdl2_fail("SDL_CreateWindow: ");

    return window;
}

SDL_GLContext create_context(SDL_Window *window) {
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context)
        sdl2_fail("SDL_GL_CreateContext: ");
    return gl_context;
}

void glew_init() {
    if (auto result = glewInit(); result != GLEW_NO_ERROR)
        glew_fail("glewInit: ", result);

    if (!GLEW_VERSION_3_3)
        throw std::runtime_error("OpenGL 3.3 is not supported");
}

struct rectangle {
    glm::vec2 position;
    glm::vec2 size;
};

int main(int argc, char *argv[]) try {
    START_EASYLOGGINGPP(argc, argv);
    sdl_init();
    set_gl_attributes();
    auto window = create_window();
    auto context = create_context(window);
    glew_init();

    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);

    auto last_frame_start = std::chrono::high_resolution_clock::now();
    float time = 0.f;
    std::map<SDL_Keycode, bool> button_down;

    glClearColor(0.8f, 0.8f, 1.f, 0.f);

    metaballs_collection balls;
    scalar_field_texture field;
    grid3d grid(grid_size);
    camera_settings camera(width, height);

    environment_map envmap1("night_sky.jpg");
    environment_map envmap2("purple.jpg");

    lighting_settings lighting1 = {
        glm::vec3(0.3),
        glm::vec3(0.6),
        glm::vec3(0.2),
        glm::vec3(1.0, 0.7, 0.0),
        glm::vec3(0.8, 0.8, 1.0),
        4.0,
    };

    lighting_settings lighting2 = {
        glm::vec3(0.2),
        glm::vec3(0.9),
        glm::vec3(0.4),
        glm::vec3(-1.0, 0.0, 0.6),
        glm::vec3(1.0, 0.3, 0.5),
        16.0,
    };

    environment_map& envmap = envmap1;
    lighting_settings& lighting = lighting1;

    box_drawer box({0.f, 0.f, 0.f}, {1.f, 1.f, 1.f});
    float target_value = 0.02f;
    float d_target_value = 0.25f;

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    int frame_idx = 0;

    recorder R;
    bool auto_recording = true;

    bool paused = false;
    bool running = true;
    while (running) {
        for (SDL_Event event; SDL_PollEvent(&event);)
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    if ((width != event.window.data1 || height != event.window.data2) && R.is_recording()) {
                        R.stop_recording();
                        LOG(WARNING) << "ffmpeg recording stopped, because window was resized";
                    }
                    width = event.window.data1;
                    height = event.window.data2;
                    camera.width = width;
                    camera.height = height;
                    glViewport(0, 0, width, height);
                    break;
                }
                break;
            case SDL_KEYDOWN:
                button_down[event.key.keysym.sym] = true;

                if (event.key.keysym.sym == SDLK_SPACE)
                    paused = !paused;

                if (event.key.keysym.sym == SDLK_RETURN) {
                    if (R.is_recording())
                        R.stop_recording();
                    else {
                        R.start_recording(width, height);
                        frame_idx = 0;
                    }
                }

                break;
            case SDL_KEYUP:
                button_down[event.key.keysym.sym] = false;
                break;
            }

        if (!running)
            break;

        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration_cast<std::chrono::duration<float>>(now - last_frame_start).count();

        // automatically start recording on start
        if (auto_recording && !R.is_recording())
            R.start_recording(width, height);

        if (R.is_recording()) {
            // fix framerate for recording
            dt = 1.f / R.fps;

            // hold right arrow for recording
            if (auto_recording)
                button_down[SDLK_RIGHT] = true;
        }

        last_frame_start = now;
        if (!paused) {
            time += dt;

            // update balls positions
            balls.update_positions(time);

            // compute function using compute shader
            field.compute(balls);
        }

        if (button_down[SDLK_RIGHTBRACKET])
            target_value += d_target_value * dt;
        if (button_down[SDLK_LEFTBRACKET])
            target_value -= d_target_value * dt;

        // update camera based on pressed keys
        camera.update(button_down, dt);

        if (auto_recording) {
            // automatically change envmap and lighting for recording
            int full_circle_frames = (M_PI * 2 / camera.horizontal_rotation_speed) * R.fps;
            if (frame_idx > 2 * full_circle_frames)
                break;
            if (frame_idx > full_circle_frames) {
                envmap = envmap2;
                lighting = lighting2;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw environment map
        envmap.draw(camera);

        // draw white box
        box.draw(camera, grid.model);

        // draw grid
        grid.draw(field, camera, lighting, target_value);

        if (R.is_recording())
            R.save_frame();

        ++frame_idx;
        SDL_GL_SwapWindow(window);
    }

    if (R.is_recording())
        R.stop_recording();

} catch (std::exception const &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}
