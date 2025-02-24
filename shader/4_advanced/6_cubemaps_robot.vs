#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Pos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Pos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords.x = aTexCoords.x;
    // texture加载时进行了y轴翻转 nanosuit模型不需要 这里转回去
    TexCoords.y = 1.0 - aTexCoords.y;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}