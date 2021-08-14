#version 450

// Scene set values
layout(set = 0, binding = 0) uniform SceneBuffer {
    mat4 transformCameraSpace;
    mat4 transformProjSpace;
    float nearPlane;
    float farPlane;
} inSceneData;

// Game object set values
layout(set = 1, binding = 0) uniform GameObjectBuffer {
    mat4 transformWorldSpace;
    mat4 normalMatrix;
} inGameObjectData;


// Input values from the pipeline
layout(push_constant) uniform Push {
    vec3 ambientLightVector;
    float ambientValue;
    bool showDepth;
} inPush;


// Input values from the pipeline
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;


// Output values to the fragment shader
layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out float outLightIntensity;


// Executed once for each vertex
void main() {
    gl_Position = inSceneData.transformProjSpace        // To Vulkan frustum position
                * inSceneData.transformCameraSpace      // To Camera space position
                * inGameObjectData.transformWorldSpace  // To World space position
                * vec4(inPosition, 1.0f);               // Object local space position

    // UV coords
    outTexCoord = inTexCoord;

    // Using ambiant color values
    vec3 normalWorldSpace = normalize(mat3(inGameObjectData.normalMatrix) * inNormal);
    float lightIntensity = max(dot(normalWorldSpace, inPush.ambientLightVector), 0);
    outLightIntensity = inPush.ambientValue + lightIntensity;
}