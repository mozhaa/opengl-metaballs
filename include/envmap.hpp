#pragma once

#include <GL/glew.h>

#include "camera.hpp"

namespace metaballs {

struct environment_map {
    GLuint texture, program;

    environment_map();
    void draw(camera_settings& camera);
};

} // namespace metaballs
