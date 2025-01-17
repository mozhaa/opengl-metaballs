#include "metaball.hpp"

#include "shaderload.h"

#include <algorithm>
#include <cmath>
#include <random>

scene::scene() {
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(-1.f, 1.f);

    for (int i = 0; i < n_balls; ++i) {
        initial_positions[i] = {dist(e2), dist(e2), dist(e2)};
        radiuses[i] = std::abs(dist(e2)) * 0.2f + 0.05f;
        colors[i] = {std::abs(dist(e2)), std::abs(dist(e2)), std::abs(dist(e2))};
        velocities[i] = glm::vec3(dist(e2), dist(e2), dist(e2)) * 0.1f;
    }

    update_positions(0);
}

inline float mod(float x, float m, float M) {
    float w = M - m;
    return std::fmod(std::fmod(x - m, w) + w, w) + m;
}

void scene::update_positions(float time) {
    for (int i = 0; i < n_balls; ++i) {
        positions[i] = initial_positions[i] + velocities[i] * time;
        positions[i].x = mod(positions[i].x, -1.f, 1.f);
        positions[i].y = mod(positions[i].y, -1.f, 1.f);
        positions[i].z = mod(positions[i].z, -1.f, 1.f);
    }
}


function_texture::function_texture() {
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

void function_texture::calculate(scene& scene) {
    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, texture);

    glBindImageTexture(0, texture, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);

    glUniform3fv(glGetUniformLocation(program, "positions"), n_balls, reinterpret_cast<float*>(scene.positions));
    glUniform3fv(glGetUniformLocation(program, "colors"), n_balls, reinterpret_cast<float*>(scene.colors));
    glUniform1fv(glGetUniformLocation(program, "radiuses"), n_balls, scene.radiuses);

    glDispatchCompute(grid_size, grid_size, grid_size);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}