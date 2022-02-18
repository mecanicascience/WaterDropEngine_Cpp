#version 450

// Input vertices
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

// Output color to fragment shader
layout (location = 0) out vec3 outColor;

// Executed once for each vertex
void main() {
    // (gl_Position goes from (-1, -1) to (1, 1))
    gl_Position = vec4(vPosition, 1.0f);
    outColor = vColor;
}
