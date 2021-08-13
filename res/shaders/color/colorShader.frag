#version 450

// Input values from the vertex shader
layout(location = 0) in vec3 inFragColor;

//layout (input_attachment_index = 0, set = 2, binding = 0) uniform subpassInput inputDepth;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;


// Ran for every fragment (line between two vertex, after rasterisation)
void main() {
    //float depth = subpassLoad(inputDepth).r;
    outColor = vec4(inFragColor, 1.0f);
    //outColor = vec4(depth, depth, depth, 1.0f);
}