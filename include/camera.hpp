#pragma once

#ifdef WIN32
#include <SDL.h>
#undef main
#else
#include <SDL2/SDL.h>
#endif

#include <map>

#define GLM_FORCE_SWIZZLE
#include <glm/mat4x4.hpp>

struct camera_settings {
    glm::mat4 view;
    glm::mat4 projection;

    glm::vec3 camera_position;
    glm::vec2 camera_direction;
    float speed;
    float base_speed;
    float rotation_speed;

    int width;
    int height;
    float near;
    float far;

    camera_settings(int width, int height);
    void update(std::map<SDL_Keycode, bool>& button_down, float dt);

private:
    void update_matrices();
};