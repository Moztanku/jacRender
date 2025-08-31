#version 460 core

layout(binding = 1) uniform sampler2D texSampler;

// Push constants are accessible in fragment shader too
layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 color;
    float time;
    uint objectId;
    vec2 padding;
} pushConstants;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 textureColor = texture(texSampler, fragTexCoord).rgb;
    vec3 finalColor = fragColor * textureColor * pushConstants.color.rgb;
    
    // Apply a time-based effect using push constant time value
    float timeEffect = 0.5 + 0.5 * sin(pushConstants.time);
    finalColor *= timeEffect;
    
    outColor = vec4(finalColor, pushConstants.color.a);
}
