#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <map>
#include <string>
#include <iostream>

class Texture
{
private:
    std::string _type, _path;
    GLenum _format;
    unsigned int _texID;
    unsigned int _refCnt;
    int _width, _height, _nrChannels;
    Texture(std::string name, std::string path);
    ~Texture();
    void loadTexture();
    static std::string _TEXTURE_BASE_PATH;
    static std::map<std::string, Texture*> _TEXTURES_LOADED;
public:
    unsigned int getTexID() { return _texID; }
    std::string getType() { return _type; }
    static Texture *Load(std::string name, std::string path);
    static void Destroy(Texture *texture);
};

std::string Texture::_TEXTURE_BASE_PATH = "./../../../assets/";
std::map<std::string, Texture*> Texture::_TEXTURES_LOADED = std::map<std::string, Texture*>();

Texture::Texture(std::string type, std::string path)
{
    _type = type;
    _path = path;
    _refCnt = 0;
    loadTexture();
}

Texture::~Texture()
{
    glDeleteTextures(1, &_texID);
}

void Texture::loadTexture()
{
    // 生成纹理
    glGenTextures(1, &_texID);

    // 翻转纹理y轴 OpenGL0,0在左下 纹理0,0在左上
    stbi_set_flip_vertically_on_load(true);
    // 加载纹理
    unsigned char *data = stbi_load((_TEXTURE_BASE_PATH + _path).c_str(), &_width, &_height, &_nrChannels, 0);
    if (!data)
    {
        std::cout << "Failed to load texture: " << _path << std::endl;
        // 释放图像内存
        stbi_image_free(data);
        return;
    }

    // 根据图像通道设置格式
    switch (_nrChannels)
    {
    case 1:
        _format = GL_RED;
        break;
    
    case 3:
        _format = GL_RGB;
        break;
    
    case 4:
        _format = GL_RGBA;
        break;
    
    default:
        std::cout << "Unknown texture format: " << _path << std::endl;
        stbi_image_free(data);
        return;
    }

    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, _texID);
    // 设置数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, _format, GL_UNSIGNED_BYTE, data);
    // 生成多级渐远纹理 MipMap
    glGenerateMipmap(GL_TEXTURE_2D);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 释放图像内存
    stbi_image_free(data);
}

Texture *Texture::Load(std::string type, std::string path)
{
    Texture *texture = nullptr;
    auto it = _TEXTURES_LOADED.find(path);
    if (it != _TEXTURES_LOADED.end())
    {
        texture = it->second;
        ++texture->_refCnt;
        return texture;
    }

    texture = new Texture(type, path);
    ++texture->_refCnt;
    _TEXTURES_LOADED[path] = texture;
    return texture;
}

void Texture::Destroy(Texture *texture)
{
    if (texture == nullptr)
    {
        return;
    }

    if (--texture->_refCnt == 0)
    {
        _TEXTURES_LOADED.erase(texture->_path);
        delete texture;
    }
}

#endif