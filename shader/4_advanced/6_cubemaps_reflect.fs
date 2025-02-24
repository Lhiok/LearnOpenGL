#version 330 core

out vec4 FragColor;

in vec3 Pos;
in vec3 Normal;

uniform mat4 view;
uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{
    vec3 viewDirection = normalize(Pos - viewPos);
    vec3 reflectDirection = reflect(viewDirection, normalize(Normal));
    FragColor = vec4(texture(skybox, reflectDirection).rgb, 1.0);
}