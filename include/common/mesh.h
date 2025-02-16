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

class mesh
{
private:
    GLuint VAO, VBO, EBO;
    std::vector<Vertex> vertices; // 顶点信息
    std::vector<unsigned int> indices; // 顶点索引
    std::vector<Texture> textures; // 纹理
    void setupMesh();
public:
    mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    ~mesh();
    void draw(Shader shader);
};

mesh::mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    setupMesh();
}

mesh::~mesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glDeleteTextures(1, &textures[i].id);
    }
}

void mesh::draw(Shader shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        std::string number;
        std::string name = textures[i].type;
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
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        // 设置纹理单元
        glUniform1i(shader.getUniformLocation((name + number).c_str()), i);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void mesh::setupMesh()
{
    // 创建VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 顶点缓冲
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // 索引缓冲
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

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