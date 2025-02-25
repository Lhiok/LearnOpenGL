#version 330 core

// GLSL提供的内建变量
// in gl_Vertex
// {
//     vec4  gl_Position;
//     float gl_PointSize;
//     float gl_ClipDistance[];
// } gl_in[];

// 输入布局修饰符
// points: 绘制 GL_POINTS 时
// lines: GL_LINES GL_LINE_STRIP
// lines_adjacency: GL_LINES_ADJACENCY GL_LINE_STRIP_ADJACENCY
// triangles: GL_TRIANGLES GL_TRIANGLE_STRIP GL_TRIANGLE_FAN
// triangles_adjacency: GL_TRIANGLES_ADJACENCY GL_TRIANGLE_STRIP_ADJACENCY
layout (triangles) in;
// 输出布局修饰符
// point
// line_strip
// triangle_strip
layout (line_strip, max_vertices = 2) out;

in VS_OUT
{
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.2;

uniform mat4 projection;

void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal * MAGNITUDE, 0.0));
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}