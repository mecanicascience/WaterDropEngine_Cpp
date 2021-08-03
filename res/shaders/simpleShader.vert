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
    vec4 worldSpacePos = inPush.transformWorldSpace * vec4(inPosition, 1.0f); // Position in world space
    gl_Position = inPush.transformProjSpace * inPush.transformCameraSpace * worldSpacePos;
    outFragColor = inColor;

    float scalAmount = (dot(inNormals, vec3(1.0f, 0.0f, 0.0f)) + 1.0f) / 2.0f;
    // outFragColor = vec3(scalAmount, scalAmount, scalAmount);

    // Calculate the position on the near and far plane
    vec2 xyPlanePosCorrected = gl_Position.xy / gl_Position.w;
    vec4 nearPlanePos = vec4(xyPlanePosCorrected, 0.0f, 1.0f); // Point coords on the near place (-1, -1, 0) -> (1, 1, 0)
    vec4 farPlanePos  = vec4(xyPlanePosCorrected, 1.0f, 1.0f); // Point coords on the far  plane (-1, -1, 1) -> (1, 1, 1)
    // outFragColor = vec3((nearPlanePos.x + 1.0f) / 2.0f, (nearPlanePos.y + 1.0f) / 2.0f, 0.0f);

    // Calculate the position in world space on the near and far plane
    vec4 worldSpaceNearPlanePos = nearPlanePos * inverse(inPush.transformProjSpace) * inverse(inPush.transformCameraSpace);
    vec4 worldSpaceFarPlanePos  = farPlanePos  * inverse(inPush.transformProjSpace) * inverse(inPush.transformCameraSpace);

    // Represent distance to the object
    float dstToObject = length(nearPlanePos.xyz - worldSpacePos.xyz);
    float s = 5.0f;
    // outFragColor = vec3(dstToObject / s, dstToObject / s, dstToObject / s);
}