#version 330 core

out vec2 TexCoords;

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

// std140布局 对齐偏移量必须是基准对齐倍数
// 标量 int/bool/float 基准量为N(N=4)
// 向量 基准量为2N或4N vec3为4N
// 标量或向量数组 每个元素
// 矩阵 存储为列向量的数组 每个向量基准对齐与vec4相同
// 结构体 等于所有元素根据规则计算后大小 但会填充到vec4倍数
layout (std140) uniform CameraBlock
{
                        // 值       // 基准对齐量       // 对齐偏移量
    mat4 view;          // 列0      // 16                // 0
                        // 列1      // 16                // 16
                        // 列2      // 16                // 32
                        // 列3      // 16                // 48
    mat4 projection;    // 列0      // 16                // 64
                        // 列1      // 16                // 80
                        // 列2      // 16                // 96
                        // 列3      // 16                // 112
    float value;        // value    // 4                 // 128
    vec3 vector;        // vector   // 16                // 144
    // 总大小 16 + 144 = 160
};

uniform mat4 model;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}