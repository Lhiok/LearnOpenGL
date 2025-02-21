#include <common/window.h>
#include <common/model.h>
#include <common/shader.h>
#include <common/light.h>

class stencil_testing : public Window
{
private:
    Camera *_camera;
    Model *_modelBackPack;
    Shader *_shaderBackPack, *_shaderOutline;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    stencil_testing(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~stencil_testing();
};

/**************************************************** stencil_testing ****************************************************/

stencil_testing::~stencil_testing()
{
    delete _camera;
    delete _modelBackPack;
    delete _shaderBackPack;
    delete _shaderOutline;
    _camera = nullptr;
    _modelBackPack = nullptr;
    _shaderBackPack = nullptr;
    _shaderOutline = nullptr;
}

void stencil_testing::onInit(GLFWwindow *window)
{
    // Camera
    Camera::SetMainCamera(_camera = new Camera(window));
    // Shader
    _shaderBackPack = new Shader("4_advanced/2_stencil_testing.vs", "4_advanced/2_stencil_testing.fs");
    _shaderOutline = new Shader("4_advanced/2_stencil_testing.vs", "common/single_color.fs");
    // Model
    _modelBackPack = new Model("backpack/backpack.obj");
}

void stencil_testing::onUpdate(GLFWwindow *window)
{
    // 启用模板缓冲
    glEnable(GL_STENCIL_TEST);
    // 清除模板缓冲
    glClear(GL_STENCIL_BUFFER_BIT);

    // 设置模板缓冲写入掩码 通常8位 只更新对应位为1的
    glStencilMask(0xFF);
    // 设置模板缓冲测试函数
    // 参数1: 测试函数
    // 参数2: 测试参考值
    // 参数3: 掩码 只有对应位为1的才参与测试
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    // 设置模板缓冲操作
    // 参数1: 模板缓冲失败操作
    // 参数2: 模板缓冲成功但深度测试失败操作
    // 参数3: 模板缓冲和深度测试均成功操作
    // GL_KEEP: 不更新模板缓冲
    // GL_ZERO: 更新模板缓冲为0
    // GL_REPLACE: 使用参考值更新模板缓冲 glStencilFunc设置的参考值
    // GL_INCR: 每次更新模板缓冲值加1
    // GL_DECR: 每次更新模板缓冲值减1
    // GL_INCR_WRAP: 每次更新模板缓冲值加1 但超出255时从0开始
    // GL_DECR_WRAP: 每次更新模板缓冲值减1 但小于0时从255开始
    // GL_INVERT: 每次更新模板缓冲值取反
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // 绘制模型
    _shaderBackPack->use();
    glm::mat4 modelCube = glm::mat4(1.0f);
    _shaderBackPack->setmat4fv("model", glm::value_ptr(modelCube));
    _shaderBackPack->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderBackPack->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _modelBackPack->draw(_shaderBackPack);

    // 禁用模板缓冲写入
    glStencilMask(0x00);
    // 设置测试模板缓冲函数
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    // 禁用深度测试
    glDisable(GL_DEPTH_TEST);

    // 绘制外轮廓
    _shaderOutline->use();
    modelCube = glm::scale(modelCube, glm::vec3(1.1f));
    _shaderOutline->setmat4fv("model", glm::value_ptr(modelCube));
    _shaderOutline->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderOutline->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _shaderOutline->set3f("color", 0.0f, 1.0f, 0.0f);
    _modelBackPack->draw(_shaderOutline);

    glStencilMask(0xFF);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new stencil_testing("2_stencil_testing", 800, 600);
    window1->start();
    delete window1;

    return 0;
}