#version 330 core

out vec4 FragColor;

in vec3 Pos;
in vec3 Normal;
in vec2 TexCoords;

uniform mat4 view;
uniform vec3 viewPos;
uniform samplerCube skybox;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_reflect;

void main()
{
    vec3 viewDirection = normalize(Pos - viewPos);
    vec3 reflectDirection = reflect(viewDirection, normalize(Normal));
    vec3 reflectColor = texture(texture_reflect, TexCoords).rgb * texture(skybox, reflectDirection).rgb;
    vec3 objectColor = texture(texture_diffuse0, TexCoords).rgb;
    FragColor = vec4(reflectColor + objectColor , 1.0);
}