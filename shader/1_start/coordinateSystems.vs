#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

// 模型矩阵
uniform mat4 model;
// 视图矩阵
uniform mat4 view;
// 投影矩阵
uniform mat4 projection;

void main()
{
    // 从右往左计算
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}