#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 Position; // 顶点坐标
    glm::vec3 Normal; // 法向量
    glm::vec2 TexCoords; // 纹理坐标
};

#endif