#include <common/window.h>
#include <common/shader.h>
#include <common/texture.h>

class textures : public Window
{
private:
    GLuint _VAO, _VBO, _EBO;
    Shader *_shader;
    Texture *_texture0;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit();
    virtual void onUpdate();
public:
    textures(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath);
    ~textures();
};

class textures2 : public Window
{
private:
    GLuint _VAO, _VBO, _EBO;
    Shader *_shader;
    Texture *_texture0, *_texture1;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit();
    virtual void onUpdate();
public:
    textures2(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath);
    ~textures2();
};

/**************************************************** textures ****************************************************/

textures::textures(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height)
{
    _vertexPath = vertexPath;
    _fragmentPath = fragmentPath;
}

textures::~textures()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
    delete _shader;
    delete _texture0;
    _shader = nullptr;
    _vertexPath = nullptr;
    _fragmentPath = nullptr;
}

void textures::onInit()
{
    float vertices[] = {
        // 位置               // 颜色              // 纹理
        +0.5f, +0.5f, +0.0f, +1.0f, +0.0f, +0.0f, 1.0f, 1.0f, // 右上
        +0.5f, -0.5f, +0.0f, +0.0f, +1.0f, +0.0f, 1.0f, 0.0f, // 右下
        -0.5f, -0.5f, +0.0f, +0.0f, +0.0f, +1.0f, 0.0f, 0.0f, // 左下
        -0.5f, +0.5f, +0.0f, +1.0f, +1.0f, +0.0f, 0.0f, 1.0f  // 左上
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    
    // 顶点数组对象
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    // 顶点缓冲对象
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 索引缓冲对象
    glGenBuffers(1, &_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
    _texture0 = new Texture("ourTexture", "container.jpg", GL_RGB);
}

void textures::onUpdate()
{
    _shader->use();

    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, _texture0->getTexID());

    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/**************************************************** textures ****************************************************/

textures2::textures2(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height)
{
    _vertexPath = vertexPath;
    _fragmentPath = fragmentPath;
}

textures2::~textures2()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
    delete _shader;
    delete _texture0;
    delete _texture1;
    _shader = nullptr;
    _vertexPath = nullptr;
    _fragmentPath = nullptr;
}

void textures2::onInit()
{
    float vertices[] = {
        // 位置               // 颜色              // 纹理
        +0.5f, +0.5f, +0.0f, +1.0f, +0.0f, +0.0f, 1.0f, 1.0f, // 右上
        +0.5f, -0.5f, +0.0f, +0.0f, +1.0f, +0.0f, 1.0f, 0.0f, // 右下
        -0.5f, -0.5f, +0.0f, +0.0f, +0.0f, +1.0f, 0.0f, 0.0f, // 左下
        -0.5f, +0.5f, +0.0f, +1.0f, +1.0f, +0.0f, 0.0f, 1.0f  // 左上
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    
    // 顶点数组对象
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    // 顶点缓冲对象
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 索引缓冲对象
    glGenBuffers(1, &_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
    _texture0 = new Texture("texture0", "container.jpg", GL_RGB);
    _texture1 = new Texture("texture1", "awesomeface.png", GL_RGBA);
}

void textures2::onUpdate()
{
    _shader->use();

    // 绑定纹理单元
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture0->getTexID());
    // 设置uniform变量
    _shader->setli("texture0", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _texture1->getTexID());
    _shader->setli("texture1", 1);

    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new textures("4_textures_textureMixColor", 800, 600, "1_start/textureMixColor.vs", "1_start/textureMixColor.fs");
    window1->start();
    Window *window2 = new textures2("4_textures_textureMixColor", 800, 600, "1_start/textureMixColor.vs", "1_start/textureMixTexture.fs");
    window2->start();
    return 0;
}