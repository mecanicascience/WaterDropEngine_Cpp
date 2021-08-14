#version 450

// The depth attachment
layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput depthTexture;

// Input values from the pipeline
layout(push_constant) uniform Push {
    float minDepth;
    float maxDepth;
} inPush;


// Input UVs
layout (location = 0) in vec2 inUV;

// Output to the swapchain value
layout (location = 0) out vec4 outColor;


float linearizeDepth(float depth, float nearVal, float farVal) {
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    // Depth input
    float depth = linearizeDepth(subpassLoad(depthTexture).r, inPush.nearVal, inPush.farVal) / farVal;
    outColor = vec4(vec3(depth), 1.0f);
}
