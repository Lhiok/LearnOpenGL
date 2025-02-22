#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture_screen;

void main()
{
    vec3 color = texture(texture_screen, TexCoord).xyz;
    // float average = (color.r + color.g + color.b) / 3.0;
    // 人眼对绿色更敏感 对蓝色不太敏感 使用加权计算
    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    FragColor = vec4(average, average, average, 1.0);
}