#version 450

// Output vertex pixel color
layout (location = 0) in vec3 inColor;

// Output fragment pixel color
layout (location = 0) out vec4 outColor;

void main() {
    // Use vertex input color
    outColor = vec4(inColor, 1.0);
}
