#version 450

// Input values from the vertex shader
/*layout(location = 0) in vec3 fragColor;
layout(push_constant) uniform Push {
    mat2 transform;
    vec2 offset;
    vec3 color;
} inPush;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;

// Run for every fragment (line between two vertex, after rasterisation)
void main() {
    outColor = vec4(fragColor, 1.0); // r, g, b, a
}*/

layout (location = 0) in vec3 fragColor;
layout (location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}
