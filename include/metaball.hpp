#pragma once

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

const int n_balls = 10;
const int grid_size = 128;

/**
 * Struct, that represents collection of <n_balls> metaballs
 */
struct metaballs_collection {
    glm::vec3 positions[n_balls];
    glm::vec3 colors[n_balls];
    float radiuses[n_balls];

    // motion parameters
    glm::vec2 orbit_scales[n_balls];
    glm::mat4 rotations[n_balls];
    float angular_velocities[n_balls];

    metaballs_collection();
    void update_positions(float time);
};

/**
 * GL Texture, that contains computed values of total metaballs function
 */
struct metaballs_texture {
    GLuint program, texture;

    metaballs_texture();
    void compute(metaballs_collection& scene);
};
