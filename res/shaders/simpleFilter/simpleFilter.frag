#version 450

// Input values from the vertex shader
layout(location = 0) in vec3 inFragColor;

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputTexture;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;

// Ran for every fragment (line between two vertex, after rasterisation)
void main() {
    //outColor = vec4(inFragColor, 1.0f);
    vec3 texColor = subpassLoad(inputTexture).rgb;
    if (texColor != vec3(0.0f, 0.0f, 0.0f))
        outColor = vec4(texColor, 1.0f);
    else
        outColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}