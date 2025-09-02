#version 460 core

// Set 0: Global UBOs
layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    vec3 position;
} camera;

// Set 1: Material UBOs
layout(set = 1, binding = 0) uniform MaterialUBO {
    float placeholder;
} material;

layout(set = 1, binding = 1) uniform sampler2D diffuse_tex;
layout(set = 1, binding = 2) uniform sampler2D normal_tex;
layout(set = 1, binding = 3) uniform sampler2D specular_tex;
layout(set = 1, binding = 4) uniform sampler2D emissive_tex;

// Set 2: Instance UBOs

// Push constants
layout(push_constant) uniform PushConstant {
    mat4 model;
    vec4 color;
    float time;
    uint objectId;
    vec2 padding;
} pc;

// Input from vertex shader
layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragTangent;
layout(location = 3) in vec2 fragTexCoord;

// Output color
layout(location = 0) out vec4 outColor;

void main() {
    vec3 diffuseColor = texture(diffuse_tex, fragTexCoord).rgb;
    
    vec3 finalColor = diffuseColor;

    vec2 uv = fragTexCoord - 0.5;
    float vignette = 1.0 - dot(uv, uv) * 1.5;
    vignette = smoothstep(0.3, 1.0, vignette);

    finalColor *= vignette;

    outColor = vec4(finalColor, pc.color.a);
}