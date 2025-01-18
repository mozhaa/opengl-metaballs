#include "camera.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace metaballs {

camera_settings::camera_settings(int width, int height)
    : width(width),
      height(height),
      base_speed(0.5f),
      speed(0.5f),
      rotation_speed(1.f),
      camera_direction({ glm::pi<float>() / 2, 0.f }),
      camera_position({ -2.f, 0.f, 0.f }),
      near(0.01f),
      far(100.f) {
    update_matrices();
}

inline glm::vec3 dir_to_vec(glm::vec2 dir) {
    return {
        cos(dir.y) * sin(dir.x),
        sin(dir.y),
        cos(dir.y) * cos(dir.x),
    };
}

void camera_settings::update(std::map<SDL_Keycode, bool> &button_down, float dt) {
    if (button_down[SDLK_w])
        camera_position += dir_to_vec(camera_direction) * speed * dt;
    if (button_down[SDLK_s])
        camera_position -= dir_to_vec(camera_direction) * speed * dt;
    if (button_down[SDLK_a]) {
        auto rot_mat = glm::rotate(glm::mat4(1.f), glm::pi<float>() / 2, glm::vec3(0.0, 1.0, 0.0));
        auto dir_vec = dir_to_vec(camera_direction);
        dir_vec = (rot_mat * glm::vec4(dir_vec, 0.0)).xyz();
        dir_vec.y = 0;
        camera_position += dir_vec * speed * dt;
    }
    if (button_down[SDLK_d]) {
        auto rot_mat = glm::rotate(glm::mat4(1.f), glm::pi<float>() / 2, glm::vec3(0.0, 1.0, 0.0));
        auto dir_vec = dir_to_vec(camera_direction);
        dir_vec = (rot_mat * glm::vec4(dir_vec, 0.0)).xyz();
        dir_vec.y = 0;
        camera_position -= dir_vec * speed * dt;
    }

    if (button_down[SDLK_RIGHT])
        camera_direction = {camera_direction.x - rotation_speed * dt, camera_direction.y};
    if (button_down[SDLK_LEFT])
        camera_direction = {camera_direction.x + rotation_speed * dt, camera_direction.y};
    if (button_down[SDLK_DOWN])
        camera_direction = {camera_direction.x, camera_direction.y - rotation_speed * dt};
    if (button_down[SDLK_UP])
        camera_direction = {camera_direction.x, camera_direction.y + rotation_speed * dt};

    if (button_down[SDLK_LSHIFT])
        speed = 2 * base_speed;
    else if (button_down[SDLK_LCTRL])
        speed = base_speed * 0.3;
    else
        speed = base_speed;

    update_matrices();
}

void camera_settings::update_matrices() {
    glm::vec3 direction_vector = dir_to_vec(camera_direction);
    glm::vec3 right_vector = glm::normalize(glm::cross(glm::vec3(0, 1, 0), direction_vector));
    glm::vec3 up_vector = glm::normalize(glm::cross(direction_vector, right_vector));
    view = glm::lookAt(camera_position, camera_position + direction_vector, up_vector);

    projection = glm::perspective(glm::pi<float>() / 2.f, (1.f * width) / height, near, far);

    view_projection_inverse = glm::inverse(projection * view);
}

} // namespace metaballs
