#include "field.hpp"

#include "shaderload.h"

namespace metaballs {

void generate_texture(GLuint& texture) {
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
}

scalar_field_texture::scalar_field_texture() {
    program = create_program({std::string(SHADERS_DIR) + "/compute_function.comp"});

    generate_texture(values);
    glBindImageTexture(0, values, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    generate_texture(normals);
    glBindImageTexture(1, normals, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void scalar_field_texture::compute(metaballs_collection &balls) {
    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, values);

    glBindImageTexture(0, values, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);

    glUniform3fv(glGetUniformLocation(program, "positions"), n_balls, reinterpret_cast<float *>(balls.positions));
    glUniform3fv(glGetUniformLocation(program, "colors"), n_balls, reinterpret_cast<float *>(balls.colors));
    glUniform1fv(glGetUniformLocation(program, "radiuses"), n_balls, balls.radiuses);

    glDispatchCompute(grid_size, grid_size, grid_size);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

 } // namespace metaballs
 