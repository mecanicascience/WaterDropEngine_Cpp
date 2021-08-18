#version 450


// Input depth value
layout(location = 0) in float inDepthValue;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;


void main() {
    outColor = vec4(vec3(inDepthValue), 1.0);
}