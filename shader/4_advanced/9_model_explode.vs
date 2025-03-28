#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT
{
    vec2 texCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;

void main()
{
    vs_out.texCoords = aTexCoords;
    // projection矩阵放到几何着色器处理
    gl_Position = view * model * vec4(aPos, 1.0);
}