#version 330 core

out vec4 FragColor;

struct DirectionalLight
{
    vec3 direction; // 方向
    
    vec3 ambient; // 环境光
    vec3 diffuse; // 漫反射
    vec3 specular; // 镜面反射
};

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_shadow;

uniform float gamma;
uniform vec3 viewPos;
uniform DirectionalLight light;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // 透视除法
    projCoords = projCoords * 0.5 + 0.5; // 将[-1,1]映射到[0,1]
    // 光源矩阵外无阴影
    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0 || projCoords.z < 0.0 || projCoords.z > 1.0)
    {
        return 0.0;
    }
    
    float shadow = 0.0;
    // 偏移 解决阴影失真
    float bias = 0.0001;
    float currentDepth = projCoords.z;
    // 多次采样减少锯齿
    vec2 texelSize = 1.0 / textureSize(texture_shadow, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(texture_shadow, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

void main()
{
    vec3 Normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 color = texture(texture_diffuse0, fs_in.TexCoords).rgb;
    // 环境光
    vec3 ambient = light.ambient;
    // 漫反射
    float diff = max(dot(-light.direction, Normal), 0.0);
    vec3 diffuse = light.diffuse * diff;
    // 镜面反射
    float spec = 0.0;
    vec3 halfVec = normalize(viewDir + (-light.direction));
    spec = pow(max(dot(Normal, halfVec), 0.0), 22);
    vec3 specular = light.specular * spec;
    // 阴影
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    // 最终颜色
    FragColor = vec4((ambient + (diffuse + specular) * (1.0 - shadow)) * color, 1.0);
    // gamma校正
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));
}