#ifndef GLOBAL_H
#define GLOBAL_H

#include "vertex.h"
#include "light.h"

#include <vector>

std::vector<Vertex> cube_vertices_vector = {
    // 正面
    {
        glm::vec3(-0.5f, -0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),
    },
    {
        glm::vec3(-0.5f, +0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, +0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, -0.5f, +0.5f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
    },
    // 背面
    {
        glm::vec3(+0.5f, -0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec2(0.0f, 0.0f),
    },
    {
        glm::vec3(+0.5f, +0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec2(0.0f, 1.0f),
    },
    {
        glm::vec3(-0.5f, +0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec2(1.0f, 1.0f),
    },
    {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec2(1.0f, 0.0f),
    },
    // 左边
    {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
    },
    {
        glm::vec3(-0.5f, +0.5f, -0.5f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
    },
    {
        glm::vec3(-0.5f, +0.5f, +0.5f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
    },
    {
        glm::vec3(-0.5f, -0.5f, +0.5f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
    },
    // 右边
    {
        glm::vec3(+0.5f, -0.5f, +0.5f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
    },
    {
        glm::vec3(+0.5f, +0.5f, +0.5f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, +0.5f, -0.5f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, -0.5f, -0.5f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
    },
    // 上边
    {
        glm::vec3(-0.5f, +0.5f, +0.5f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
    },
    {
        glm::vec3(-0.5f, +0.5f, -0.5f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, +0.5f, -0.5f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, +0.5f, +0.5f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
    },
    // 下边
    {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
    },
    {
        glm::vec3(-0.5f, -0.5f, +0.5f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, -0.5f, +0.5f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
    },
    {
        glm::vec3(+0.5f, -0.5f, -0.5f),
        glm::vec3(0.0f, -1.0f, 0.0f),
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

glm::vec3 cube_positions[] = {
    glm::vec3(+0.0f, +0.0f, +0.0f), 
    glm::vec3(+2.0f, +5.0f, -15.f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.f),  
    glm::vec3(+2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f, +3.0f, -7.5f),  
    glm::vec3(+1.3f, -2.0f, -2.5f),  
    glm::vec3(+1.5f, +2.0f, -2.5f), 
    glm::vec3(+1.5f, +0.2f, -1.5f), 
    glm::vec3(-1.3f, +1.0f, -1.5f),  
};

DirectionalLight *light_directional = new DirectionalLight(
    glm::vec3(-0.2f, -1.0f, -0.3f),
    glm::vec3(0.2f, 0.2f, 0.2f),
    glm::vec3(0.3f, 0.3f, 0.3f),
    glm::vec3(1.0f, 1.0f, 1.0f)
);

DirectionalLight *light_directional_weak = new DirectionalLight(
    glm::vec3(-0.2f, -1.0f, -0.3f),
    glm::vec3(0.05f, 0.05f, 0.05f),
    glm::vec3(0.2f, 0.2f, 0.2f),
    glm::vec3(1.0f, 1.0f, 1.0f)
);

PointLight *light_point = new PointLight(
    glm::vec3(1.2f, 1.0f, 2.0f),
    1.0f,
    0.09f,
    0.032f,
    glm::vec3(0.2f, 0.2f, 0.2f),
    glm::vec3(0.3f, 0.3f, 0.3f),
    glm::vec3(1.0f, 1.0f, 1.0f)
);

std::vector<PointLight*> light_point_vector = {
    new PointLight(
        glm::vec3(0.7f, 0.2f, 2.0f),
        1.0f,
        0.09f,
        0.032f,
        glm::vec3(0.05f, 0.05f, 0.05f),
        glm::vec3(0.2f, 0.2f, 0.2f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    ),
    new PointLight(
        glm::vec3(2.3f, -3.3f, -4.0f),
        1.0f,
        0.09f,
        0.032f,
        glm::vec3(0.05f, 0.0f, 0.0f),
        glm::vec3(0.8f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    ),
    new PointLight(
        glm::vec3(-4.0f, 2.0f, -12.0f),
        1.0f,
        0.09f,
        0.032f,
        glm::vec3(0.0f, 0.05f, 0.0f),
        glm::vec3(0.0f, 0.8f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    ),
    new PointLight(
        glm::vec3(0.0f, 0.0f, -3.0f),
        1.0f,
        0.09f,
        0.032f,
        glm::vec3(0.0f, 0.0f, 0.05f),
        glm::vec3(0.0f, 0.0f, 0.8f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    )
};

SpotLight *light_spot = new SpotLight(
    glm::vec3(1.2f, 1.0f, 2.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::cos(glm::radians(12.5f)),
    glm::cos(glm::radians(15.0f)),
    glm::vec3(0.05f, 0.05f, 0.05f),
    glm::vec3(0.8f, 0.8f, 0.8f),
    glm::vec3(1.0f, 1.0f, 1.0f)
);

#endif