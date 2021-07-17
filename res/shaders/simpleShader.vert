#version 450

// Input values from the pipeline
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(push_constant) uniform Push {
    mat2 transform;
    vec2 offset;
    vec3 color;
} inPush;

// Output values to the fragment shader
layout(location = 0) out vec3 fragColor;


// Executed once for each vertex
void main() {
    // inPush.transform * inPosition + inPush.offset
    gl_Position = vec4(inPosition + inPush.offset, 0.0, 1.0); // vec2 (x,y), zPadding, divider
    fragColor = inColor;
}