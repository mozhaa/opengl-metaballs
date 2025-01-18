#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

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
    
    color = (vx.texvalue.xyz * x + vy.texvalue.xyz * y) / s;
    gl_Position = (vx.position * x + vy.position * y) / s;
    EmitVertex();
}

void process_1000(V va, V vb, V vc, V vd) {
    emit_on_edge(va, vb);
    emit_on_edge(va, vc);
    emit_on_edge(va, vd);
    EndPrimitive();
}

void process_1100(V va, V vb, V vc, V vd) {
    emit_on_edge(va, vc);
    emit_on_edge(va, vd);
    emit_on_edge(vb, vd);
    EndPrimitive();
    emit_on_edge(va, vd);
    emit_on_edge(vb, vd);
    emit_on_edge(vb, vc);
    EndPrimitive();
}

vec4 transform(vec3 point) {
    return projection * view * model * vec4(point, 1.0);
}

void process_tetrahedron(vec3 va, vec3 vb, vec3 vc, vec3 vd) {
    V A = V(texture(field, va), transform(va));
    V B = V(texture(field, vb), transform(vb));
    V C = V(texture(field, vc), transform(vc));
    V D = V(texture(field, vd), transform(vd));

    bool a = A.texvalue.w > target_value;
    bool b = B.texvalue.w > target_value;
    bool c = C.texvalue.w > target_value;
    bool d = D.texvalue.w > target_value;

    if (a) {
        if (b) {
            if (c) {
                if (d) {
                } else {
                    process_1000(D, A, B, C);
                }
            } else {
                if (d) {
                    process_1000(C, A, B, D);
                } else {
                    process_1100(C, D, A, B);
                }
            }
        } else {
            if (c) {
                if (d) {
                    process_1000(B, A, C, D);
                } else {
                    process_1100(B, D, A, C);
                }
            } else {
                if (d) {
                    process_1000(B, C, A, D);
                } else {
                    process_1100(A, B, C, D);
                }
            }
        }
    } else {
        if (b) {
            if (c) {
                if (d) {
                    process_1000(A, B, C, D);
                } else {
                    process_1100(A, D, B, C);
                }
            } else {
                if (d) {
                    process_1100(A, C, B, D);
                } else {
                    process_1000(B, A, C, D);
                }
            }
        } else {
            if (c) {
                if (d) {
                    process_1100(A, B, C, D);
                } else {
                    process_1000(C, A, B, D);
                }
            } else {
                if (d) {
                    process_1000(D, A, B, C);
                } else {
                }
            }
        }
    }
}

void main() {   
    vec3 v0 = gl_in[0].gl_Position.xyz; 
    vec3 v1 = v0 + vec3(1.0 / grid_size); 
    float x0 = v0.x;
    float x1 = v1.x;
    float y0 = v0.y;
    float y1 = v1.y;
    float z0 = v0.z;
    float z1 = v1.z;

    vec3 p000 = vec3(x0, y0, z0);
    vec3 p001 = vec3(x0, y0, z1);
    vec3 p010 = vec3(x0, y1, z0);
    vec3 p011 = vec3(x0, y1, z1);
    vec3 p100 = vec3(x1, y0, z0);
    vec3 p101 = vec3(x1, y0, z1);
    vec3 p110 = vec3(x1, y1, z0);
    vec3 p111 = vec3(x1, y1, z1);

    process_tetrahedron(p000, p001, p010, p110);
    process_tetrahedron(p011, p001, p010, p110);
    process_tetrahedron(p011, p001, p111, p110);
    process_tetrahedron(p101, p001, p111, p110);
    process_tetrahedron(p101, p001, p100, p110);
    process_tetrahedron(p000, p001, p100, p110);
}  