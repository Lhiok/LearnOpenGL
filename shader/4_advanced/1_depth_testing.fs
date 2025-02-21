#version 330 core

out vec4 FragColor;

void main()
{
    // gl_FragCoord.z: 片段深度值
    // 使用非线性映射到 [0.0, 1.0]
    // z: 视图空间z坐标
    // near: 近平面距离
    // far: 远平面距离
    // gl_FragCoord.z = (1 / z - 1 / near) / (1 / far - 1 / near);
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0f);
}