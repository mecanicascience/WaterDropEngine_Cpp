#version 450

layout(location = 0) out vec3 fragColor;

// Simple triangle
vec2 positions[3] = vec2[] (
    vec2( 0.0, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
);

// To make a beautiful color shading
vec3 colors[3] = vec3[](
    vec3(1.0, 1.0, 1.0),
    vec3(1.0, 0.0, 1.0),
    vec3(1.0, 0.0, 1.0)
);

// Executed once for each vertex
void main() {
    // (from (-1, -1) to (1, 1))
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0); // vec2 (x,y), zPadding, divider

    // Auto interpolate colors between vertices
    fragColor = colors[gl_VertexIndex];
}