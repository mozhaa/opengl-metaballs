#include "grid3d.hpp"

#include <glm/ext/matrix_transform.hpp>

#include "shaderload.h"

grid3d::grid3d(int grid_size) : grid_size(grid_size), vertices(), model(1.f), scale(1.f) {
    vertices.reserve((grid_size - 1) * (grid_size - 1) * (grid_size - 1));

    for (int i = 0; i < grid_size - 1; ++i)
        for (int j = 0; j < grid_size - 1; ++j)
            for (int k = 0; k < grid_size - 1; ++k)
                vertices.push_back({(i + 0.5f) / grid_size, (j + 0.5f) / grid_size, (k + 0.5f) / grid_size});

    program = create_program({
        std::string(SHADERS_DIR) + "/grid_draw.vert",
        std::string(SHADERS_DIR) + "/grid_draw.geom",
        std::string(SHADERS_DIR) + "/grid_draw.frag",
    });

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const void *)(0));
}

void grid3d::draw(metaballs_texture &field, camera_settings &camera) {
    glUseProgram(program);

    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, field.texture);

    glUniform1i(glGetUniformLocation(program, "field"), 0);
    glUniform1i(glGetUniformLocation(program, "grid_size"), grid_size);
    glUniform1f(glGetUniformLocation(program, "target_value"), 0.5f);
    
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.view));
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, reinterpret_cast<float *>(&model));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.projection));

    glDrawArrays(GL_POINTS, 0, vertices.size());
}

void grid3d::update() {
    model = glm::mat4(1.f) * scale;
}