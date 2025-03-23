#version 330 core

out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;

uniform float radius;
uniform int kernelSize;

// 平铺噪声纹理到屏幕分辨率
const vec2 noiseScale = vec2(800.0 / 4.0, 600.0 / 4.0);

void main()
{
    // 视图空间坐标
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = texture(gNormal, TexCoords).xyz;
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
    // 使用噪声创建TBN矩阵
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; ++i)
    {
        vec3 sample = TBN * samples[i]; // 切线空间->视图空间
        sample = fragPos + sample * radius;

        vec4 samplePos = projection * vec4(sample, 1.0); // 视图空间->裁剪空间
        samplePos.xyz /= samplePos.w; // 透视划分
        samplePos.xyz = samplePos.xyz * 0.5 + 0.5; // 变换到[0, 1]

        float sampleDepth = -texture(gPosition, samplePos.xy).w;

        // 去除深度值在半径范围外的影响
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    FragColor = occlusion;
}