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
layout (triangle_strip, max_vertices = 3) out;

const float MAGNITUDE = 2.0;

in VS_OUT
{
    vec2 texCoords;
} gs_in[];

out vec2 TexCoords;

uniform float time;
uniform mat4 projection;

vec3 GetNormal()
{
    vec3 a = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 b = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal)
{
    vec3 offset = MAGNITUDE * normal * ((sin(time) + 1.0) / 2.0);
    return position + vec4(offset, 0.0);
}

void main()
{
    vec3 normal = GetNormal();

    gl_Position = projection * explode(gl_in[0].gl_Position, normal);
    TexCoords = gs_in[0].texCoords;
    EmitVertex();

    gl_Position = projection * explode(gl_in[1].gl_Position, normal);
    TexCoords = gs_in[1].texCoords;
    EmitVertex();

    gl_Position = projection * explode(gl_in[2].gl_Position, normal);
    TexCoords = gs_in[2].texCoords;
    EmitVertex();

    EndPrimitive();
}