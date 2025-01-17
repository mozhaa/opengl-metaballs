#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/vec3.hpp>

#include "metaball.hpp"

struct grid3d {
    int grid_size;

    struct vertex {
        int i;
        int j;
        int k;
    };
    std::vector<vertex> vertices;

    GLuint VAO, VBO, program;

    grid3d(int grid_size);
    void draw(metaballs_texture &field) const;
};