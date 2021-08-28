#version 450

// Output UV
layout(location = 0) out vec2 outUV;

// Executed once for each vertex
void main() {
    // UV of the actual vertex position
    outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(outUV * 2.0f - 1.0f, 0.0f, 1.0f);
}