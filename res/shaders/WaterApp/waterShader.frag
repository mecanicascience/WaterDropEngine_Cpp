#version 450

// Color terrain attachment
layout (input_attachment_index = 0, set = 2, binding = 0) uniform subpassInput inputTerrainDepth;
// Depth terrain attachment
layout (input_attachment_index = 1, set = 2, binding = 1) uniform subpassInput inputTerrainNormals;
// Depth terrain attachment
layout (input_attachment_index = 2, set = 2, binding = 2) uniform subpassInput inputTerrainColor;

// Set 0-0
layout(set = 0, binding = 0) uniform SceneBuffer {
    mat4 transformCameraSpace;  // Matrix from world space to camera space
    mat4 transformProjSpace;    // Matrix from camera space to projection frustum space
    float nearPlane;
    float farPlane;
} inSceneData;

// Input values from the pipeline
layout(push_constant) uniform Push {
    vec3 lightDirection;
    float alphaMultiplier;
} inPush;

// Vertex input
layout(location = 0) in vec3 inFragWorldSpacePosition;

// Fragment output color
layout(location = 0) out vec4 outColor;


void main() {
    // World space ray positions
    /*vec3 viewPosition = (inverse(inSceneData.transformCameraSpace) * vec4(0, 0, 0, 1)).xyz; // Camera world space position
    vec3 fragPosition = inFragWorldSpacePosition; // Interpolated from vertex position
    vec3 ray          = fragPosition - viewPosition; // Vector that starts at camera, and ends at vertex position

    // Distances
    float distanceToTerrain = subpassLoad(inputTerrainDepth).r * inSceneData.farPlane;
    float distanceToWater   = length(ray);
    float distanceThroughWater = distanceToTerrain - distanceToWater;

    // Output color
    vec4 terrainColor = vec4(subpassLoad(inputTerrainColor).rgb, 1.0);
    if (distanceThroughWater > 0) {
        //float alpha = 1 - exp(-distanceThroughWater * inPush.alphaMultiplier);
        //float alpha = 1.0;
        //vec4 oceanColor = vec4(0.0, 0.0, 1.0, 1.0);

        // outColor = mix(terrainColor, oceanColor, alpha);
        // return;
    }*/
    // outColor = terrainColor;

    // Input attachments maps
    float terrainDepth = subpassLoad(inputTerrainDepth).r;
    vec3 terrainNormal = subpassLoad(inputTerrainNormals).rgb;
    vec4 terrainColor  = subpassLoad(inputTerrainColor).rgba;


    // Compute output color
    vec4 oceanColor = vec4(0.3, 0.3, 0.85, 1.0);
    float alpha = terrainDepth;


    // Output color
    outColor = mix(terrainColor, oceanColor, alpha);
}