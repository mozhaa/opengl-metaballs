#include "grid3d.hpp"

#include "shaderload.h"

grid3d::grid3d(int grid_size) : grid_size(grid_size), vertices() {
    vertices.reserve((grid_size - 1) * (grid_size - 1) * (grid_size - 1));

    for (int i = 0; i < grid_size - 1; ++i)
        for (int j = 0; j < grid_size - 1; ++j)
            for (int k = 0; k < grid_size - 1; ++k)
                vertices.push_back({i, j, k});

    program = create_program({
        std::string(SHADERS_DIR) + "/grid_draw.vert",
        std::string(SHADERS_DIR) + "/grid_draw.geom",
        std::string(SHADERS_DIR) + "/grid_draw.frag",
    });

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, sizeof(vertex), (const void *)(0));
}

void grid3d::draw(metaballs_texture &field) const {
    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, field.texture);
    glUniform1i(glGetUniformLocation(program, "field"), 0);
    glUniform1i(glGetUniformLocation(program, "grid_size"), grid_size);
    glUniform1f(glGetUniformLocation(program, "target_value"), 0.5f);

    glDrawArrays(GL_POINTS, 0, vertices.size());
}