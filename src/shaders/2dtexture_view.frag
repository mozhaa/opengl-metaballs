#version 330 core

uniform sampler3D tex;

in vec2 texcoord;

layout (location = 0) out vec4 out_color;

void main() {
    // out_color = vec4(1.0, 0.0, 0.0, 1.0);
    out_color = vec4(texture(tex, vec3(texcoord, 0.1)).xyz, 1.0);
}