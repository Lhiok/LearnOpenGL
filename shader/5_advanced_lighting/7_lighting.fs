#version 330 core

out vec4 FragColor;

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
    // 立方体内部法线取反
    vec3 normal = normalize(-fs_in.Normal);
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
    FragColor = vec4(ambient + lighting * color, 1.0);}