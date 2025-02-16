#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <string>
#include <iostream>

class Texture
{
private:
    unsigned int _texID;
    int _width, _height, _nrChannels;
    GLenum _format;
    std::string _name, _path;
    void loadTexture();
    static std::string textureBasePath;
public:
    Texture(std::string name, std::string path, GLenum format);
    ~Texture();
    unsigned int getTexID() { return _texID; }
    std::string getName() { return _name; }
};

std::string Texture::textureBasePath = "./../../../assets/";

Texture::Texture(std::string name, std::string path, GLenum format)
{
    _name = name;
    _path = path;
    _format = format;
    loadTexture();
}

Texture::~Texture()
{
    glDeleteTextures(1, &_texID);
    _texID = 0;
    _width = 0;
    _height = 0;
    _nrChannels = 0;
    _format = 0;
    _name = nullptr;
    _path = nullptr;
}

void Texture::loadTexture()
{
    // 生成纹理
    glGenTextures(1, &_texID);
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, _texID);
    // 翻转纹理y轴 OpenGL0,0在左下 纹理0,0在左上
    stbi_set_flip_vertically_on_load(true);
    // 加载纹理
    unsigned char *data = stbi_load((textureBasePath + _path).c_str(), &_width, &_height, &_nrChannels, 0);
    if (data)
    {
        // 设置数据
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, _format, GL_UNSIGNED_BYTE, data);
        // 生成多级渐远纹理 MipMap
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << _path << std::endl;
    }
    // 释放图像内存
    stbi_image_free(data);
}

#endif