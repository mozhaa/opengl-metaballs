#include "camera.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

camera_settings::camera_settings(int width, int height)
    : width(width),
      height(height),
      view_elevation(glm::radians(45.f)),
      view_azimuth(0.f),
      camera_distance(1.5f),
      near(0.01f),
      far(10.f) {
    update();
}

void camera_settings::update() {
    view = glm::translate(glm::mat4(1.f), {0.f, 0.f, -camera_distance});
    view = glm::rotate(view, view_elevation, {1.f, 0.f, 0.f});
    view = glm::rotate(view, view_azimuth, {0.f, 1.f, 0.f});

    projection = glm::perspective(glm::pi<float>() / 2.f, (1.f * width) / height, near, far);
}