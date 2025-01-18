#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace metaballs {

const int n_balls = 10;

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

} // namespace metaballs
