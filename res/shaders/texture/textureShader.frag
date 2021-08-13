#version 450

// Input material set
layout(set = 2, binding = 0) uniform sampler2D inTexture;

// Input values from the vertex shader
layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in float inLightIntensity;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;



// Ran for every fragment (line between two vertex, after rasterisation)
void main() {
    vec4 textureColor = texture(inTexture, inTexCoord);
    outColor = vec4(textureColor.xyz * inLightIntensity, textureColor.w);
}