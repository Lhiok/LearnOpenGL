#version 330 core

const float ratio = 1.0 / 1.52;

out vec4 FragColor;

in vec3 Pos;
in vec3 Normal;

uniform vec3 viewPos;
uniform samplerCube skybox;
uniform mat4 view;

void main()
{
    vec3 viewDirection = normalize(Pos - viewPos);
    // 单面折射 不考虑出射
    vec3 refractDirection = refract(viewDirection, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, refractDirection).rgb, 1.0);
}