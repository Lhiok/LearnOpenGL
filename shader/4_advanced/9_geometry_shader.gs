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
layout (points) in;
// 输出布局修饰符
// point
// line_strip
// triangle_strip
layout (points, max_vertices = 1) out;

void main()
{
    gl_PointSize = 2.0;
    gl_Position = gl_in[0].gl_Position;
    // 发射顶点
    EmitVertex();
    // 将发射的所有顶点合成指定的输出渲染单元
    EndPrimitive();
}