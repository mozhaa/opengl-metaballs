#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform sampler3D field;
uniform int grid_size;
uniform float target_value;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

in int tetrahydron_type[];
in int id0[];
in int id1[];
in int id2[];

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
    emit_on_edge(p2, n1);
    emit_on_edge(p1, n2);
    emit_on_edge(p2, n2);
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
    int vertices_positivity = (
        (1 << 0) * int(D.texvalue.w > 0) +
        (1 << 1) * int(C.texvalue.w > 0) +
        (1 << 2) * int(B.texvalue.w > 0) +
        (1 << 3) * int(A.texvalue.w > 0)
    );

    switch (vertices_positivity) {
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


void main() {   
    float x0 = (float(id0[0]) + 0.5) / grid_size;
    float x1 = (float(id0[0]) + 1.5) / grid_size;
    float y0 = (float(id1[0]) + 0.5) / grid_size;
    float y1 = (float(id1[0]) + 1.5) / grid_size;
    float z0 = (float(id2[0]) + 0.5) / grid_size;
    float z1 = (float(id2[0]) + 1.5) / grid_size;

    switch (tetrahydron_type[0]) {
    case 0:
        process_tetrahedron(
            vec3(x0, y0, z0),
            vec3(x1, y1, z1),
            vec3(x1, y1, z0),
            vec3(x1, y0, z0)
        );
        break;
    case 1:
        process_tetrahedron(
            vec3(x0, y0, z0),
            vec3(x1, y1, z1),
            vec3(x1, y0, z0),
            vec3(x1, y0, z1)
        );
        break;
    case 2:
        process_tetrahedron(
            vec3(x0, y0, z0),
            vec3(x1, y1, z1),
            vec3(x1, y0, z1),
            vec3(x0, y0, z1)
        );
        break;
    case 3:
        process_tetrahedron(
            vec3(x0, y0, z0),
            vec3(x1, y1, z1),
            vec3(x0, y0, z1),
            vec3(x0, y1, z1)
        );
        break;
    case 4:
        process_tetrahedron(
            vec3(x0, y0, z0),
            vec3(x1, y1, z1),
            vec3(x0, y1, z1),
            vec3(x0, y1, z0)
        );
        break;
    case 5:
        process_tetrahedron(
            vec3(x0, y0, z0),
            vec3(x1, y1, z1),
            vec3(x0, y1, z0),
            vec3(x1, y1, z0)
        );
        break;
    }
}  