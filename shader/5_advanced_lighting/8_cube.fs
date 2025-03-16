#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Light
{
    vec3 Position;
    vec3 Color;
};

uniform Light lights[4];
uniform sampler2D texture_diffuse0;
uniform vec3 viewPos;

void main()
{
    vec3 color = texture(texture_diffuse0, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    // 环境光
    vec3 ambient = 0.0 * color;
    // lighting
    vec3 lighting = vec3(0.0);
    for (int i = 0; i < 4; i++)
    {
        // 漫反射
        vec3 lightDir = normalize(lights[i].Position - fs_in.FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = lights[i].Color * diff;
        float distance = length(lights[i].Position - fs_in.FragPos);
        diffuse *= 1.0 / (distance * distance);
        lighting += diffuse;
    }
    FragColor = vec4(ambient + lighting * color, 1.0);

    // 提取高亮区域
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
    {
        BrightColor = vec4(FragColor.rgb, 1.0);
    }
    else
    {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}