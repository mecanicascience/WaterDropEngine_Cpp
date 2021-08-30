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
} inPush;


// The waves height buffer
layout(std430, set = 2, binding = 0) readonly buffer ObjectBuffer {
    float wavesHeight[];
} inObjectBuffer;


// Input values from the pipeline
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;


// The vertex light intensity
layout(location = 0) out float outLightIntensity;


// Executed once for each vertex
void main() {
    float normalizedWaveHeight = inObjectBuffer.wavesHeight[gl_VertexIndex] / 10.0;
    float offset = 1.0;
    vec3 waveVec = vec3(0.0, normalizedWaveHeight - offset, 0.0);

    gl_Position = inSceneData.transformProjSpace        // To Vulkan frustum position
                * inSceneData.transformCameraSpace      // To Camera space position
                * inGameObjectData.transformWorldSpace  // To World space position
                * vec4(inPosition + waveVec, 1.0f);  // Object local space position * Wave height

    vec3 normalWorldSpace = normalize(mat3(inGameObjectData.normalMatrix) * normalize(inNormal + waveVec));
    outLightIntensity = 0.2 + max(dot(normalWorldSpace, inPush.ambientLightVector), 0);
}