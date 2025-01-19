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
#include "hsv2rgb.hpp"

namespace metaballs {

template <class rng>
float sample_in_range(rng& r, std::mt19937& e2, float m, float M) {
    float result;
    do {
        result = r(e2);
    } while (result > M || result < m);
    return result;
}

glm::vec3 random_color(std::mt19937& e2) {
    std::normal_distribution<> h(0.f, 50.f);
    std::normal_distribution<> s(0.8f, 0.01f);
    std::normal_distribution<> v(0.9f, 0.01f);

    return HSVtoRGB({
        fmod(h(e2) + 360.f, 360.f),
        sample_in_range(s, e2, 0.f, 1.f),
        sample_in_range(v, e2, 0.f, 1.f)
    });
}

metaballs_collection::metaballs_collection() {
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist_positive(0.f, 1.f);
    std::uniform_real_distribution<> dist_symmetrical(-1.f, 1.f);

    for (int i = 0; i < n_balls; ++i) {
        radiuses[i] = dist_positive(e2) * 0.05f + 0.03f;
        colors[i] = random_color(e2);

        scale[i] = glm::vec3(dist_positive(e2), dist_positive(e2), dist_positive(e2)) * 0.4f;
        shift[i] = glm::vec3(dist_symmetrical(e2), dist_symmetrical(e2), dist_symmetrical(e2)) * (1.f - 2.f * (2.f * scale[i] + radiuses[i] * 3));
        time_shift[i] = glm::vec3(dist_positive(e2), dist_positive(e2), dist_positive(e2)) * (float)M_PI;

        angular_velocities[i] = sqrt(dist_positive(e2)) * 2.f;
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