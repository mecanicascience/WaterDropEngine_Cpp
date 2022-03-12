#version 460

// Input vertices
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;

// Output fragment UV
layout (location = 0) out vec3 outUV;


// Camera set
layout(set = 0, binding = 0) uniform SceneBuffer {
    mat4 transformCameraSpace;  // Matrix from world space to camera space
    mat4 transformProjSpace;    // Matrix from camera space to projection frustum space
} inSceneData;

// Objects set
struct ObjectData {
    mat4 model;
    float collisionSphereR;
};
layout(std140, set = 0, binding = 1) readonly buffer ObjectBuffer {
    ObjectData objects[];
} inObjectBuffer;


// Executed once for each vertex
void main() {
    // Remove culling from camera
    mat4 viewStatic = mat4(inSceneData.transformCameraSpace);
    viewStatic[3][0] = 0.0f;
    viewStatic[3][1] = 0.0f;
    viewStatic[3][2] = 0.0f;

    mat4 transformProjSpaceStatic = mat4(inSceneData.transformProjSpace);
    transformProjSpaceStatic[2][2] = 1.0f;
    transformProjSpaceStatic[3][2] = 0.0f;

    // Computes world space position
    vec4 positionWorldSpace = inObjectBuffer.objects[gl_BaseInstance].model * vec4(vPosition, 1.0); // To world space position
    gl_Position = transformProjSpaceStatic // To Vulkan frustum position
                * viewStatic               // To Camera space position
                * positionWorldSpace;      // Object world space position

    // Output UVs
    outUV = vPosition;
}
