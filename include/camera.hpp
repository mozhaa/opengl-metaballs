#pragma once

#include <glm/mat4x4.hpp>

struct camera_settings {
    glm::mat4 view;
    glm::mat4 projection;

    float camera_distance;
    float view_elevation;
    float view_azimuth;
    int width;
    int height;
    float near = 0.01f;
    float far = 10.f;

    camera_settings(int width, int height);
    void update();
};