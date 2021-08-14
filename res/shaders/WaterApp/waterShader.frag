#version 450

// Vertex input color
layout(location = 0) in vec3 inFragColor;

// Fragment output color
layout(location = 0) out vec4 outColor;


void main() {
    outColor = vec4(inFragColor, 1.0f);
}