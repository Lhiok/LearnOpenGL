#include <common/vertex.h>

#include <vector>

// 草丛位置
std::vector<glm::vec3> grass_position_vector = {
    glm::vec3(+1.5f, +0.0f, -0.4f),
    glm::vec3(+1.5f, +0.0f, +0.5f),
    glm::vec3(+0.0f, +0.0f, +0.7f),
    glm::vec3(-0.3f, +0.0f, -2.3f),
    glm::vec3(+0.5f, +0.0f, -0.6f),
};

// 玻璃位置
std::vector<glm::vec3> glass_position_vector = {
    glm::vec3(+0.0f, +0.0f, +0.51f),
    glm::vec3(+0.8f, +0.0f, +1.5f),
    glm::vec3(+1.5f, +0.0f, +0.0f),
    glm::vec3(-0.5f, +0.0f, -1.0f),
};