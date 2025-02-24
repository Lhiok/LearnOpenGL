#ifndef CAMERA_H
#define CAMERA_H

// GLAD一定要在GLFW之前include
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

enum ECameraUpdateFlag
{
    None = 0,
    Front = 1 << 0,
    View = 1 << 1,
    Projection = 1 << 2,
};

class Camera
{
private:
    GLFWwindow *_window;
    // 更新标记
    unsigned int _flag;
    // 俯仰角 偏航角 视野
    float _pitch, _yaw, _fov;
    glm::vec3 _position, _front, _up;
    glm::mat4 _view, _projection;
    // 鼠标操作
    static bool _inited;
    static double _lastPosX, _lastPosY, _moveSpeed, _sensitivity;
    static void onMouseMove(GLFWwindow *window, double xPos, double yPos);
    static void onMouseScroll(GLFWwindow *window, double xOffset, double yOffset);
public:
    Camera(GLFWwindow *window);
    ~Camera();
    glm::vec3 position() { return _position; }
    glm::vec3 front();
    glm::mat4 view();
    glm::mat4 projection();
    static Camera *main;
    static void SetMainCamera(Camera *camera);
    static void onKeyInput(GLFWwindow *window, float deltaTime);
};

bool Camera::_inited = false;
double Camera::_lastPosX = 0.0f;
double Camera::_lastPosY = 0.0f;
double Camera::_moveSpeed = 5.0f;
double Camera::_sensitivity = 0.05f;
Camera *Camera::main = nullptr;

Camera::Camera(GLFWwindow *window)
{
    _window = window;
    _flag = ECameraUpdateFlag::Front | ECameraUpdateFlag::View | ECameraUpdateFlag::Projection;
    _pitch = 0.0f;
    _yaw = -90.0f;
    _fov = 45.0f;
    _position = glm::vec3(0.0f, 0.0f, 3.0f);
    _up = glm::vec3(0.0f, 1.0f, 0.0f);

    if (Camera::main == nullptr)
    {
        Camera::SetMainCamera(this);
    }
}

Camera::~Camera()
{
    if (Camera::main == this)
    {
        Camera::SetMainCamera(nullptr);
    }
    _window = nullptr;
}

glm::vec3 Camera::front()
{
    if (_flag & ECameraUpdateFlag::Front)
    {
        float radianPitch = glm::radians(_pitch);
        float radianYaw = glm::radians(_yaw);
        _front.x = cos(radianPitch) * cos(radianYaw);
        _front.y = sin(radianPitch);
        _front.z = cos(radianPitch) * sin(radianYaw);
        _flag ^= ECameraUpdateFlag::Front;
    }
    return _front;
}

glm::mat4 Camera::view()
{
    if (_flag & ECameraUpdateFlag::View)
    {
        _view = glm::lookAt(_position, _position + front(), _up);
        _flag ^= ECameraUpdateFlag::View;
    }
    return _view;
}

glm::mat4 Camera::projection()
{
    if (_flag & ECameraUpdateFlag::Projection)
    {
        int width, height;
        glfwGetWindowSize(_window, &width, &height);
        _projection = glm::perspective(glm::radians(_fov), width * 1.0f / height, 0.1f, 100.0f);
        _flag ^= ECameraUpdateFlag::Projection;
    }
    return _projection;
}

void Camera::onKeyInput(GLFWwindow *window, float deltaTime)
{
    if (Camera::main == nullptr || Camera::main->_window != window)
    {
        return;
    }
    float cameraSpeed = Camera::_moveSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        Camera::main->_flag |= ECameraUpdateFlag::View;
        Camera::main->_position += Camera::main->front() * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        Camera::main->_flag |= ECameraUpdateFlag::View;
        Camera::main->_position -= Camera::main->front() * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        Camera::main->_flag |= ECameraUpdateFlag::View;
        Camera::main->_position -= glm::normalize(glm::cross(Camera::main->front(), Camera::main->_up)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        Camera::main->_flag |= ECameraUpdateFlag::View;
        Camera::main->_position += glm::normalize(glm::cross(Camera::main->front(), Camera::main->_up)) * cameraSpeed;
    }
}

void Camera::SetMainCamera(Camera *camera)
{
    if (Camera::main == camera)
    {
        return;
    }
    if (Camera::main != nullptr)
    {
        glfwSetInputMode(Camera::main->_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(Camera::main->_window, nullptr);
        glfwSetScrollCallback(Camera::main->_window, nullptr);
        Camera::main = nullptr;
    }
    if (camera != nullptr)
    {
        Camera::main = camera;
        Camera::_inited = false;
        // 捕获鼠标并禁用
        glfwSetInputMode(camera->_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // 监听鼠标移动
        glfwSetCursorPosCallback(camera->_window, Camera::onMouseMove);
        // 监听鼠标滚轮
        glfwSetScrollCallback(camera->_window, Camera::onMouseScroll);
    }
}

void Camera::onMouseMove(GLFWwindow *window, double xPos, double yPos)
{
    if (Camera::main == nullptr)
    {
        return;
    }
    if (!Camera::_inited)
    {
        Camera::_inited = true;
        Camera::_lastPosX = xPos;
        Camera::_lastPosY = yPos;
        return;;
    }
    float xOffset = xPos - Camera::_lastPosX;
    float yOffset = yPos - Camera::_lastPosY;
    Camera::_lastPosX = xPos;
    Camera::_lastPosY = yPos;
    Camera::main->_yaw += xOffset * Camera::_sensitivity;
    Camera::main->_pitch -= yOffset * Camera::_sensitivity;
    // 限制俯仰角
    Camera::main->_pitch = std::clamp(Camera::main->_pitch, -89.0f, 89.0f);
    Camera::main->_flag |= ECameraUpdateFlag::Front | ECameraUpdateFlag::View;
}

void Camera::onMouseScroll(GLFWwindow *window, double xOffset, double yOffset)
{
    if (Camera::main == nullptr)
    {
        return;
    }
    Camera::main->_fov -= yOffset;
    if (Camera::main->_fov < 1.0f)
    {
        Camera::main->_fov = 1.0f;
    }
    else if (Camera::main->_fov > 45.0f)
    {
        Camera::main->_fov = 45.0f;
    }
    Camera::main->_flag |= ECameraUpdateFlag::Projection;
}

#endif