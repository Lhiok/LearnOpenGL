#version 330 core

// 输入 位置变量属性位置为0
layout (location = 0) in vec3 aPos; 

// 输出
out vec4 vertexColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    // 设置输出颜色为暗红色
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
}