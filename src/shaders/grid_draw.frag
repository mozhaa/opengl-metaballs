#version 330 core

layout (location = 0) out vec4 out_color;

float roughness = 0.3;
float glossiness = 4.0;

vec3 ambient_light = vec3(0.7, 0.7, 0.7);

vec3 sun_direction = vec3(0.0, 0.3, 0.6);
vec3 sun_color = vec3(0.7, 0.2, 0.1);

uniform vec3 camera_position;

in vec4 color;
in vec3 normal;
in vec3 position;

vec3 diffuse(vec3 direction) {
    return color.xyz * max(0.0, dot(normal, direction));
}

vec3 specular(vec3 direction) {
    vec3 view_direction = normalize(camera_position - position);
    vec3 reflected = normalize(2.0 * normal * dot(normal, direction) - direction);
    float power = 1 / (roughness * roughness) - 1;
    return glossiness * color.xyz * pow(max(0.0, dot(reflected, view_direction)), power);
}

void main() {
    vec3 ambient = color.xyz * ambient_light;
    vec3 sun_light = (diffuse(sun_direction) + specular(sun_direction)) * sun_color;  
    // out_color = vec4(normal, 1.0);
    out_color = vec4(diffuse(sun_direction), 1.0);
    // out_color = vec4(ambient + sun_light, 1.0);
}