#version 450

// The color attachment
layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputTexture;

// Input UVs
layout(location = 0) in vec2 inUV;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;

// Just copy input color attachment
void main() {
   /* // Color input
    vec3 inputTextureColor = subpassLoad(inputTexture).rgb;
    outColor = vec4(inputTextureColor, 1.0f);*/

    // Depth input
    float depth = subpassLoad(inputTexture).r;
    float min = 0.0f;
    float max = 1.0f;
    rgbColor = vec3((depth - min) * 1.0 / (max - min));
    outColor = vec4(rgbColor, 1.0f);
}