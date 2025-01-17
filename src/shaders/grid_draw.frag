#version 330 core

layout (location = 0) out vec4 out_color;

in vec3 color;

void main() {
    // out_color = vec4(1.0, 0.0, 0.0, 1.0);
    out_color = vec4(color, 1.0);
}