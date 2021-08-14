#version 450

// Scene set values
layout(set = 0, binding = 0) uniform SceneBuffer {
    mat4 transformCameraSpace;
    mat4 transformProjSpace;
    float nearPlane;
    float farPlane;
} inSceneData;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;


// Linearize the depth value between the depth near and far plane
float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * inSceneData.nearPlane * inSceneData.farPlane) / (inSceneData.farPlane + inSceneData.nearPlane - z * (inSceneData.farPlane - inSceneData.nearPlane));
}

void main() {
    outColor = vec4(vec3(linearizeDepth(gl_FragCoord.z) / inSceneData.farPlane), 1.0);
}