#include <common/window.h>

class hello_triangle : public Window
{
private:
    GLuint _VAO, _VBO;
    GLuint _programID;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    hello_triangle(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) {}
    ~hello_triangle();
};

class hello_rectangle : public Window
{
private:
    GLuint _VAO, _VBO, _EBO;
    GLuint _programID;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    hello_rectangle(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) {}
    ~hello_rectangle();
};

/**************************************************** hello_triangle ****************************************************/

hello_triangle::~hello_triangle()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteProgram(_programID);
}

void hello_triangle::onInit(GLFWwindow *window)
{
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

    // 顶点着色器
    const GLchar *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // 检查编译错误
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Compile Vertex Shader Error" << std::endl;
        std::cout << infoLog << std::endl;
    }

    // 片段着色器
    const GLchar *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n\0";
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // 着色器程序
    _programID = glCreateProgram();
    glAttachShader(_programID, vertexShader);
    glAttachShader(_programID, fragmentShader);
    glLinkProgram(_programID);
    // 检查链接错误
    glGetProgramiv(_programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetProgramInfoLog(_programID, 512, NULL, infoLog);
        std::cout << "Link Shader Error" << std::endl;
        std::cout << infoLog << std::endl;
    }
    // 链接成功后删除着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void hello_triangle::onUpdate(GLFWwindow *window)
{
    glUseProgram(_programID);
    glBindVertexArray(_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

/**************************************************** hello_rectangle ****************************************************/

hello_rectangle::~hello_rectangle()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
    glDeleteProgram(_programID);
}

void hello_rectangle::onInit(GLFWwindow *window)
{
    // 顶点
    float vertices[] = {
        -0.5f, -0.5f, +0.0f ,// 左下角
        +0.5f, -0.5f, +0.0f, // 右下角
        +0.5f, +0.5f, +0.0f, // 右上角
        -0.5f, +0.5f, +0.0f, // 左上角
    };
    // 顶点索引
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    // 顶点数组
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    // 顶点缓冲
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 元素缓冲
    glGenBuffers(1, &_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 设置顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 顶点着色器
    const GLchar *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // 检查编译错误
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Compile Vertex Shader Error" << std::endl;
        std::cout << infoLog << std::endl;
    }

    // 片段着色器
    const GLchar *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n\0";
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // 着色器程序
    _programID = glCreateProgram();
    glAttachShader(_programID, vertexShader);
    glAttachShader(_programID, fragmentShader);
    glLinkProgram(_programID);
    // 检查链接错误
    glGetProgramiv(_programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetProgramInfoLog(_programID, 512, NULL, infoLog);
        std::cout << "Link Shader Error" << std::endl;
        std::cout << infoLog << std::endl;
    }
    // 链接成功后删除着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void hello_rectangle::onUpdate(GLFWwindow *window)
{
    // 使用线框模式绘制三角形正面与背面
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUseProgram(_programID);
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new hello_triangle("2_hello_triangle", 800, 600);
    window1->start();
    delete window1;

    Window *window2 = new hello_rectangle("2_hello_rectangle", 800, 600);
    window2->start();
    delete window2;
    
    return 0;
}