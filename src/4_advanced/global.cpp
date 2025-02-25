#include <common/vertex.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

// 点位置颜色
float point_positions_colors[] = {
    -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    +0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    +0.5f, +0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, +0.5f, 1.0f, 1.0f, 1.0f,
};

// 实例偏移
std::vector<glm::vec3> instance_offset_vector = {
    glm::vec3(+4.0f, +4.0f, +0.0f),
    glm::vec3(+4.0f, +0.0f, +0.0f),
    glm::vec3(+4.0f, -4.0f, +0.0f),
    glm::vec3(+0.0f, +4.0f, +0.0f),
    glm::vec3(+0.0f, +0.0f, +0.0f),
    glm::vec3(+0.0f, -4.0f, +0.0f),
    glm::vec3(-4.0f, +4.0f, +0.0f),
    glm::vec3(-4.0f, +0.0f, +0.0f),
    glm::vec3(-4.0f, -4.0f, +0.0f),
};

// 小行星带陨石model矩阵
std::vector<glm::mat4> rock_model_matrices;

// 初始化小行星带陨石model矩阵
void init_rock_model_matrices()
{
    if (rock_model_matrices.size() != 0)
    {
        return;
    }

    int amount = 100000;
    float offset = 10.0f, radius = 30.0f;
    for (int index = 0; index < amount; ++index)
    {
        glm::mat4 model = glm::mat4(1.0f);
    
        // 位移 平均分布圆弧上 偏移范围 [-offset, offset]
        float angle = index * 360.0f / amount;
        glm::vec3 offsetDirection = glm::vec3(rand(), rand(), rand());
        offsetDirection = glm::normalize(offsetDirection) * offset;
        model = glm::translate(model, glm::vec3(radius * sin(angle) + offsetDirection.x, offsetDirection.y, radius * cos(angle) + offsetDirection.z));
    
        // 缩放 [0.05, 0.25]
        float scale = (rand() % 20) / 100.0f + 0.05f;
        model = glm::scale(model, glm::vec3(scale));
    
        // 旋转
        float rotAngle = rand() % 3600 / 10.0f;
        glm::vec3 rotDirection = glm::vec3(rand(), rand(), rand());
        rotDirection = glm::normalize(rotDirection);
        model = glm::rotate(model, rotAngle, rotDirection);
    
        rock_model_matrices.push_back(model);
    }
}