#version 330 core

// 输入 位置变量属性位置为0
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

// 输出
out vec4 vertexColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    // 使用顶点属性
    vertexColor = aColor;
}