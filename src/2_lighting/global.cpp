#include <common/light.h>

DirectionalLight *light_directional = new DirectionalLight(
    glm::vec3(-0.2f, -1.0f, -0.3f),
    glm::vec3(0.2f, 0.2f, 0.2f),
    glm::vec3(0.5f, 0.5f, 0.5f),
    glm::vec3(1.0f, 1.0f, 1.0f)
);

PointLight *light_point = new PointLight(
    glm::vec3(1.2f, 1.0f, 2.0f),
    1.0f,
    0.09f,
    0.032f,
    glm::vec3(0.2f, 0.2f, 0.2f),
    glm::vec3(0.5f, 0.5f, 0.5f),
    glm::vec3(1.0f, 1.0f, 1.0f)
);

SpotLight *light_spot = new SpotLight(
    glm::vec3(1.2f, 1.0f, 2.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::cos(glm::radians(12.5f)),
    glm::cos(glm::radians(15.0f)),
    glm::vec3(0.2f, 0.2f, 0.2f),
    glm::vec3(0.5f, 0.5f, 0.5f),
    glm::vec3(1.0f, 1.0f, 1.0f)
);