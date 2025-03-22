#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

const float gamma = 2.2;

struct Light
{
    vec3 Position;
    vec3 Color;
    float Radius;
};

uniform sampler2D texture_position;
uniform sampler2D texture_normal;
uniform sampler2D texture_color;

uniform vec3 viewPos;
uniform Light lights[4];

uniform bool hdr;
uniform float exposure;

void main()
{
    vec3 position = texture(texture_position, TexCoords).xyz;
    vec3 normal = normalize(texture(texture_normal, TexCoords).xyz);
    vec3 color = texture(texture_color, TexCoords).rgb;
    // float specular = texture(texture_color, TexCoords).a;

    // 环境光
    vec3 ambient = 0.0 * color;
    // lighting
    vec3 lighting = vec3(0.0);
    for (int i = 0; i < 4; i++)
    {
        // 漫反射
        vec3 lightDir = lights[i].Position - position;
        float distance = length(lightDir);
        // 光体积优化
        // 会产生边界
        // OpenGL不擅长分支 并不一定是正向优化
        if (distance < lights[i].Radius)
        {
            float diff = max(dot(normalize(lightDir), normal), 0.0);
            vec3 diffuse = lights[i].Color * diff;
            diffuse *= 1.0 / (1.0 + 0.7 * distance + 1.8 * distance * distance);
            lighting += diffuse;
        }
    }
    vec3 finalColor = ambient + lighting * color;

    // Reinhard校正
    if (hdr) {
        finalColor = vec3(1.0) - exp(-finalColor * exposure);
    }
    // Gamma校正
    finalColor = pow(finalColor, vec3(1.0 / gamma));
    FragColor = vec4(finalColor, 1.0);
}