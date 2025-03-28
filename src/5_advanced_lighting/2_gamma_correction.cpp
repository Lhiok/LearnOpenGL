#include <common/global.h>
#include <common/window.h>
#include <common/light.h>
#include <common/mesh.h>
#include <common/material.h>
#include <common/shader.h>
#include <common/camera.h>

class gamma_correction : public Window
{
private:
    float gamma;
    bool correctEnable;
    Camera *_camera;
    Mesh *_meshFloor, *_meshLight;
    Material *_floorMaterial;
    PointLight *_pointLight;
    Shader *_shader, *_shaderPointLight;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
    virtual void processInput(GLFWwindow *window);
public:
    gamma_correction(std::string name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~gamma_correction();
};

/**************************************************** gamma_correction ****************************************************/

gamma_correction::~gamma_correction()
{
    delete _camera;
    delete _meshFloor;
    delete _meshLight;
    delete _floorMaterial;
    delete _pointLight;
    delete _shader;
    delete _shaderPointLight;
    _camera = nullptr;
    _meshFloor = nullptr;
    _meshLight = nullptr;
    _floorMaterial = nullptr;
    _pointLight = nullptr;
    _shader = nullptr;
    _shaderPointLight = nullptr;
}

void gamma_correction::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _camera->setPosition(0.0f, 5.0f, 0.0f);
    _meshFloor = new Mesh(floor_vertices_vector, floor_indices_vector, std::vector<Texture*>());
    _meshLight = new Mesh(cube_vertices_vector, cube_indices_vector, std::vector<Texture*>());
    _floorMaterial = new Material(32.0f, glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    _pointLight = new PointLight(glm::vec3(0.0f, 5.0f, -20.0f), 1.0f, 0.09f, 0.032f, glm::vec3(0.05f), glm::vec3(0.3f), glm::vec3(1.0f));
    _shader = new Shader("5_advanced_lighting/2_gamma_correction.vs", "5_advanced_lighting/2_gamma_correction.fs");
    _shaderPointLight = new Shader("common/light.vs", "common/light.fs");

    gamma = 2.2f;
    correctEnable = true;
    std::cout << (correctEnable ? "enable gamma correct" : "disable gamma correct") << std::endl;
}

void gamma_correction::onUpdate(GLFWwindow *window)
{
    _shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(50.0f));
    // MVP
    _shader->setmat4fv("model", glm::value_ptr(model));
    _shader->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shader->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    // 材质
    _floorMaterial->draw(_shader);
    // 观察位置
    glm::vec3 viewPos = _camera->position();
    _shader->set3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
    // 光源
    _pointLight->draw(_shader, "pointLight");
    // 是否开启Gamma校正
    _shader->set1f("gamma", gamma);
    _shader->set1i("correctEnable", int(correctEnable));
    // 绘制
    _meshFloor->draw(_shader);

    // 绘制光源
    _shaderPointLight->use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, _pointLight->position);
    model = glm::scale(model, glm::vec3(0.2f));
    _shaderPointLight->setmat4fv("model", glm::value_ptr(model));
    _shaderPointLight->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderPointLight->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    glm::vec3 lightColor = _pointLight->specular;
    _shaderPointLight->set3f("color", lightColor.x, lightColor.y, lightColor.z);
    _meshLight->draw(_shaderPointLight);
}

void gamma_correction::processInput(GLFWwindow *window)
{
    if (Input::isKeyDown(window, GLFW_KEY_SPACE))
    {
        correctEnable = !correctEnable;
        std::cout << (correctEnable ? "enable gamma correct" : "disable gamma correct") << std::endl;
    }

    Window::processInput(window);
}

/**************************************************** test ****************************************************/

int main()
{
    // Gamma校正
    Window *window1 = new gamma_correction("2_gamma_correction", 800, 600);
    window1->start();
    delete window1;

    return 0;
}