#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

class Window
{
private:
    const GLchar *_name;
    GLuint _width;
    GLuint _height;
    GLFWwindow *_window;
    static bool _glfwInited;
    static bool _gladInited;
    static void onWindowSizeChange(GLFWwindow* window, int width, int height);
protected:
    virtual void onInit(GLFWwindow *window) {}
    virtual void onUpdate(GLFWwindow *window) {}
    virtual void processInput(GLFWwindow *window);
public:
    Window(const GLchar *name, GLuint width, GLuint height);
    ~Window();
    void start();
};

bool Window::_glfwInited = false;
bool Window::_gladInited = false;

Window::Window(const GLchar *name, GLuint width, GLuint height)
{
    _name = name;
    _width = width;
    _height = height;
    _window = nullptr;
}

Window::~Window()
{
    glfwDestroyWindow(_window);
    _window = nullptr;
}

void Window::start()
{
    if (!_glfwInited)
    {
        _glfwInited = true;
        // 初始化glfw
        glfwInit();
        // 设置主要和次要版本
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        // 使用核心模式 不需要向后兼容
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        std::cout << "GLFW initialized" << std::endl;
    }

    if (_window == nullptr)
    {
        // 创建窗口对象
        _window = glfwCreateWindow(_width, _height, _name, NULL, NULL);
        if (_window == nullptr)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        std::cout << "Window created: " << _name << std::endl;
    }

    // 设置上下文
    glfwMakeContextCurrent(_window);

    if (!_gladInited)
    {
        _gladInited = true;
        // 初始化glad
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }
        std::cout << "GLAD initialized" << std::endl;
    }

    // 设置窗口尺寸
    glViewport(0, 0, _width, _height);
    // 设置窗口变化回调
    glfwSetFramebufferSizeCallback(_window, onWindowSizeChange);
    std::cout << "Window context set: " << _name << std::endl;

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 初始化
    onInit(_window);
    std::cout << "Window init: " << _name << std::endl;

    // 渲染循环
    while (!glfwWindowShouldClose(_window))
    {
        // 设置清空屏幕所用的颜色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // 清空颜色缓冲、深度缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // 处理输入
        processInput(_window);
        // 渲染
        onUpdate(_window);
        // 检查并调用事件
        glfwPollEvents();
        // 交换缓冲
        glfwSwapBuffers(_window);
    }
}

void Window::processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void Window::onWindowSizeChange(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif