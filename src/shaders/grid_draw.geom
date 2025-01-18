#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 90) out;

uniform sampler3D field;
uniform int grid_size;
uniform float target_value;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

out vec3 color;

struct V {
    vec4 texvalue;
    vec4 position;
};

void emit_on_edge(V vx, V vy) {
    float x = abs(vx.texvalue.w);
    float y = abs(vy.texvalue.w);
    float s = x + y;
    
    color = (vx.texvalue.xyz * y + vy.texvalue.xyz * x) / s;
    gl_Position = (vx.position * y + vy.position * x) / s;
    EmitVertex();
}

void process1000(V p1, V n1, V n2, V n3) {
    // triangle must be back-faced from p1 perspective 
    // (we assume that camera is watching from {function < target_value} space)
    emit_on_edge(p1, n1);
    emit_on_edge(p1, n3);
    emit_on_edge(p1, n2);
    EndPrimitive();
}

void process1100(V p1, V p2, V n1, V n2) {
    emit_on_edge(p1, n1);
    emit_on_edge(p2, n2);
    emit_on_edge(p1, n2);
    EndPrimitive();
    emit_on_edge(p2, n2);
    emit_on_edge(p1, n1);
    emit_on_edge(p2, n1);
    EndPrimitive();
}

void process1110(V p1, V p2, V p3, V n1) {
    emit_on_edge(n1, p1);
    emit_on_edge(n1, p3);
    emit_on_edge(n1, p2);
    EndPrimitive();
}

vec4 as_actual_point(vec3 point) {
    return projection * view * model * vec4(point, 1.0);
}

V from_cube_vertex(vec3 v) {
    V result = V(texture(field, v), as_actual_point(v));
    result.texvalue.w -= target_value;
    return result;
}

void process_tetrahedron(vec3 va, vec3 vb, vec3 vc, vec3 vd) {
    V A = from_cube_vertex(va);
    V B = from_cube_vertex(vb);
    V C = from_cube_vertex(vc);
    V D = from_cube_vertex(vd);

    // f.e. 0011 -- A,B - negative, C,D - positive
    int tetrahydron_type = (
        (1 << 0) * int(D.texvalue.w > 0) +
        (1 << 1) * int(C.texvalue.w > 0) +
        (1 << 2) * int(B.texvalue.w > 0) +
        (1 << 3) * int(A.texvalue.w > 0)
    );

    switch (tetrahydron_type) {
    // all process####(X1, X2, X3, X4) have (X1, X2, X3, X4) in correct orientation
    case 0:
        // process0000(A, B, C, D);
        break;
    case 1:
        process1000(D, B, A, C);
        break;
    case 2:
        process1000(C, A, B, D);
        break;
    case 3:
        process1100(C, D, A, B);
        break;
    case 4:
        process1000(B, C, A, D);
        break;
    case 5:
        process1100(D, B, A, C);
        break;
    case 6:
        process1100(B, C, A, D);
        break;
    case 7:
        process1110(C, B, D, A);
        break;
    case 8:
        process1000(A, B, C, D);
        break;
    case 9:
        process1100(A, D, B, C);
        break;
    case 10:
        process1100(C, A, B, D);
        break;
    case 11:
        process1110(A, C, D, B);
        break;
    case 12:
        process1100(A, B, C, D);
        break;
    case 13:
        process1110(B, A, D, C);
        break;
    case 14:
        process1110(A, B, C, D);
        break;
    case 15:
        // process1111(A, B, C, D);
        break;
    }
}

void process_cube(vec3 p000, vec3 p001, vec3 p010, vec3 p011, vec3 p100, vec3 p101, vec3 p110, vec3 p111) {
    // all tetrahydra have the same orientation 
    process_tetrahedron(p000, p111, p110, p100);
    process_tetrahedron(p000, p111, p100, p101);
    process_tetrahedron(p000, p111, p101, p001);
    process_tetrahedron(p000, p111, p001, p011);
    process_tetrahedron(p000, p111, p011, p010);
    process_tetrahedron(p000, p111, p010, p110);
}

// void draw_line(vec3 a, vec3 b, vec3 line_color, float line_width) {
//     vec4 at = transform(a);
//     vec4 bt = transform(b);
//     vec4 dir = at - bt;
//     vec4 ortho = normalize(vec4(-dir.y, dir.x, 0.0, 0.0)) * line_width;

//     color = line_color;
//     gl_Position = at + ortho;
//     EmitVertex();

//     color = line_color;
//     gl_Position = at - ortho;
//     EmitVertex();

//     color = line_color;
//     gl_Position = bt - ortho;
//     EmitVertex();

//     EndPrimitive();

//     color = line_color;
//     gl_Position = at + ortho;
//     EmitVertex();

//     color = line_color;
//     gl_Position = bt - ortho;
//     EmitVertex();

//     color = line_color;
//     gl_Position = bt + ortho;
//     EmitVertex();
    
//     EndPrimitive();
// }

// void draw_cube(vec3 p000, vec3 p001, vec3 p010, vec3 p011, vec3 p100, vec3 p101, vec3 p110, vec3 p111) {
//     draw_line(p000, p001, vec3(1.0, 0.0, 0.0), 0.01);
//     draw_line(p011, p001, vec3(1.0, 0.0, 0.0), 0.01);
//     draw_line(p011, p010, vec3(1.0, 0.0, 0.0), 0.01);
//     draw_line(p000, p010, vec3(1.0, 0.0, 0.0), 0.01);
//     draw_line(p000, p100, vec3(1.0, 0.0, 0.0), 0.01);
//     draw_line(p110, p100, vec3(1.0, 0.0, 0.0), 0.01);
//     draw_line(p110, p010, vec3(1.0, 0.0, 0.0), 0.01);
//     draw_line(p110, p111, vec3(1.0, 0.0, 0.0), 0.01);
//     draw_line(p101, p111, vec3(1.0, 0.0, 0.0), 0.01);
//     draw_line(p101, p001, vec3(1.0, 0.0, 0.0), 0.01);
//     draw_line(p101, p100, vec3(1.0, 0.0, 0.0), 0.01);
//     draw_line(p011, p111, vec3(1.0, 0.0, 0.0), 0.01);

//     draw_line(p001, p110, vec3(0.2, 0.0, 1.0), 0.007);
// }

void main() {   
    vec3 v0 = gl_in[0].gl_Position.xyz; 
    vec3 v1 = v0 + vec3(1.0 / grid_size); 

    process_cube(
        vec3(v0.x, v0.y, v0.z),
        vec3(v0.x, v0.y, v1.z),
        vec3(v0.x, v1.y, v0.z),
        vec3(v0.x, v1.y, v1.z),
        vec3(v1.x, v0.y, v0.z),
        vec3(v1.x, v0.y, v1.z),
        vec3(v1.x, v1.y, v0.z),
        vec3(v1.x, v1.y, v1.z)
    );
}  