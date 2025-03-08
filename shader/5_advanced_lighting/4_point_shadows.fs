#version 330 core

out vec4 FragColor;

const vec3 sampleOffsetDirections[20] = vec3[]
(
    vec3(+1.0, +1.0, +1.0), vec3(+1.0, -1.0, +1.0), vec3(-1.0, -1.0, +1.0), vec3(-1.0, +1.0, +1.0),
    vec3(+1.0, +1.0, -1.0), vec3(+1.0, -1.0, -1.0), vec3(-1.0, -1.0, -1.0), vec3(-1.0, +1.0, -1.0),
    vec3(+1.0, +1.0, +0.0), vec3(+1.0, -1.0, +0.0), vec3(-1.0, -1.0, +0.0), vec3(-1.0, +1.0, +0.0),
    vec3(+1.0, +0.0, +1.0), vec3(+1.0, +0.0, -1.0), vec3(-1.0, +0.0, -1.0), vec3(-1.0, +0.0, +1.0),
    vec3(+0.0, +1.0, +1.0), vec3(+0.0, -1.0, +1.0), vec3(+0.0, -1.0, -1.0), vec3(+0.0, +1.0, -1.0)
);

in vec4 FragPos;
in vec3 Normal;
in vec2 TexCoords;

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

uniform float gamma;
uniform vec3 viewPos;
uniform float farPlane;
uniform PointLight light;
uniform sampler2D texture_diffuse0;
uniform samplerCube texture_cube_shadow;

void main()
{
    float distance = length(light.position - FragPos.xyz);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    // 环境光
    vec3 ambient = light.ambient;

    // 漫反射
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(light.position - FragPos.xyz);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * diff;

    // 镜面反射
    vec3 viewDirection = normalize(viewPos - FragPos.xyz);
    vec3 halfDirection = normalize(lightDirection + viewDirection);
    float spec = pow(max(dot(normal, halfDirection), 0.0), 32.0);
    vec3 specular = light.specular * spec;

    // 阴影
    float shadow = 0.0;
    float bias = 0.05;
    int samples = 20;
    float viewDistance = length(viewPos - FragPos.xyz);
    float diskRadius = (1.0 + viewDistance / farPlane) / 25.0;
    for (int i = 0; i < samples; ++i)
    {
        vec3 lightDir = FragPos.xyz + sampleOffsetDirections[i] * diskRadius - light.position;
        float currentDepth = length(lightDir) / farPlane;
        float closestDepth = texture(texture_cube_shadow, lightDir).r;
        if (currentDepth - bias > closestDepth)
        {
            shadow += 1.0;
        }
    }
    shadow /= samples;

    // 最终颜色
    vec3 diffuseColor = texture(texture_diffuse0, TexCoords).xyz;
    FragColor = vec4((ambient + (diffuse + specular) * (1.0 - shadow)) * diffuseColor * attenuation, 1.0);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));
}