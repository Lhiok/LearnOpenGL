#version 330 core

out vec4 FragColor;

// 在C++代码中设置变量值
uniform vec4 ourColor;

void main()
{
    FragColor = ourColor;
}