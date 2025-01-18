#version 330 core

uniform int grid_size;

int N = grid_size - 1;

out int tetrahydron_type;
out int id0;
out int id1;
out int id2;

void main() {
    int vid = gl_VertexID;
    
    tetrahydron_type = vid % 6;
    vid /= 6;

    id2 = vid % N;
    vid /= N;

    id1 = vid % N;
    vid /= N;

    id0 = vid % N;
    vid /= N;
}