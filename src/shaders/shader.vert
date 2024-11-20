#version 330

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_size;

out vec2 size;

void main() {
    // gl_Position = vec4(in_position, 0.0, 1.0);
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
    size = in_size;
}