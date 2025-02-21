#include <common/window.h>
#include <common/shader.h>
#include <common/texture.h>

#include "global.cpp"

class texture_mix_color : public Window
{
private:
    GLuint _VAO, _VBO, _EBO;
    Shader *_shader;
    Texture *_texture0;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    texture_mix_color(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath);
    ~texture_mix_color();
};

class texture_mix_texture : public Window
{
private:
    GLuint _VAO, _VBO, _EBO;
    Shader *_shader;
    Texture *_texture0, *_texture1;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    texture_mix_texture(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath);
    ~texture_mix_texture();
};

/**************************************************** textures ****************************************************/

texture_mix_color::texture_mix_color(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height)
{
    _vertexPath = vertexPath;
    _fragmentPath = fragmentPath;
}

texture_mix_color::~texture_mix_color()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
    delete _shader;
    Texture::Destroy(_texture0);
    _shader = nullptr;
    _texture0 = nullptr;
}

void texture_mix_color::onInit(GLFWwindow *window)
{
    // 顶点数组对象
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    // 顶点缓冲对象
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);

    // 索引缓冲对象
    glGenBuffers(1, &_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW);

    // 链接顶点属性-位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 链接顶点属性-颜色
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 链接顶点属性-纹理
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 着色器程序
    _shader = new Shader(_vertexPath, _fragmentPath);
    _texture0 = Texture::Load("ourTexture", "container.jpg");
}

void texture_mix_color::onUpdate(GLFWwindow *window)
{
    _shader->use();

    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, _texture0->getTexID());

    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/**************************************************** textures ****************************************************/

texture_mix_texture::texture_mix_texture(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height)
{
    _vertexPath = vertexPath;
    _fragmentPath = fragmentPath;
}

texture_mix_texture::~texture_mix_texture()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
    delete _shader;
    Texture::Destroy(_texture0);
    Texture::Destroy(_texture1);
    _shader = nullptr;
    _texture0 = nullptr;
    _texture1 = nullptr;
}

void texture_mix_texture::onInit(GLFWwindow *window)
{
    // 顶点数组对象
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    // 顶点缓冲对象
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);

    // 索引缓冲对象
    glGenBuffers(1, &_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW);

    // 链接顶点属性-位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 链接顶点属性-颜色
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 链接顶点属性-纹理
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 着色器程序
    _shader = new Shader(_vertexPath, _fragmentPath);
    _texture0 = Texture::Load("texture0", "container.jpg");
    _texture1 = Texture::Load("texture1", "awesomeface.png");
}

void texture_mix_texture::onUpdate(GLFWwindow *window)
{
    _shader->use();

    // 绑定纹理单元
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture0->getTexID());
    // 设置uniform变量
    _shader->set1i("texture0", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _texture1->getTexID());
    _shader->set1i("texture1", 1);

    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new texture_mix_color("4_texture_mix_color", 800, 600, "1_start/textureMixColor.vs", "1_start/textureMixColor.fs");
    window1->start();
    delete window1;

    Window *window2 = new texture_mix_texture("4_texture_mix_texture", 800, 600, "1_start/textureMixColor.vs", "1_start/textureMixTexture.fs");
    window2->start();
    delete window2;
    
    return 0;
}