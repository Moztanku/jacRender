#version 460 core

layout(location = 0) in vec3 fragColor; // Input from vertex shader

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0); // Set the output color to red
}
