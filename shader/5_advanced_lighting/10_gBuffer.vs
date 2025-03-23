#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 viewPos = view * model * vec4(aPos, 1.0);
    gl_Position = projection * viewPos;
    // 存储视图空间坐标
    vs_out.FragPos = viewPos.xyz;
    vs_out.Normal = transpose(inverse(mat3(view * model))) * aNormal;
    vs_out.TexCoords = aTexCoord;
}