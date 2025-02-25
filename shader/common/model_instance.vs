#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
// location 3 4 5 6 全部占用
layout (location = 3) in mat4 aModelMatrix;

out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * aModelMatrix * vec4(aPos, 1.0);
}