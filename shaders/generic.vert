#version 460 core

// Set 0: Global UBOs
layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    vec3 position;
} camera;

layout(set = 0, binding = 1) uniform LightUBO {
    uint lightCount;
    // Up to 10 lights at once
    vec3 lightPositions[10];
    vec3 lightColors[10];
} lights;

// Set 1: Material UBOs
layout(set = 1, binding = 0) uniform MaterialUBO {
    float placeholder;
} material;

// Set 2: Instance UBOs, unused in this shader

// Push constants
layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 color;
    float time;
    uint objectId;
    vec2 padding;
} pc;

// Input attributes
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec2 inTexCoord;

// Output to fragment shader
layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragTangent;
layout(location = 3) out vec2 fragTexCoord;

void main() {
    mat4 mvp = camera.proj * camera.view * pc.model;

    fragPosition = vec3(pc.model * vec4(inPosition, 1.0));
    fragNormal = normalize(mat3(transpose(inverse(pc.model))) * inNormal);
    fragTangent = normalize(mat3(pc.model) * inTangent);
    fragTexCoord = inTexCoord;

    gl_Position = mvp * vec4(inPosition, 1.0);
}
