#version 330 core

layout (location = 0) in ivec3 in_coord;

void main() {
    gl_Position = vec4(in_coord, 1.0);
}