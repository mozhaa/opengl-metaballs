#include "metaball.hpp"

#include <algorithm>
#include <cmath>
#include <random>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/mat4x4.hpp>

#include "shaderload.h"

namespace metaballs {

metaballs_collection::metaballs_collection() {
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist_positive(0.f, 1.f);
    std::uniform_real_distribution<> dist_symmetrical(-1.f, 1.f);

    glm::vec3 mix_color = {1.f, 0.9f, 0.9f};
    float mix_color_coeff = 0.2;

    for (int i = 0; i < n_balls; ++i) {
        radiuses[i] = dist_positive(e2) * 0.1f + 0.1f;
        colors[i] = glm::vec3{dist_positive(e2), dist_positive(e2), dist_positive(e2)};
        colors[i] = colors[i] * (1 - mix_color_coeff) + mix_color * mix_color_coeff;

        scale[i] = glm::vec3(dist_positive(e2), dist_positive(e2), dist_positive(e2)) * 0.4f;
        shift[i] = glm::vec3(dist_symmetrical(e2), dist_symmetrical(e2), dist_symmetrical(e2)) * (1.f - 2.f * (2.f * scale[i] + radiuses[i] * 3)) * 0.6f;
        time_shift[i] = glm::vec3(dist_positive(e2), dist_positive(e2), dist_positive(e2)) * (float)M_PI;

        angular_velocities[i] = sqrt(dist_positive(e2)) * 4.f;
    }

    update_positions(0);
}

void metaballs_collection::update_positions(float time) {
    for (int i = 0; i < n_balls; ++i) {
        float theta = time * angular_velocities[i];
        positions[i] = scale[i] * sin(theta + time_shift[i]) + shift[i];
    }
}

} // namespace metaballs