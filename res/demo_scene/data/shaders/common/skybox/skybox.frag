#version 460

// Texture image
layout(set = 1, binding = 0) uniform samplerCube inTexture;

// Input vertex UVs
layout (location = 0) in vec3 inUV;

// Output fragment pixel color
layout (location = 0) out vec4 outColor;


void main() {
    outColor = texture(inTexture, inUV);
}
