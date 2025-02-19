#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

struct Light
{
    vec3 position; // 位置
    vec3 ambient; // 环境光
    vec3 diffuse; // 漫反射
    vec3 specular; // 镜面反射
};

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;

// 反光度
uniform float shininess;
// 光源
uniform Light light;
// 相机位置
uniform vec3 viewPos;

void main()
{
    // 环境光
    vec3 diffuseColor = texture(texture_diffuse0, TexCoord).xyz;
    vec3 ambient = light.ambient * diffuseColor;

    // 漫反射
    vec3 lightDirection = normalize(FragPos - light.position);
    // 传入顶点属性已经归一化不需要处理
    // Normal = normalize(Normal);
    // 单位法向量 * 单位光照方向向量(反向) = cos
    float diff = max(dot(Normal, -lightDirection), 0.0);
    vec3 diffuse = light.diffuse * (diff * diffuseColor);

    // 镜面反射
    // 计算反射向量 表面射出的方向向量
    // 参数1: 入射向量 光源到表面
    // 参数2: 表面法向量
    vec3 reflectDirection = reflect(lightDirection, Normal);
    vec3 viewDirection = normalize(FragPos - viewPos);
    // pow(,x) 高光反光度 反光度越高 反射光越强 散射越少 高亮点越小
    float spec = pow(max(-dot(viewDirection, reflectDirection), 0.0), shininess);
    vec3 specularColor = texture(texture_specular0, TexCoord).xyz;
    vec3 specular = light.specular * (spec * specularColor);

    // 最终颜色
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}