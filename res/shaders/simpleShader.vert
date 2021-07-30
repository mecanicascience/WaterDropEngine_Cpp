#version 450

// Input values from the pipeline
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(push_constant) uniform Push {
    mat4 transform;
} inPush;

// Output values to the fragment shader
layout(location = 0) out vec3 outFragColor;


// Executed once for each vertex
void main() {
    gl_Position = inPush.transform * vec4(inPosition, 1.0);
    outFragColor = inColor;
}