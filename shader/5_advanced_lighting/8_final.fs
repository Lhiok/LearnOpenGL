#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

const float gamma = 2.2;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_bloom;

uniform bool hdr;
uniform float exposure;
uniform bool bloom;

void main()
{
    vec3 color = texture(texture_diffuse0, TexCoords).rgb;
    if (bloom) {
        color += texture(texture_bloom, TexCoords).rgb;
    }
    if (hdr) {
        color = vec3(1.0) - exp(-color * exposure);
    }
    // Gamma校正
    color = pow(color, vec3(1.0 / gamma));
    FragColor = vec4(color, 1.0);
}