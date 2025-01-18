#include "grid3d.hpp"

#include <glm/ext/matrix_transform.hpp>

#include "shaderload.h"

namespace metaballs {

grid3d::grid3d(int grid_size) : grid_size(grid_size) {
    program = create_program({
        std::string(SHADERS_DIR) + "/grid_draw.vert",
        std::string(SHADERS_DIR) + "/grid_draw.geom",
        std::string(SHADERS_DIR) + "/grid_draw.frag",
    });

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    model = glm::translate(glm::scale(glm::mat4(1.f), glm::vec3(5.f)), glm::vec3(-0.5f));
}

void grid3d::draw(scalar_field_texture &field, camera_settings &camera, float target_value) {
    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, field.values);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_3D, field.normals);

    glUniform1i(glGetUniformLocation(program, "field_values"), 0);
    glUniform1i(glGetUniformLocation(program, "field_normals"), 1);
    glUniform1i(glGetUniformLocation(program, "grid_size"), grid_size);
    glUniform1f(glGetUniformLocation(program, "target_value"), target_value);

    glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, reinterpret_cast<float *>(&camera.camera_position));
    
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.view));
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, reinterpret_cast<float *>(&model));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.projection));

    glDrawArrays(GL_POINTS, 0, (grid_size - 1) * (grid_size - 1) * (grid_size - 1) * 6);
}

} // namespace metaballs