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

#include <stdio.h>
#include <chrono>
#include <cmath>
#include <ctime>

#include <iostream>
#include <sstream>
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
#include "stb_image_write.h"

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
    environment_map envmap;
    box_drawer box({0.f, 0.f, 0.f}, {1.f, 1.f, 1.f});
    float target_value = 0.5f;
    float d_target_value = 0.25f;

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    int frame_idx = 0;

    std::vector<uint32_t> pixels;
    bool ffmpeg_process_was_started = false;
    FILE *ffmpeg_process = NULL; 
    float ffmpeg_framerate = 60.f;

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
                    width = event.window.data1;
                    height = event.window.data2;
                    camera.width = width;
                    camera.height = height;
                    glViewport(0, 0, width, height);
                    if (ffmpeg_process != NULL) {
                        LOG(WARNING) << "ffmpeg recording stopped, because window was resized";
                        pclose(ffmpeg_process);
                        ffmpeg_process = NULL;
                    }
                    break;
                }
                break;
            case SDL_KEYDOWN:
                button_down[event.key.keysym.sym] = true;

                if (event.key.keysym.sym == SDLK_SPACE)
                    paused = !paused;

                if (event.key.keysym.sym == SDLK_RETURN) {
                    if (ffmpeg_process == NULL) {
                        auto t = std::time(nullptr);
                        auto tm = *std::localtime(&t);
                        std::ostringstream output_fp;
                        output_fp << "\"" << PROJECT_ROOT << "/resources/" << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << ".mp4\"";

                        std::string command = (
                            std::string("ffmpeg -y -f rawvideo -video_size ") + 
                            std::to_string(width) + "x" + std::to_string(height) + 
                            " -pix_fmt rgb24 -r " + std::to_string(ffmpeg_framerate) + 
                            " -i - -vf vflip -an -c:v libx264 " + output_fp.str()
                        );
                        ffmpeg_process = popen(command.c_str(), "w");
                        if (!ffmpeg_process)
                            throw std::runtime_error("Failed to start ffmpeg process.");
                        pixels.reserve(width * height * 3);
                        LOG(INFO) << "ffmpeg recording started, output=" << output_fp.str();
                    } else {
                        pclose(ffmpeg_process);
                        ffmpeg_process = NULL;
                        LOG(INFO) << "ffmpeg recording stopped";
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

        if (ffmpeg_process != NULL) {
            // fix framerate
            dt = 1.f / ffmpeg_framerate;

            // hold right arrow
            // button_down[SDLK_RIGHT] = true;
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw environment map
        envmap.draw(camera);

        // draw white box
        box.draw(camera, grid.model);

        // draw grid
        grid.draw(field, camera, target_value);

        if (ffmpeg_process != NULL) {
            glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
            // stbi_write_png((std::to_string(frame_idx) + ".png").c_str(), width, height, 3, pixels.data(), width * 3);
            fwrite(pixels.data(), width * height * 3, 1, ffmpeg_process);
        }

        SDL_GL_SwapWindow(window);
    }

    if (ffmpeg_process != NULL)
        pclose(ffmpeg_process);

} catch (std::exception const &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}
