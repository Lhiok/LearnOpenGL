#version 330 core

out vec4 FragColor;

in vec3 Pos;
in vec3 Normal;

uniform vec3 viewPos;
uniform samplerCube skybox;
uniform mat4 view;

void main()
{
    vec3 viewDirection = normalize(Pos - viewPos);
    vec3 reflectionDirection = reflect(-viewDirection, normalize(Normal));
    // TODO 方向存疑
    FragColor = vec4(texture(skybox, reflectionDirection).rgb, 1.0);
}