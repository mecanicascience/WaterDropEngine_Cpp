#version 460

// Input vertices
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;


// Camera set
layout(set = 0, binding = 0) uniform SceneBuffer {
    mat4 transformCameraSpace;  // Matrix from world space to camera space
    mat4 transformProjSpace;    // Matrix from camera space to projection frustum space
} inSceneData;

// Objects set
struct ObjectData {
    mat4 model;
};
layout(std140, set = 0, binding = 1) readonly buffer ObjectBuffer {
    ObjectData objects[];
} inObjectBuffer;


// Output values to the fragment shader
layout(location = 0) out vec2 outTexCoord;



// Executed once for each vertex
void main() {
    // Computes world space position
    vec4 positionWorldSpace = inObjectBuffer.objects[gl_BaseInstance].model * vec4(vPosition, 1.0); // To world space position
    gl_Position = inSceneData.transformProjSpace    // To Vulkan frustum position
                * inSceneData.transformCameraSpace  // To Camera space position
                * positionWorldSpace;               // Object world space position

    // UV coords
    outTexCoord = vUV;
}
