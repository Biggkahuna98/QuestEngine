#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

// Push constant MVP
layout(push_constant) uniform MVP_PushConstant
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
} MVP;

void main() {
    gl_Position = MVP.Projection * MVP.View * MVP.Model * vec4(inPosition, 1.0);
    fragColor = inColor;
}