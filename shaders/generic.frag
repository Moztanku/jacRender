#version 460 core

// Set 0: Global UBOs
layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    vec3 position;
    uint debugConfig;
} camera;

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float decay;
    float maxDistance;
};

layout(set = 0, binding = 1) uniform LightUBO {
    PointLight pointLights[MAX_POINT_LIGHTS];
    uint pointLightCount;
} lights;

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

const float AMBIENT_LIGHT = 0.01;
const float MATERIAL_SHININESS = 0.0;

void main() {
    const bool DEBUG_1 = (camera.debugConfig & 0x1u) != 0u;

    const vec3 normal = normalize(fragNormal);
    const vec3 viewDir = normalize(camera.position - fragPosition);

    const vec3 diffuse_color = texture(diffuse_tex, fragTexCoord).rgb;
    const vec3 normal_color = texture(normal_tex, fragTexCoord).rgb;
    const vec3 specular_color = texture(specular_tex, fragTexCoord).rgb;
    const vec3 emissive_color = texture(emissive_tex, fragTexCoord).rgb;

    vec3 result = diffuse_color * AMBIENT_LIGHT;

    for (uint i = 0u; i < lights.pointLightCount; i++) {
        const PointLight light = lights.pointLights[i];

        if (light.intensity <= 0.0) continue;

        const float distance = length(light.position - fragPosition);

        const float attenuation = 1.0 / max(
            pow(distance, light.decay), 0.01
        ) * light.intensity;

        if (DEBUG_1) {
            // const float color = pow(2.0, -distance / 4.0);
            // const float color = distance - 40.0;
            const float color = 1.0 / max(
                pow(distance, light.decay), 0.01
            );

            result = vec3(color, color, color);

            continue;
        }

        if (attenuation <= 0.001) continue;

        // Diffuse
        const vec3 lightDir = normalize(light.position - fragPosition);
        const float diff = max(dot(normal, lightDir), 0.0);
    
        // Specular
        const vec3 reflectDir = reflect(-lightDir, normal);
        const float spec = pow(max(dot(viewDir, reflectDir), 0.0), MATERIAL_SHININESS);
    
        // Apply attenuation and light color
        const vec3 diffuse = diff * diffuse_color * light.color * attenuation;
        const vec3 specular = spec * specular_color * light.color * attenuation;
    
        result += diffuse + specular;
    }

    outColor = vec4(result, 1.0) * pc.color;
}
