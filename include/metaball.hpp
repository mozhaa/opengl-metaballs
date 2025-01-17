#pragma once

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

const int n_parameters = 3;
const int n_balls = 10;
const int grid_size = 128;

struct scene {
    glm::vec3 positions[n_balls];
    glm::vec3 colors[n_balls];
    float radiuses[n_balls];

    // motion parameters
    glm::vec2 orbit_scales[n_balls];
    glm::mat4 rotations[n_balls];
    float angular_velocities[n_balls];

    scene();
    void update_positions(float time);
};

struct function_texture {
    GLuint program, texture;

    function_texture();
    void calculate(scene& scene);
};
