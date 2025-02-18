#ifndef GLOBAL_H
#define GLOBAL_H

#include "vertex.h"

#include <vector>

std::vector<Vertex> cube_vertices_vector = {
    // 正面
    {
        glm::vec3(-0.5f, -0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
    },
    {
        glm::vec3(-0.5f, +0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, +0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, -0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
    },
    // 背面
    {
        glm::vec3(+0.5f, -0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
    },
    {
        glm::vec3(+0.5f, +0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
    },
    {
        glm::vec3(-0.5f, +0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
    },
    {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
    },
    // 左边
    {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
    },
    {
        glm::vec3(-0.5f, +0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
    },
    {
        glm::vec3(-0.5f, +0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
    },
    {
        glm::vec3(-0.5f, -0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
    },
    // 右边
    {
        glm::vec3(+0.5f, -0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
    },
    {
        glm::vec3(+0.5f, +0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, +0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, -0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
    },
    // 上边
    {
        glm::vec3(-0.5f, +0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
    },
    {
        glm::vec3(-0.5f, +0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, +0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, +0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
    },
    // 下边
    {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
    },
    {
        glm::vec3(-0.5f, -0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, -0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, -0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
    },
};

std::vector<unsigned int> cube_indices_vector = {
    // 正面
    0, 1, 2,
    2, 3, 0,
    // 背面
    4, 5, 6,
    6, 7, 4,
    // 左边
    8, 9, 10,
    10, 11, 8,
    // 右边
    12, 13, 14,
    14, 15, 12,
    // 上边
    16, 17, 18,
    18, 19, 16,
    // 下边
    20, 21, 22,
    22, 23, 20,
};

#endif