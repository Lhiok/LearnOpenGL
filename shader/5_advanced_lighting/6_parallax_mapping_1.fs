#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentLightDir; // 切线空间的光源方向
    vec3 TangentViewPos; // 切线空间的观察者位置
    vec3 TangentFragPos; // 切线空间的片元位置
} fs_in;

struct DirectionalLight
{
    vec3 direction; // 方向
    
    vec3 ambient; // 环境光
    vec3 diffuse; // 漫反射
    vec3 specular; // 镜面反射
};

uniform float gamma;
uniform DirectionalLight light;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_normal;

// 仅使用法线贴图
void main()
{
    // 环境光
    vec3 ambient = light.ambient;

    // 漫反射
    vec3 normal = normalize(texture(texture_normal, fs_in.TexCoords).rgb * 2.0 - 1.0);
    float diff = max(dot(normal, -fs_in.TangentLightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;

    // 镜面反射
    vec3 viewDirection = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 halfDirection = normalize(-fs_in.TangentLightDir + viewDirection);
    float spec = pow(max(dot(normal, halfDirection), 0.0), 32.0);
    vec3 specular = light.specular * spec;

    // 最终颜色
    vec3 diffuseColor = texture(texture_diffuse0, fs_in.TexCoords).xyz;
    FragColor = vec4((ambient + diffuse + specular) * diffuseColor, 1.0);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));
}