#include "grid3d.hpp"

#include <glm/ext/matrix_transform.hpp>

#include "shaderload.h"

grid3d::grid3d(int grid_size) : grid_size(grid_size), model(1.f), scale(1.f) {
    program = create_program({
        std::string(SHADERS_DIR) + "/grid_draw.vert",
        std::string(SHADERS_DIR) + "/grid_draw.geom",
        std::string(SHADERS_DIR) + "/grid_draw.frag",
    });

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
}

void grid3d::draw(scalar_field_texture &field, camera_settings &camera, float target_value) {
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, field.texture);

    glUniform1i(glGetUniformLocation(program, "field"), 0);
    glUniform1i(glGetUniformLocation(program, "grid_size"), grid_size);
    glUniform1f(glGetUniformLocation(program, "target_value"), target_value);
    
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.view));
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, reinterpret_cast<float *>(&model));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.projection));

    glDrawArrays(GL_POINTS, 0, (grid_size - 1) * (grid_size - 1) * (grid_size - 1) * 6);
}

void grid3d::update() {
    model = glm::mat4(scale);
}