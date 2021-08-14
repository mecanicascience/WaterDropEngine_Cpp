#version 450

// Input material set
layout(set = 2, binding = 0) uniform sampler2D inTexture;

// Scene set values
layout(set = 0, binding = 0) uniform SceneBuffer {
    mat4 transformCameraSpace;
    mat4 transformProjSpace;
    float nearPlane;
    float farPlane;
} inSceneData;

// Input values from the vertex shader
layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in float inLightIntensity;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;

// Input values from the pipeline
layout(push_constant) uniform Push {
    vec3 ambientLightVector;
    float ambientValue;
    bool showDepth;
} inPush;



// Linearize the depth value between the depth near and far plane
float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * inSceneData.nearPlane * inSceneData.farPlane) / (inSceneData.farPlane + inSceneData.nearPlane - z * (inSceneData.farPlane - inSceneData.nearPlane));
}

// Ran for every fragment (line between two vertex, after rasterisation)
void main() {
    if (inPush.showDepth == true) {
        // Show depth
        outColor = vec4(vec3(linearizeDepth(gl_FragCoord.z) / inSceneData.farPlane), 1.0);
    }
    else {
        // Show texture
        vec4 textureColor = texture(inTexture, inTexCoord);
        outColor = vec4(textureColor.xyz * inLightIntensity, 1.0f);
    }
}