#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

// 环境光强度
uniform float ambientStrength;
// 光照颜色
uniform vec3 lightColor;
// 光源位置
uniform vec3 lightPos;
// 反射光强度
uniform float specularStrength;
// 相机位置
uniform vec3 viewPos;

void main()
{
    // 环境光
    vec3 ambient = ambientStrength * lightColor;

    // 漫反射
    vec3 lightDirection = normalize(FragPos - lightPos);
    // 传入顶点属性已经归一化不需要处理
    // Normal = normalize(Normal);
    // 单位法向量 * 单位光照方向向量(反向) = cos
    float diff = max(dot(Normal, -lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    // 镜面反射
    // 计算反射向量 表面射出的方向向量
    // 参数1: 入射向量 光源到表面
    // 参数2: 表面法向量
    vec3 reflectDirection = reflect(lightDirection, Normal);
    vec3 viewDirection = normalize(FragPos - viewPos);
    // pow(,x) 高光反光度 反光度越高 反射光越强 散射越少 高亮点越小
    float spec = pow(max(-dot(viewDirection, reflectDirection), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // 物体颜色
    vec4 objectColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.2);

    // 最终颜色
    FragColor = vec4((ambient + diffuse + specular) * objectColor.xyz, 1.0);
}