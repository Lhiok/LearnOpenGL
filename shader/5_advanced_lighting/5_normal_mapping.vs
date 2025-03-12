#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentLightPos; // 切线空间的光源位置
    vec3 TangentViewPos; // 切线空间的观察者位置
    vec3 TangentFragPos; // 切线空间的片元位置
} vs_out;

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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;
uniform PointLight light;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    // vec3 B = normalize(normalMatrix * aBitangent);
    vec3 N = normalize(normalMatrix * aNormal);
    // 共享顶点平均化切线向量使得TBN不再互相垂直 进行重正交化
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(T, N);
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = N;
    vs_out.TexCoords = aTexCoords;
    vs_out.TangentLightPos = TBN * light.position;
    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TangentFragPos = TBN * vs_out.FragPos;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}