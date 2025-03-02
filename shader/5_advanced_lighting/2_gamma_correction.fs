#version 330 core

out vec4 FragColor;

struct Material
{
    float shininess;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} fs_in;

uniform vec3 viewPos;
uniform Material material;
uniform PointLight pointLight;

uniform float gamma;
uniform bool correctEnable;

void main()
{
    vec3 Normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 lightDir = normalize(pointLight.position - fs_in.FragPos);
    
    // 距离衰减
    float distance = length(pointLight.position - fs_in.FragPos);
    float attenuation = 1.0f / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * distance * distance);

    // 环境光
    vec3 ambient = pointLight.ambient * material.ambient;

    // 漫反射
    float diff = max(dot(lightDir, Normal), 0.0);
    vec3 diffuse = pointLight.diffuse * material.diffuse * diff;

    // 镜面反射
    float spec = 0.0;
    vec3 halfVec = normalize(viewDir + lightDir);
    spec = pow(max(dot(Normal, halfVec), 0.0), material.shininess);
    vec3 specular = pointLight.specular * material.specular * spec;

    FragColor = vec4((ambient + diffuse + specular) * attenuation, 1.0);
    
    // gamma校正
    if (correctEnable)
    {
        FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));        
    }
}