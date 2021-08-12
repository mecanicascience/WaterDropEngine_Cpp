#version 450

// Input material set
layout(set = 2, binding = 0) uniform sampler2D inTexture;

// Input values from the vertex shader
layout(location = 0) in vec3 inFragColor;
layout(location = 1) in vec2 inTexCoord;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;


// Ran for every fragment (line between two vertex, after rasterisation)
void main() {
    outColor = vec4(texture(inTexture, inTexCoord).xyz, 1.0f);
}