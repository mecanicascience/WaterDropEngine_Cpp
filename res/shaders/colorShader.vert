#version 450

// Input values from the sub-render pass
layout(push_constant) uniform Push {
    mat4 transformWorldSpace;
    mat4 transformCameraSpace;
    mat4 transformProjSpace;
} inPush;

// Input values from the pipeline
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormals;
layout(location = 2) in vec3 inColor;

// Output values to the fragment shader
layout(location = 0) out vec3 outFragColor;


// Executed once for each vertex
void main() {
    gl_Position = inPush.transformProjSpace   // Vulkan frustum position
                * inPush.transformCameraSpace // Camera space position
                * inPush.transformWorldSpace  // World space position
                * vec4(inPosition, 1.0f);     // Object space position
    outFragColor = inColor;
}