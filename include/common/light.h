#ifndef LIGHT_H
#define LIGHT_H

#include "shader.h"

#include <glm/glm.hpp>

class Light
{
public:
    glm::vec3 ambient; // 环境光
    glm::vec3 diffuse; // 漫反射
    glm::vec3 specular; // 镜面反射

    Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }

    ~Light()
    {

    }

    virtual glm::vec3 GetPosition() = 0;

    virtual void draw(Shader *shader, std::string name = "light")
    {
        shader->set3f((name + ".ambient").c_str(), ambient.x, ambient.y, ambient.z);
        shader->set3f((name + ".diffuse").c_str(), diffuse.x, diffuse.y, diffuse.z);
        shader->set3f((name + ".specular").c_str(), specular.x, specular.y, specular.z);
    }
};

/********************************************************* 平行光 *********************************************************/

// 无衰减
class DirectionalLight : public Light
{
public:
    glm::vec3 direction; // 方向

    DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : Light(ambient, diffuse, specular)
    {
        this->direction = direction;
    }

    ~DirectionalLight()
    {

    }

    glm::vec3 GetPosition()
    {
        return glm::vec3(100.0f, 100.0f, 100.0f);
    }

    virtual void draw(Shader *shader, std::string name = "light")
    {
        Light::draw(shader, name);
        shader->set3f((name + ".direction").c_str(), direction.x, direction.y, direction.z);
    }
};

/********************************************************* 点光源 *********************************************************/

// 强度 = 1 / (constant + linear * distance + quadratic * distance^2)
class PointLight : public Light
{
public:
    glm::vec3 position; // 位置
    float constant; // 常量
    float linear; // 一次项
    float quadratic; // 二次项
    
    PointLight(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : Light(ambient, diffuse, specular)
    {
        this->position = position;
        this->constant = constant;
        this->linear = linear;
        this->quadratic = quadratic;
    }

    ~PointLight()
    {

    }

    glm::vec3 GetPosition()
    {
        return position;
    }

    virtual void draw(Shader *shader, std::string name = "light")
    {
        Light::draw(shader, name);
        shader->set3f((name + ".position").c_str(), position.x, position.y, position.z);
        shader->set1f((name + ".constant").c_str(), constant);
        shader->set1f((name + ".linear").c_str(), linear);
        shader->set1f((name + ".quadratic").c_str(), quadratic);
    }
};

/********************************************************* 聚光灯 *********************************************************/

// 强度 = (入射光&光源方向夹角余弦 - 外切光余弦) / (内切光余弦 - 外切光余弦)
class SpotLight : public Light  
{
public:
    glm::vec3 position; // 位置
    glm::vec3 direction; // 方向
    float cutOff; // 内切角余弦值
    float outerCutOff; // 外切角余弦值

    SpotLight(glm::vec3 position, glm::vec3 direction, float cutOff, float outerCutOff, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : Light(ambient, diffuse, specular)
    {
        this->position = position;
        this->direction = direction;
        this->cutOff = cutOff;
        this->outerCutOff = outerCutOff;
    }

    ~SpotLight()
    {

    }

    glm::vec3 GetPosition()
    {
        return position;
    }

    virtual void draw(Shader *shader, std::string name = "light")
    {
        Light::draw(shader, name);
        shader->set3f((name + ".position").c_str(), position.x, position.y, position.z);
        shader->set3f((name + ".direction").c_str(), direction.x, direction.y, direction.z);
        shader->set1f((name + ".cutOff").c_str(), cutOff);
        shader->set1f((name + ".outerCutOff").c_str(), outerCutOff);
    }
};

#endif