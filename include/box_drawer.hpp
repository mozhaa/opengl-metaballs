#pragma once

#include <GL/glew.h>

#include "camera.hpp"

namespace metaballs {

struct box_drawer {
    GLuint program, VAO, VBO;

    glm::vec3 cube_edges[24];

    box_drawer(glm::vec3 a, glm::vec3 b);
    void draw(camera_settings& camera, glm::mat4& model);
};

} // namespace metaballs
