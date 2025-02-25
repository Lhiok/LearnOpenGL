#version 330 core

const int INSTANCE_NUM = 9;

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 offsets[INSTANCE_NUM];

void main()
{
    TexCoords = aTexCoords;
    vec3 offset = offsets[gl_InstanceID];
    gl_Position = projection * view * model * vec4(aPos + offset, 1.0);
}