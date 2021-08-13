#version 450

// The color attachment
layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputTexture;

// Input UVs
layout(location = 0) in vec2 inUV;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;

// Invert input color attachment
void main() {
    vec3 inputTextureColor = subpassLoad(inputTexture).rgb;
    outColor = vec4(1.0f - inputTextureColor, 1.0f);
}