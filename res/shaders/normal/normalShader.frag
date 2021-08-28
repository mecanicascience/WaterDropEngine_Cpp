#version 450

// Input color from the vertex shader
layout(location = 0) in vec3 inNormalColor;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(inNormalColor, 1.0);
}