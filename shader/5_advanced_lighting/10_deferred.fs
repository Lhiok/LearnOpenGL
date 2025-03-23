#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

struct DirectionalLight
{
    vec3 direction; // 方向
    
    vec3 ambient; // 环境光
    vec3 diffuse; // 漫反射
    vec3 specular; // 镜面反射
};

uniform DirectionalLight light;

uniform sampler2D texture_position;
uniform sampler2D texture_normal;
uniform sampler2D texture_color;
uniform sampler2D texture_ssao;
uniform bool enableSSAO;

void main()
{
    vec3 position = texture(texture_position, TexCoords).xyz;
    vec3 normal = texture(texture_normal, TexCoords).xyz;
    vec3 color = texture(texture_color, TexCoords).rgb;
    float ambientOcclusion = texture(texture_ssao, TexCoords).r;

    // Blinn-Phong 观察空间下
    vec3 viewDir = normalize(-position);
    // 环境光
    vec3 ambient = enableSSAO? light.ambient * ambientOcclusion: light.ambient;
    // 漫反射
    vec3 lightDir = normalize(-light.direction);
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * light.diffuse;
    // 镜面反射
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), 32.0);
    vec3 specular = spec * light.specular;
    // 最终颜色
    FragColor = vec4((ambient + diffuse + specular) * color, 1.0);
}