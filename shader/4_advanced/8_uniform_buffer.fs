#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 color;
uniform sampler2D texture_diffuse0;

void main()
{
    FragColor = vec4(texture(texture_diffuse0, TexCoords).rgb * color, 1.0);
}