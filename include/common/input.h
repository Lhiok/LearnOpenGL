#ifndef INPUT_H
#define INPUT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <map>

enum EKeyState
{
    // 无状态
    Normal = 0,
    // 按下
    Down,
    // 保持
    HoldOn,
    // 松开
    Up,
};

class Input
{
private:
    static GLFWwindow* _window;
    static std::map<int, EKeyState> _states;
public:
    static void enableInput(GLFWwindow* window);
    static void disableInput(GLFWwindow* window);
    static void update(GLFWwindow* window);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static EKeyState getKeyState(GLFWwindow* window, int key);
    static bool isKeyDown(GLFWwindow* window, int key);
    static bool isKeyHold(GLFWwindow* window, int key);
    static bool isKeyUp(GLFWwindow* window, int key);
};

GLFWwindow *Input::_window = nullptr;
std::map<int, EKeyState> Input::_states = std::map<int, EKeyState>();

void Input::enableInput(GLFWwindow* window)
{
    if (Input::_window == window)
    {
        return;
    }
    if (Input::_window != nullptr)
    {
        glfwSetKeyCallback(Input::_window, nullptr);
        Input::_states.clear();
        Input::_window = nullptr;
    }
    if (window != nullptr)
    {
        Input::_window = window;
        glfwSetKeyCallback(window, keyCallback);
    }
}

void Input::disableInput(GLFWwindow* window)
{
    if (Input::_window == window)
    {
        glfwSetKeyCallback(window, nullptr);
        Input::_states.clear();
        Input::_window = nullptr;
    }
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (Input::_window != window)
    {
        return;
    }
    if (action == GLFW_PRESS)
    {
        Input::_states[key] = EKeyState::Down;
    }
    else if (action == GLFW_RELEASE)
    {
        Input::_states[key] = EKeyState::Up;
    }
}

void Input::update(GLFWwindow* window)
{
    if (Input::_window != window)
    {
        return;
    }
    for (auto& it: Input::_states)
    {
        if (it.second == EKeyState::Up)
        {
            Input::_states[it.first] = EKeyState::Normal;
        }
        else if (it.second == EKeyState::Down)
        {
            Input::_states[it.first] = EKeyState::HoldOn;
        }
    }
}

EKeyState Input::getKeyState(GLFWwindow* window, int key)
{
    if (Input::_window != window || Input::_states.find(key) == Input::_states.end())
    {
        return EKeyState::Normal;
    }
    return Input::_states[key];
}

bool Input::isKeyDown(GLFWwindow* window, int key)
{
    return Input::getKeyState(window, key) == EKeyState::Down;
}

bool Input::isKeyHold(GLFWwindow* window, int key)
{
    EKeyState state = Input::getKeyState(window, key);
    return state == EKeyState::HoldOn || state == EKeyState::Down;
}

bool Input::isKeyUp(GLFWwindow* window, int key)
{
    return Input::getKeyState(window, key) == EKeyState::Up;
}

#endif