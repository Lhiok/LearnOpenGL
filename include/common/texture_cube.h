#ifndef TEXTURE_CUBE_H
#define TEXTURE_CUBE_H

#include <glad/glad.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#endif

#include <vector>
#include <string>
#include <iostream>

class TextureCube
{
private:
    unsigned int _texID;
    void loadTexture(GLenum target, std::string path);
    static std::string _TEXTURE_CUBE_BASE_PATH;
public:
    TextureCube(std::vector<std::string> paths);
    ~TextureCube();
    unsigned int getTexID() { return _texID; }
};

std::string TextureCube::_TEXTURE_CUBE_BASE_PATH = "./../../../assets/";

TextureCube::TextureCube(std::vector<std::string> paths)
{
    // 生成纹理
    glGenTextures(1, &_texID);
    // 绑定纹理
    glBindTexture(GL_TEXTURE_CUBE_MAP, _texID);
    // cubemap 左上角为(0,0) 不用翻转y轴
    stbi_set_flip_vertically_on_load(false);
    // 加载6个面 右 左 上 下 前 后
    for (int index = 0, len = paths.size(); index < len; ++index)
    {
        loadTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, paths[index]);
    }
    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // 解绑
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

TextureCube::~TextureCube()
{
    glDeleteTextures(1, &_texID);
}

void TextureCube::loadTexture(GLenum target, std::string path)
{
    int width, height, nrChannels, format;
    // 加载纹理
    unsigned char *data = stbi_load((_TEXTURE_CUBE_BASE_PATH + path).c_str(), &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cout << "Failed to load texture: " << path << std::endl;
        // 释放图像内存
        stbi_image_free(data);
        return;
    }

    // 根据图像通道设置格式
    switch (nrChannels)
    {
    case 1:
        format = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        std::cout << "Unknown texture format: " << path << std::endl;
        stbi_image_free(data);
        return;
    }

    // 设置数据
    glTexImage2D(target, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    // 释放图像内存
    stbi_image_free(data);
}

#endif