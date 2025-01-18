#include "box_drawer.hpp"

#include "shaderload.h"

namespace metaballs {

box_drawer::box_drawer(glm::vec3 a, glm::vec3 b) {
    program = create_program({
        std::string(SHADERS_DIR) + "/box_draw.vert",
        std::string(SHADERS_DIR) + "/box_draw.frag",
    });

    cube_edges[0] =     glm::vec3(a.x, a.y, a.y);
    cube_edges[1] =     glm::vec3(b.x, a.y, a.y);

    cube_edges[2] =     glm::vec3(a.x, a.y, b.y);
    cube_edges[3] =     glm::vec3(b.x, a.y, b.y);

    cube_edges[4] =     glm::vec3(a.x, b.y, a.y);
    cube_edges[5] =     glm::vec3(b.x, b.y, a.y);

    cube_edges[6] =     glm::vec3(a.x, b.y, b.y);
    cube_edges[7] =     glm::vec3(b.x, b.y, b.y);


    cube_edges[8] =     glm::vec3(a.x, a.y, a.y);
    cube_edges[9] =     glm::vec3(a.x, b.y, a.y);

    cube_edges[10] =    glm::vec3(a.x, a.y, b.y);
    cube_edges[11] =    glm::vec3(a.x, b.y, b.y);

    cube_edges[12] =    glm::vec3(b.x, a.y, a.y);
    cube_edges[13] =    glm::vec3(b.x, b.y, a.y);

    cube_edges[14] =    glm::vec3(b.x, a.y, b.y);
    cube_edges[15] =    glm::vec3(b.x, b.y, b.y);


    cube_edges[16] =    glm::vec3(a.x, a.y, a.y);
    cube_edges[17] =    glm::vec3(a.x, a.y, b.y);

    cube_edges[18] =    glm::vec3(a.x, b.y, a.y);
    cube_edges[19] =    glm::vec3(a.x, b.y, b.y);

    cube_edges[20] =    glm::vec3(b.x, a.y, a.y);
    cube_edges[21] =    glm::vec3(b.x, a.y, b.y);

    cube_edges[22] =    glm::vec3(b.x, b.y, a.y);
    cube_edges[23] =    glm::vec3(b.x, b.y, b.y);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec3), cube_edges, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));
}

void box_drawer::draw(camera_settings& camera, glm::mat4& model) {
    glUseProgram(program);
    
    glDisable(GL_DEPTH_TEST);

    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.view));
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, reinterpret_cast<float *>(&model));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, reinterpret_cast<float *>(&camera.projection));

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 24);
}

} // namespace metaballs