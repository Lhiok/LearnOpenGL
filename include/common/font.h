#ifndef FONT_H
#define FONT_H

#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <string>

struct Character
{
    GLuint TexID;
    glm::vec2 Size; // 字体大小
    glm::vec2 Bearing; // 从基准线到字体左部&顶部的距离
    GLuint Advance; // 原点距离下一个字符的水平距离
};

class Font
{
private:
    GLuint _VAO, _VBO;
    std::map<GLchar, Character> _characters;
    static std::string _FONT_BASE_PATH;
public:
    Font(std::string fontPath, GLuint width, GLuint height);
    ~Font();
    void DrawText(Shader *shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, GLfloat screenWidth, GLfloat screenHeight);
};

std::string Font::_FONT_BASE_PATH = "../../../assets/fonts/";

Font::Font(std::string fontPath, GLuint width, GLuint height)
{
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    // 4个顶点 * (位置坐标 + 纹理坐标)
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    FT_Library ft;
    // 初始化FreeType
    if (FT_Init_FreeType(&ft)) 
    {
        std::cout << "Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    // 加载字体
    if (FT_New_Face(ft, (_FONT_BASE_PATH + fontPath).c_str(), 0, &face))
    {
        std::cout << "Failed to load font: " + fontPath << std::endl;
        return;
    }
    
    // 设置字体大小
    // 宽度设置为0: 通过高度计算宽带
    // FT_Set_Pixel_Sizes(face, 0, 48);
    FT_Set_Pixel_Sizes(face, width, height);

    // 禁用字节对齐限制
    // OpenGL默认为4字节对齐 字体纹理格式为GL_RED 每个颜色仅由一个字节表示
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 获取每个字符的信息
    for (GLubyte c = 0; c < 128; c++)
    {
        // 加载字符
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "Failed to load Glyph: " << c << std::endl;
            continue;
        }
        // 生成纹理
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // 设置纹理格式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // 存储字符
        Character character = {
            texture,
            glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        _characters.insert(std::pair<GLchar, Character>(c, character));
    };

    // 释放资源
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

Font::~Font()
{
    for (std::map<GLchar, Character>::iterator it = _characters.begin(); it != _characters.end(); ++it)
    {
        glDeleteTextures(1, &it->second.TexID);
    }
    _characters.clear();
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
}

void Font::DrawText(Shader *shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, GLfloat screenWidth, GLfloat screenHeight)
{
    shader->use();
    // 关闭面剔除
    glDisable(GL_CULL_FACE);
    // 开启混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);
    shader->setmat4fv("projection", glm::value_ptr(projection));
    shader->set3f("textColor", color.x, color.y, color.z);
    shader->set1i("text", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    
    for (std::string::const_iterator c = text.begin(); c != text.end(); ++c)
    {
        Character ch = _characters[*c];
        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        // 绑定纹理
        glBindTexture(GL_TEXTURE_2D, ch.TexID);
        // 更新顶点数据
        GLfloat vertices[4][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
        };
        // 更新顶点缓冲区
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        // 渲染字符
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        // 更新x坐标 单位1/64像素
        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

#endif