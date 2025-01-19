#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/vec3.hpp>

#include "metaball.hpp"
#include "field.hpp"
#include "camera.hpp"
#include "lighting_settings.hpp"

namespace metaballs {

struct grid3d {
    int grid_size;

    GLuint VAO, VBO, program;

    glm::mat4 model;

    grid3d(int grid_size);
    void draw(scalar_field_texture &field, camera_settings& camera, lighting_settings& lighting, float target_value);
};

} // namespace metaballs
