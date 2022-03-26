#version 460

// Input vertex pixel color and normal
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inNormal;

// Output fragment pixel color
layout (location = 0) out vec4 outColor;

void main() {
    // Default light direction
    vec3 lightDir = normalize(vec3(1.0, 1.0, 0.0));
    float emissivity = 0.1;

    // Use vertex input color
    outColor = vec4(inColor * dot(inNormal, lightDir) + emissivity, 1.0);
}
