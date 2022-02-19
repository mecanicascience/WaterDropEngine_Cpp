#version 450

// Input vertices
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

// Output color to fragment shader
layout (location = 0) out vec3 outColor;


// Camera set
layout(set = 0, binding = 0) uniform SceneBuffer {
    mat4 transformCameraSpace;  // Matrix from world space to camera space
    mat4 transformProjSpace;    // Matrix from camera space to projection frustum space
} inSceneData;

// Game object set values
layout(set = 2, binding = 0) uniform GameObjectBuffer {
    mat4 transformWorldSpace;   // To world space GO position
} inGameObjectData;


// Executed once for each vertex
void main() {
    // Computes world space position
    vec4 positionWorldSpace = inGameObjectData.transformWorldSpace * vec4(vPosition, 1.0); // To world space position
    gl_Position = inSceneData.transformProjSpace    // To Vulkan frustum position
                * inSceneData.transformCameraSpace  // To Camera space position
                * positionWorldSpace;               // Object world space position

    // Vertex color
    outColor = vColor;
}
