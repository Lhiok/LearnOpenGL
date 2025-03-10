#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT
{
    vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(normalMatrix * aNormal);
    // projection矩阵放到几何着色器处理
    gl_Position = view * model * vec4(aPos, 1.0);
}