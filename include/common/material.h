#ifndef MATERIAL_H
#define MATERIAL_H

#include "shader.h"

#include <glm/glm.hpp>

class Material
{
public:
    float shininess; // 反光度
    glm::vec3 ambient; // 环境光
    glm::vec3 diffuse; // 漫反射
    glm::vec3 specular; // 镜面反射

    Material(float shininess, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    {
        this->shininess = shininess;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }

    ~Material()
    {

    }

    void draw(Shader *shader)
    {
        shader->set1f("material.shininess", shininess);
        shader->set3f("material.ambient", ambient.x, ambient.y, ambient.z);
        shader->set3f("material.diffuse", diffuse.x, diffuse.y, diffuse.z);
        shader->set3f("material.specular", specular.x, specular.y, specular.z);
    }
};

#endif