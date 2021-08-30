#version 450

// The frag light intensity
layout(location = 0) in float inLightIntensity;

// Output to the swapchain value
layout(location = 0) out vec4 outColor;

void main() {
    vec3 waveColor = vec3(0.3, 0.3, 0.8);
    outColor = vec4(waveColor * inLightIntensity, 1.0);
}
