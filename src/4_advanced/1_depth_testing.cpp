#include <common/window.h>
#include <common/model.h>
#include <common/shader.h>
#include <common/light.h>

class depth_testing : public Window
{
private:
    Camera *_camera;
    Model *_modelBackPack;
    Shader *_shaderBackPack;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    depth_testing(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height) { _vertexPath = vertexPath; _fragmentPath = fragmentPath; }
    ~depth_testing();
};

/**************************************************** depth_testing ****************************************************/

depth_testing::~depth_testing()
{
    delete _camera;
    delete _modelBackPack;
    delete _shaderBackPack;
    _camera = nullptr;
    _modelBackPack = nullptr;
    _shaderBackPack = nullptr;
}

void depth_testing::onInit(GLFWwindow *window)
{
    // Camera
    Camera::SetMainCamera(_camera = new Camera(window));
    // Shader
    _shaderBackPack = new Shader(_vertexPath, _fragmentPath);
    // Model
    _modelBackPack = new Model("backpack/backpack.obj");

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 禁用深度更新
    // glDepthMask(GL_FALSE);

    // 设置深度测试函数
    // GL_LESS: 默认值 新深度值小于缓冲中的深度值,则更新缓冲
    // GL_LEQUAL: 小于等于则更新缓冲 配合glDepthMask(GL_FALSE)实现透明片段写入
    // GL_ALWAYS: 总是更新缓冲 2D界面
    glDepthFunc(GL_LESS);
}

void depth_testing::onUpdate(GLFWwindow *window)
{
    _shaderBackPack->use();
    // 模型矩阵
    _shaderBackPack->setmat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    // 视图矩阵
    _shaderBackPack->setmat4fv("view", glm::value_ptr(_camera->view()));
    // 投影矩阵
    _shaderBackPack->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    // 绘制模型
    _modelBackPack->draw(_shaderBackPack);
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new depth_testing("1_depth_testing", 800, 600, "2_lighting/1_colors.vs", "4_advanced/1_depth_testing.fs");
    window1->start();
    delete window1;

    return 0;
}