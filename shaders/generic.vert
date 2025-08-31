#version 460 core

layout(binding = 0) uniform UBO {
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 color;
    float time;
    uint objectId;
    vec2 padding;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragTangent;
layout(location = 3) out vec2 fragTexCoord;

void main() {
    mat4 mvp = ubo.proj * ubo.view * pushConstants.model;
    gl_Position = mvp * vec4(inPosition, 1.0);

    fragPosition = vec3(pushConstants.model * vec4(inPosition, 1.0));
    fragNormal = normalize(mat3(transpose(inverse(pushConstants.model))) * inNormal);
    fragTangent = normalize(mat3(pushConstants.model) * inTangent);
    fragTexCoord = inTexCoord;
}