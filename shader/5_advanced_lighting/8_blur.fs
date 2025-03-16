#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

const float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

uniform sampler2D texture_diffuse0;

uniform bool horizontal;

void main()
{
    vec2 offset = 1.0 / textureSize(texture_diffuse0, 0);
    vec3 resultColor = texture(texture_diffuse0, TexCoords).rgb * weight[0];
    if (horizontal)
    {
        offset.y = 0;
    } 
    else
    {
        offset.x = 0;
    }
    for (int i = 1; i < 5; i++)
    {
        resultColor += texture(texture_diffuse0, TexCoords + offset * i).rgb * weight[i];
        resultColor += texture(texture_diffuse0, TexCoords - offset * i).rgb * weight[i];
    }
    FragColor = vec4(resultColor, 1.0);
}