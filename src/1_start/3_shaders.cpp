#include <common/window.h>
#include <common/shader.h>

class shaders : public Window
{
private:
    GLuint _VAO, _VBO;
    Shader *_shader;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    shaders(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath);
    ~shaders();
};

class shaders2 : public Window
{
private:
    GLuint _VAO, _VBO;
    Shader *_shader;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    shaders2(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath);
    ~shaders2();
};

/**************************************************** shaders ****************************************************/

shaders::shaders(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height)
{
    _vertexPath = vertexPath;
    _fragmentPath = fragmentPath;
}

shaders::~shaders()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    delete _shader;
    _shader = nullptr;
    _vertexPath = nullptr;
    _fragmentPath = nullptr;
}

void shaders::onInit(GLFWwindow *window)
{
    // 获取硬件支持最大的顶点属性上限 至少16个
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum number of vertex attributes supported: " << nrAttributes << std::endl;

    float vertices[] = {
        -0.5f, -0.5f, +0.0f,
        +0.5f, -0.5f, +0.0f,
        +0.0f, +0.5f, +0.0f
    };
    
    // 顶点数组对象
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    // 顶点缓冲对象
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 链接顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 着色器程序
    _shader = new Shader(_vertexPath, _fragmentPath);
}

void shaders::onUpdate(GLFWwindow *window)
{
    _shader->use();
    // 根据时间设置ourColor
    float timeValue = glfwGetTime();
    float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
    _shader->set4f("ourColor", 0.0f, greenValue, 0.0f, 1.0f);

    glBindVertexArray(_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

/**************************************************** shaders2 ****************************************************/

shaders2::shaders2(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height)
{
    this->_vertexPath = vertexPath;
    this->_fragmentPath = fragmentPath;
}

shaders2::~shaders2()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    delete _shader;
    _shader = nullptr;
    _vertexPath = nullptr;
    _fragmentPath = nullptr;
}

void shaders2::onInit(GLFWwindow *window)
{
    // 获取硬件支持最大的顶点属性上限 至少16个
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum number of vertex attributes supported: " << nrAttributes << std::endl;

    float vertices[] = {
        // 位置               // 颜色
        -0.5f, -0.5f, +0.0f, +1.0f, +0.0f, +0.0f,
        +0.5f, -0.5f, +0.0f, +0.0f, +1.0f, +0.0f,
        +0.0f, +0.5f, +0.0f, +0.0f, +0.0f, +1.0f
    };
    
    // 顶点数组对象
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    // 顶点缓冲对象
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 链接顶点属性-位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 链接顶点属性-颜色
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 着色器程序
    _shader = new Shader(_vertexPath, _fragmentPath);
}

void shaders2::onUpdate(GLFWwindow *window)
{
    _shader->use();
    glBindVertexArray(_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new shaders("3_shaders_vertexColor", 800, 600, "1_start/vertexColor.vs", "1_start/vertexColor.fs");
    window1->start();
    Window *window2 = new shaders("3_shaders_uniformColor", 800, 600, "common/base.vs", "1_start/uniformColor.fs");
    window2->start();
    Window *window3 = new shaders2("3_shaders_attributeColor", 800, 600, "1_start/attributeColor.vs", "1_start/vertexColor.fs");
    window3->start();
    return 0;
}