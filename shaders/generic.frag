#version 460 core

// For now we just support single diffuse texture
//  I'll add PBR later
layout(binding = 1) uniform sampler2D texture_diffuse;

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 color;
    float time;
    uint objectId;
    vec2 padding;
} pushConstants;

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragTangent;
layout(location = 3) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    // Use constant color from push constants (no texture sampling)
    vec3 finalColor = vec3(1.0, 1.0, 1.0);

    // Simple vignetting effect - darkening around edges
    vec2 uv = fragTexCoord - 0.5; // Center coordinates around (0,0)
    float vignette = 1.0 - dot(uv, uv) * 1.5; // Distance from center
    vignette = smoothstep(0.3, 1.0, vignette); // Smooth falloff
    
    finalColor *= vignette;

    outColor = vec4(finalColor, pushConstants.color.a);
}