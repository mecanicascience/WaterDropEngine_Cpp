#version 450

// Input values from the vertex shader
layout(location = 0) in vec3 inFragColor;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;


// Ran for every fragment (line between two vertex, after rasterisation)
void main() {
    outColor = vec4(inFragColor, 1.0f); // r, g, b, a
}