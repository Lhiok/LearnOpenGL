#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

const float gamma = 2.2;

uniform sampler2D texture_diffuse0;
uniform bool hdr;
uniform float exposure;

void main()
{
    vec3 hdrColor = texture(texture_diffuse0, TexCoords).rgb;
    // Reinhard校正
    if (hdr) {
        hdrColor = vec3(1.0) - exp(-hdrColor * exposure);
    }
    // Gamma校正
    hdrColor = pow(hdrColor, vec3(1.0 / gamma));
    FragColor = vec4(hdrColor, 1.0);
}