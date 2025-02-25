#ifndef MESH_H
#define MESH_H

#include "vertex.h"
#include "texture.h"
#include "shader.h"

#include <glad/glad.h>

#include <string>
#include <vector>
#include <iostream>

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
    void draw(Shader *shader, GLsizei instanceCount = 0);
    int getTextureCount() { return _textures.size(); }
    // 测试用
    GLuint getVAO() { return _VAO; }
};

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures)
{
    _vertices = vertices;
    _indices = indices;
    _textures = textures;
    setupMesh();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);

    // TODO vector使用有问题
    for (int index = _textures.size() - 1; ~index; --index)
    {
        Texture::Destroy(_textures[index]);
    }

    _vertices.clear();
    _indices.clear();
    _textures.clear();
    _vertices.shrink_to_fit();
    _indices.shrink_to_fit();
    _textures.shrink_to_fit();
}

void Mesh::draw(Shader *shader, GLsizei instanceCount)
{
    int diffuseNr = 0;
    int specularNr = 0;
    for (int index = 0, len = _textures.size(); index < len; ++index)
    {
        Texture *tex = _textures[index];
        std::string number;
        std::string name = tex->getType();
        if (name == "texture_diffuse")
        {
            number = std::to_string(diffuseNr++);
        }
        else if (name == "texture_specular")
        {
            number = std::to_string(specularNr++);
        }
        // 激活纹理单元
        glActiveTexture(GL_TEXTURE0 + index);
        // 绑定纹理
        glBindTexture(GL_TEXTURE_2D, tex->getTexID());
        // 设置纹理单元
        shader->set1i((name + number).c_str(), index);
    }

    glBindVertexArray(_VAO);
    
    if (instanceCount > 0)
    {
        glDrawElementsInstanced(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0, instanceCount);
    }
    else
    {
        glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    }
    
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);
    // 法线
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);
    // 纹理坐标
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    // 解绑
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

#endif