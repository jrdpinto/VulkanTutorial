#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

layout(binding = 0) uniform ProjectionMatrices
{
    mat4 perspective;
    mat4 view;
    mat4 model;
} projMat;

layout(location = 0) out vec3 fragCol;

void main() 
{
    gl_Position = projMat.perspective * projMat.view * projMat.model * vec4(pos, 1.0);
    fragCol = col;
}