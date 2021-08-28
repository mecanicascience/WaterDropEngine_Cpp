#version 450

// Input texture
layout(set = 0, binding = 0) uniform sampler2D inTexture;


// Input UV
layout(location = 0) in vec2 inUV;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;


// Just copy input color attachment
void main() {
    // Suppose color input
    vec3 inputTexture = texture(inTexture, inUV).rgb;
    outColor = vec4(inputTexture, 1.0f);
}