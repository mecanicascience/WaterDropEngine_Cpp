#version 460

// Input material set
layout(set = 1, binding = 0) uniform sampler2D inTexture;

// Input values from the vertex shader
layout(location = 0) in vec2 inTexCoord;

// Output fragment pixel color
layout (location = 0) out vec4 outColor;



void main() {
    // Show texture
    vec4 textureColor = texture(inTexture, inTexCoord);
    outColor = vec4(textureColor.xyz, 1.0f);
}
