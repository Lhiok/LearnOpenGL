#include <common/global.h>
#include <common/window.h>
#include <common/model.h>
#include <common/shader.h>
#include <common/light.h>

class model_load : public Window
{
private:
    Camera *_camera;
    float _shininess;
    Model *_modelBackPack;
    Shader *_shaderBackPack;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    model_load(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height) { _vertexPath = vertexPath; _fragmentPath = fragmentPath; }
    ~model_load();
};

/**************************************************** model_load ****************************************************/

model_load::~model_load()
{
    delete _camera;
    delete _modelBackPack;
    delete _shaderBackPack;
    _camera = nullptr;
    _modelBackPack = nullptr;
    _shaderBackPack = nullptr;
}

void model_load::onInit(GLFWwindow *window)
{
    _shininess = 32.0f;
    // Camera
    Camera::SetMainCamera(_camera = new Camera(window));
    // Shader
    _shaderBackPack = new Shader(_vertexPath, _fragmentPath);
    // Model
    _modelBackPack = new Model("backpack/backpack.obj");
}

void model_load::onUpdate(GLFWwindow *window)
{
    _shaderBackPack->use();
    _shaderBackPack->set1f("shininess", _shininess);
    // 相机位置
    glm::vec3 viewPos = _camera->position();
    _shaderBackPack->set3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
    // 视图矩阵
    _shaderBackPack->setmat4fv("view", glm::value_ptr(_camera->view()));
    // 投影矩阵
    _shaderBackPack->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    // 改变聚光灯位置
    light_spot->position = _camera->position();
    light_spot->direction = _camera->front();
    // 设置光照信息
    light_directional_weak->draw(_shaderBackPack, "directionalLight");
    light_spot->draw(_shaderBackPack, "spotLight");
    // 模型矩阵
    glm::mat4 modelCube = glm::mat4(1.0f);
    _shaderBackPack->setmat4fv("model", glm::value_ptr(modelCube));
    // 法线矩阵
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelCube)));
    _shaderBackPack->setmat3fv("normalMatrix", glm::value_ptr(normalMatrix));
    // 绘制模型
    _modelBackPack->draw(_shaderBackPack);
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new model_load("1_model_load", 800, 600, "2_lighting/2_basic_lighting.vs", "3_model/1_model_load.fs");
    window1->start();
    delete window1;

    return 0;
}