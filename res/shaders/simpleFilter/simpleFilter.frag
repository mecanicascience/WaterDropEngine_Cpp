#version 450

// The color attachment
layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputTexture;

// Input UVs
layout(location = 0) in vec2 inUV;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;

// Ran for every fragment (line between two vertex, after rasterisation)
void main() {
    /*vec3 texColor = subpassLoad(inputTexture).rgb;
    if (texColor != vec3(0.0f, 0.0f, 0.0f))
        outColor = vec4(1.0f - texColor, 1.0f);
    else
        outColor = vec4(texColor, 0.0f);*/

    vec3 inputTextureColor = subpassLoad(inputTexture).rgb;
    if (inUV.x > 0.5f)
        outColor = vec4(inputTextureColor, 1.0f);
    else
        outColor = vec4(1.0f - inputTextureColor, 1.0f);
}