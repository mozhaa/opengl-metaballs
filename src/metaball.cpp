#include "metaball.hpp"

#include <algorithm>
#include <cmath>
#include <random>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/mat4x4.hpp>

#include "shaderload.h"

metaballs_collection::metaballs_collection() {
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0.f, 1.f);

    for (int i = 0; i < n_balls; ++i) {
        radiuses[i] = dist(e2) * 0.2f + 0.05f;
        colors[i] = {dist(e2), dist(e2), dist(e2)};
        orbit_scales[i] = glm::vec2(dist(e2) - 0.5f, dist(e2) - 0.5f) * 2.f;
        glm::vec3 axis = glm::normalize(glm::vec3(dist(e2) - 0.5f, dist(e2) - 0.5f, dist(e2) - 0.5f));
        float angle = dist(e2) * M_2_PI;
        rotations[i] = glm::rotate(glm::mat4(1.f), angle, axis);
        angular_velocities[i] = std::sqrt(dist(e2));
    }

    update_positions(0);
}

void metaballs_collection::update_positions(float time) {
    for (int i = 0; i < n_balls; ++i) {
        float theta = time * angular_velocities[i];
        glm::vec4 planar = glm::vec4(cos(theta) * orbit_scales[i].x, sin(theta) * orbit_scales[i].y, 0.f, 1.f);
        positions[i] = rotations[i] * planar;
    }
}

metaballs_texture::metaballs_texture() {
    program = create_program({std::string(SHADERS_DIR) + "/compute_function.comp"});

    glEnable(GL_TEXTURE_3D);
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, texture);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, grid_size, grid_size, grid_size, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void metaballs_texture::compute(metaballs_collection &balls) {
    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, texture);

    glBindImageTexture(0, texture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);

    glUniform3fv(glGetUniformLocation(program, "positions"), n_balls, reinterpret_cast<float *>(balls.positions));
    glUniform3fv(glGetUniformLocation(program, "colors"), n_balls, reinterpret_cast<float *>(balls.colors));
    glUniform1fv(glGetUniformLocation(program, "radiuses"), n_balls, balls.radiuses);

    glDispatchCompute(grid_size, grid_size, grid_size);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}