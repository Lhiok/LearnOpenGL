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
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT
{
    vec3 color;
} gs_in[];

out vec3 fColor;

void build_house(vec4 position, vec3 color)
{
    fColor = color;
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);
    EmitVertex();
    gl_Position = position + vec4(+0.2, -0.2, 0.0, 0.0);
    EmitVertex();
    gl_Position = position + vec4(-0.2, +0.2, 0.0, 0.0);
    EmitVertex();
    gl_Position = position + vec4(+0.2, +0.2, 0.0, 0.0);
    EmitVertex();
    fColor = vec3(1.0, 1.0, 1.0);
    gl_Position = position + vec4(+0.0, +0.4, 0.0, 0.0);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    build_house(gl_in[0].gl_Position, gs_in[0].color);
}