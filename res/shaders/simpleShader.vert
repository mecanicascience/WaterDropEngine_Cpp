#version 450

/*
// Input values from the pipeline
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(push_constant) uniform Push {
    mat2 transform;
    vec2 offset;
    vec3 color;
} inPush;

// Output values to the fragment shader
layout(location = 0) out vec3 fragColor;


// Executed once for each vertex
void main() {
    gl_Position = vec4(inPush.transform * inPosition + inPush.offset, 0.0, 1.0); // vec2 (x,y), zPadding, divider
    fragColor = inColor;
}*/

layout(location = 0) out vec3 fragColor;

// Simple triangle
vec2 positions[3] = vec2[] (
    vec2( 0.0, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
);

// To make a beautiful color shading
vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

// Executed once for each vertex
void main() {
    // (from (-1, -1) to (1, 1))
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0); // vec2 (x,y), zPadding, divider

    // Auto interpolate colors between vertices
    fragColor = colors[gl_VertexIndex];
}