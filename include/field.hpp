#pragma once

#include "metaball.hpp"

const int grid_size = 64;

/**
 * GL Texture, that contains computed values of total metaballs function
 */
struct scalar_field_texture {
    GLuint program, texture;

    scalar_field_texture();
    void compute(metaballs_collection& scene);
};
