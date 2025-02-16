#version 330 core

// 顶点着色器输出作为输入 名称/类型完全相同
in vec4 vertexColor;

out vec4 FragColor;

void main()
{
    FragColor = vertexColor;
}