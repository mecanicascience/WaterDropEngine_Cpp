#version 450

layout(location = 0) in vec3 fragColor;

// Select framebuffer at index O
layout(location = 0) out vec4 outColor;

// Run for every fragment (line between two vertex, after rasterisation)
void main() {
    outColor = vec4(fragColor, 1.0); // r, g, b, a
}
