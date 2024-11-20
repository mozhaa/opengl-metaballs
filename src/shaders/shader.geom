#version 330

in vec2 size[];

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec2 texcoord;

void main() {
    vec2 center = gl_in[0].gl_Position.xy;
    vec2 size = size[0];

    gl_Position = vec4(center + vec2(-size.x, -size.y), 0.0, 1.0);
    texcoord = vec2(0.0, 0.0);
    EmitVertex();

    gl_Position = vec4(center + vec2(-size.x, size.y), 0.0, 1.0);
    texcoord = vec2(0.0, 1.0);
    EmitVertex();

    gl_Position = vec4(center + vec2(size.x, -size.y), 0.0, 1.0);
    texcoord = vec2(1.0, 0.0);
    EmitVertex();

    gl_Position = vec4(center + vec2(size.x, size.y), 0.0, 1.0);
    texcoord = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}