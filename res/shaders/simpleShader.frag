#version 450

// Shader Input
layout(location = 0) in vec3 inFragColor;

// Pipeline Output
layout(location = 0) out vec4 outColor;


// For every fragment
void main() {
    outColor = vec4(inFragColor, 1.0);
}
