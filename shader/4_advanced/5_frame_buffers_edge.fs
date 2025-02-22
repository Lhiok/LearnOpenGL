#version 330 core

const float offset = 1.0 / 300.0;

const vec2 offsets[9] = vec2[](
    vec2(-offset, offset),  // 左上
    vec2(0.0, offset),      // 上
    vec2(offset, offset),   // 右上
    vec2(-offset, 0.0),     // 左
    vec2(0.0, 0.0),         // 中
    vec2(offset, 0.0),      // 右
    vec2(-offset, -offset), // 左下
    vec2(0.0, -offset),     // 下
    vec2(offset, -offset)   // 右下
);

// 核 卷积矩阵
const float kernel[9] = float[](
    +1.0, +1.0, +1.0,
    +1.0, -8.0, +1.0,
    +1.0, +1.0, +1.0
);

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture_screen;

void main()
{
    vec3 color = vec3(0.0);
    for (int index = 0; index < 9; ++index)
    {
        color += texture(texture_screen, TexCoord + offsets[index]).xyz * kernel[index];
    }
    FragColor = vec4(color, 1.0);
}