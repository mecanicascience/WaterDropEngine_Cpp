#version 450

// Input vertices
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;
layout (location = 3) in vec2 vUV;

// Output color to fragment shader
layout (location = 0) out vec3 outColor;


// In data
layout(set = 0, binding = 0) uniform SceneBuffer {
    mat4 camView;
    mat4 camProj;
    vec4 matColor;
} inSceneData;

void main() {
    // Computes world space position
    vec4 positionWorldSpace = vec4(vPosition, 1.0); // To world space position
    gl_Position = inSceneData.camProj    // To Vulkan frustum position
                * inSceneData.camView    // To Camera space position
                * positionWorldSpace;    // Object world space position

    // Vertex color
    outColor = inSceneData.matColor.xyz;
}
