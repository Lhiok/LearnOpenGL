#version 330 core

layout (location = 0) out vec4 FragPosition;
layout (location = 1) out vec3 FragNormal;
layout (location = 2) out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D texture_diffuse0;

const float NEAR = 0.1;
const float FAR = 100.0f;

// 将屏幕空间非线性深度值转换为视图空间深度值
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));	
}

void main()
{
    // 视图空间坐标
    FragPosition.xyz = fs_in.FragPos;
    FragColor.a = LinearizeDepth(gl_FragCoord.z);
    FragNormal = normalize(fs_in.Normal);
    // FragColor.rgb = texture(texture_diffuse0, fs_in.TexCoords).rgb;
    FragColor.rgb = vec3(0.95);
    // FragColor.a = texture(texture_specular0, fs_in.TexCoords).r;
    FragColor.a = 1.0;
}