#version 450

// Simple triangle hardcoded vertices
vec2 positions[3] = vec2[] (
    vec2( 0.0, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
);

// Executed once for each vertex
void main() {
    // (gl_Position goes from (-1, -1) to (1, 1))
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0); // vec2 (x,y), zPadding, divider
}
