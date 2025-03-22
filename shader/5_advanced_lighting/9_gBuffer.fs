#version 330 core

layout (location = 0) out vec3 FragPosition;
layout (location = 1) out vec3 FragNormal;
layout (location = 2) out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D texture_diffuse0;

void main()
{
    FragPosition = fs_in.FragPos;
    // 立方体内部法线取反
    FragNormal = -fs_in.Normal;
    FragColor.rgb = texture(texture_diffuse0, fs_in.TexCoords).rgb;
    // FragColor.a = texture(texture_specular0, fs_in.TexCoords).r;
    FragColor.a = 1.0;
}