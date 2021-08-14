#version 450

// Color terrain attachment
layout (input_attachment_index = 0, set = 2, binding = 0) uniform subpassInput inputTerrain;
// Depth terrain attachment
layout (input_attachment_index = 1, set = 2, binding = 1) uniform subpassInput inputTerrainDepth;

// Vertex input color
layout(location = 0) in vec3 inFragColor;

// Fragment output color
layout(location = 0) out vec4 outColor;


void main() {
    // Output same terrain color
    vec3 terrainColor = subpassLoad(inputTerrain).rgb;
    //outColor = vec4(terrainColor, 1.0f);

    // Output depth
    vec3 terrainDepth = subpassLoad(inputTerrainDepth).rgb;
    //outColor = vec4(terrainDepth, 1.0f);

    // White mesh color
    outColor = vec4(inFragColor, 1.0f);
}