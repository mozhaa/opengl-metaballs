#version 330 core

layout (location = 0) out vec4 out_color;

uniform sampler3D field_normals;

vec3 ambient_strength = vec3(0.4);
vec3 diffuse_strength = vec3(0.5);
vec3 specular_strength = vec3(0.7);

float power = 2.0;

vec3 sun_direction = vec3(0.0, 1.0, 0.0);
vec3 sun_color = vec3(0.9, 0.5, 0.8);

uniform vec3 camera_position;

in vec4 color;
in vec3 position;
in vec3 texcoord;

void main() {
    vec3 normal = normalize(texture(field_normals, texcoord).xyz);
    vec3 albedo = color.xyz;

    float cosine = dot(normal, normalize(sun_direction));
    float light_factor = max(0.0, cosine);
    vec3 reflect_dir = 2.0 * normal * cosine - normalize(sun_direction);
    
    vec3 view_dir = normalize(camera_position - position);

    vec3 ambient_light = albedo * ambient_strength;
    vec3 diffuse_light = sun_color * albedo * light_factor;
    vec3 specular_light = specular_strength * sun_color * pow(max(0.0, dot(reflect_dir, view_dir)), power);

    out_color = vec4(ambient_light + diffuse_light + specular_light, 1.0);
}