#version 450

// Output values to the fragment shader
layout(location = 0) out vec3 outFragColor;


// Simple triangle
vec2 positions[3] = vec2[] (
    vec2( 0.0, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
);

// Triangle color shading
vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

// Executed once for each vertex
void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);

    outFragColor = colors[gl_VertexIndex];
}