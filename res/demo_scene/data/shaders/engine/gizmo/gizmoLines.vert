#version 460

// Input vertices
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;

// Output color to fragment shader
layout (location = 0) out vec3 outColor;


// In data
layout(set = 0, binding = 0) uniform CameraBuffer {
    mat4 camView;
    mat4 camProj;
} inCameraData;

layout(std140, set = 0, binding = 1) readonly buffer ColorBuffer {
    vec4 color[];
} inColor;

void main() {
    // Computes world space position
    vec4 positionWorldSpace = vec4(vPosition, 1.0); // To world space position
    gl_Position = inCameraData.camProj    // To Vulkan frustum position
                * inCameraData.camView    // To Camera space position
                * positionWorldSpace;    // Object world space position

    // Vertex color
    outColor = inColor.color[gl_BaseInstance].xyz;
}
