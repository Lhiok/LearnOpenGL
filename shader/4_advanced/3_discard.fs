#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture_diffuse0;

void main()
{
    FragColor = texture(texture_diffuse0, TexCoord);
    if (FragColor.w < 0.1)
    {
        discard;
    }
}