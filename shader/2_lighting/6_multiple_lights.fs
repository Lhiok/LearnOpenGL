#version 330 core

#define POINT_LIGHT_NUM 4

struct DirectionalLight
{
    vec3 direction; // 方向

    vec3 ambient; // 环境光
    vec3 diffuse; // 漫反射
    vec3 specular; // 镜面反射
};

struct PointLight
{
    vec3 position; // 位置

    float constant; // 常量
    float linear; // 一次项
    float quadratic; // 二次项

    vec3 ambient; // 环境光
    vec3 diffuse; // 漫反射
    vec3 specular; // 镜面反射
};

struct SpotLight
{
    vec3 position; // 位置
    vec3 direction; // 方向

    float cutOff; // 内切角
    float outerCutOff; // 外切角

    vec3 ambient; // 环境光
    vec3 diffuse; // 漫反射
    vec3 specular; // 镜面反射
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
// 反光度
uniform float shininess;
// 相机位置
uniform vec3 viewPos;
// 光源
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[POINT_LIGHT_NUM];
uniform SpotLight spotLight;

// 计算平行光
vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{
    // 环境光
    vec3 diffuseColor = texture(texture_diffuse0, TexCoord).xyz;
    vec3 ambient = light.ambient * diffuseColor;

    // 漫反射
    vec3 lightDirection = normalize(light.direction);
    // 单位法向量 * 单位光照方向向量(反向) = cos
    float diff = max(dot(normal, -lightDirection), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor;

    // 镜面反射
    vec3 specularColor = texture(texture_specular0, TexCoord).xyz;
    // 计算反射向量 表面射出的方向向量
    // 参数1: 入射向量 光源到表面
    // 参数2: 表面法向量
    vec3 reflectDirection = reflect(lightDirection, normal);
    // pow(,x) 高光反光度 反光度越高 反射光越强 散射越少 高亮点越小
    float spec = pow(max(-dot(viewDirection, reflectDirection), 0.0), shininess);
    vec3 specular = light.specular * spec * specularColor;

    // 最终颜色
    return ambient + diffuse + specular;
}

// 计算点光源
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)
{
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    // 环境光
    vec3 diffuseColor = texture(texture_diffuse0, TexCoord).xyz;
    vec3 ambient = light.ambient * diffuseColor;

    // 漫反射
    vec3 lightDirection = normalize(fragPos - light.position);
    float diff = max(dot(normal, -lightDirection), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor;

    // 镜面反射
    vec3 specularColor = texture(texture_specular0, TexCoord).xyz;
    vec3 reflectDirection = reflect(lightDirection, normal);
    float spec = pow(max(-dot(viewDirection, reflectDirection), 0.0), shininess);
    vec3 specular = light.specular * spec * specularColor;

    // 最终颜色
    return (ambient + diffuse + specular) * attenuation;
}

// 计算聚光灯
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)
{
    vec3 lightDirection = normalize(fragPos - light.position);
    float theta = dot(normalize(light.direction), lightDirection);
    float attenuation = clamp((theta - light.outerCutOff) / (light.cutOff - light.outerCutOff), 0.0, 1.0);

    // 环境光
    vec3 diffuseColor = texture(texture_diffuse0, TexCoord).xyz;
    vec3 ambient = light.ambient * diffuseColor;

    // 漫反射
    float diff = max(dot(normal, -lightDirection), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor;

    // 镜面反射
    vec3 specularColor = texture(texture_specular0, TexCoord).xyz;
    vec3 reflectDirection = reflect(lightDirection, normal);
    float spec = pow(max(-dot(viewDirection, reflectDirection), 0.0), shininess);
    vec3 specular = light.specular * spec * specularColor;

    // 最终颜色
    return (ambient + diffuse + specular) * attenuation;
}

void main()
{
    // 传入顶点属性已经归一化不需要处理
    // Normal = normalize(Normal);
    vec3 viewDirection = normalize(FragPos - viewPos);
    
    // 平行光
    vec3 result = calcDirectionalLight(directionalLight, Normal, viewDirection);
    // 点光源
    for (int i = 0; i < POINT_LIGHT_NUM; i++) {
        result += calcPointLight(pointLights[i], Normal, FragPos, viewDirection);
    }
    // 聚光灯
    result += calcSpotLight(spotLight, Normal, FragPos, viewDirection);

    // 最终颜色
    FragColor = vec4(result, 1.0);
}