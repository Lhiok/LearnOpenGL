#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

out vec4 FragPos;

uniform mat4 lightSpaceMatrixs[6];

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        // 设置输出到texture_cube哪一面
        gl_Layer = face;
        for (int i = 0; i < 3; ++i)
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = lightSpaceMatrixs[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}