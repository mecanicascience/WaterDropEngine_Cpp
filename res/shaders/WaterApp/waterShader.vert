#version 450


// Scene set values
layout(set = 0, binding = 0) uniform SceneBuffer {
    mat4 transformCameraSpace;  // Matrix from world space to camera space
    mat4 transformProjSpace;    // Matrix from camera space to projection frustum space
} inSceneData;

// Game object set values
layout(set = 1, binding = 0) uniform GameObjectBuffer {
    mat4 transformWorldSpace;   // To world space GO position
    mat4 normalMatrix;          // Corresponds to transpose(inverse(transformWorldSpace))
} inGameObjectData;


// Input values from the pipeline
layout(push_constant) uniform Push {
    vec3 lightDirection;
} inPush;


// Vertex description
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;


// Output values to the fragment shader
layout(location = 0) out vec3 outFragColor;


void main() {
    // Computes world space position
    vec4 positionWorldSpace = inGameObjectData.transformWorldSpace * vec4(inPosition, 1.0); // To world space position
    gl_Position = inSceneData.transformProjSpace    // To Vulkan frustum position
                * inSceneData.transformCameraSpace  // To Camera space position
                * positionWorldSpace;               // Object world space position

    // Vertex color
    outFragColor = inColor;
}
