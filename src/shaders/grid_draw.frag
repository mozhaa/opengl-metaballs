#version 330 core

layout (location = 0) out vec4 out_color;

vec3 ambient_strength = vec3(0.4);
vec3 diffuse_strength = vec3(0.5);
vec3 specular_strength = vec3(0.3);

float power = 4.0;

vec3 sun_direction = vec3(-1.0, 0.0, 0.0);
vec3 sun_color = vec3(0.9, 0.5, 0.8);

uniform vec3 camera_position;

in vec4 color;
in vec3 normal;
in vec3 position;

float cosine(vec3 dir1, vec3 dir2) {
    return max(0.0, dot(normalize(dir1), normalize(dir2)));
}

void main() {
    vec3 albedo = color.xyz;

    vec3 reflect_dir = reflect(sun_direction, normal);
    vec3 view_dir = normalize(position - camera_position);

    vec3 ambient_light = albedo * ambient_strength;
    vec3 diffuse_light = sun_color * albedo * cosine(normal, sun_direction);
    vec3 specular_light = sun_color * specular_strength * pow(cosine(position - camera_position, reflect(sun_direction, normal)), power);

    out_color = vec4(ambient_light + diffuse_light + specular_light, 1.0);
}