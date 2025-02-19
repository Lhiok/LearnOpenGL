#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

// 环境光强度
uniform float ambientStrength;
// 光照颜色
uniform vec3 lightColor;

void main()
{
    // 环境光
    vec3 ambient = ambientStrength * lightColor;

    // 物体颜色
    vec4 objectColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.2);

    // 最终颜色
    FragColor = vec4(ambient * objectColor.xyz, 1.0);
}