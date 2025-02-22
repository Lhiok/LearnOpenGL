#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture_screen;

void main()
{
    vec3 color = 1.0 - texture(texture_screen, TexCoord).xyz;
    FragColor = vec4(color, 1.0);
}