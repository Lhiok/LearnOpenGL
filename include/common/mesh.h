#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <iostream>

#include "texture.h"
#include "shader.h"

struct Vertex
{
    glm::vec3 position; // 顶点坐标
    glm::vec3 TexCoords; // 纹理坐标
};

class Mesh
{
private:
    GLuint _VAO, _VBO, _EBO;
    std::vector<Vertex> _vertices; // 顶点信息
    std::vector<unsigned int> _indices; // 顶点索引
    std::vector<Texture*> _textures; // 纹理
    void setupMesh();
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);
    ~Mesh();
    void draw(Shader shader);
};

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures)
{
    this->_vertices = vertices;
    this->_indices = indices;
    this->_textures = textures;
    setupMesh();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
    for (unsigned int i = 0; i < _textures.size(); i++)
    {
        delete _textures[i];
    }
    _vertices.clear();
    _indices.clear();
    _textures.clear();
    _vertices.shrink_to_fit();
    _indices.shrink_to_fit();
    _textures.shrink_to_fit();
}

void Mesh::draw(Shader shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < _textures.size(); i++)
    {
        Texture *tex = _textures[i];
        std::string number;
        std::string name = tex->getName();
        if (name == "texture_diffuse")
        {
            number = std::to_string(diffuseNr++);
        }
        else if (name == "texture_specular")
        {
            number = std::to_string(specularNr++);
        }
        // 激活纹理单元
        glActiveTexture(GL_TEXTURE0 + i);
        // 绑定纹理
        glBindTexture(GL_TEXTURE_2D, tex->getTexID());
        // 设置纹理单元
        glUniform1i(shader.getUniformLocation((name + number).c_str()), i);
    }

    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    // 创建VAO
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    // 顶点缓冲
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

    // 索引缓冲
    glGenBuffers(1, &_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);

    // 设置顶点属性
    // 顶点坐标
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // 纹理坐标
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(1);

    // 解绑
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

#endif