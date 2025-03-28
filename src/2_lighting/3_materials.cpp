#include <common/global.h>
#include <common/window.h>
#include <common/mesh.h>
#include <common/shader.h>
#include <common/material.h>
#include <common/light.h>

class materials : public Window
{
private:
    Camera *_camera;
    PointLight *_light;
    Material *_material;
    glm::vec3 _posCube;
    glm::mat4 _modelCube, _modelLight;
    glm::mat3 _normalMatrix;
    Mesh *_meshCube, *_meshLight;
    Shader *_shaderCube, *_shaderLight;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    materials(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height) { _vertexPath = vertexPath; _fragmentPath = fragmentPath; }
    ~materials();
};

/**************************************************** materials ****************************************************/

materials::~materials()
{
    delete _camera;
    delete _light;
    delete _material;
    delete _meshCube;
    delete _meshLight;
    delete _shaderCube;
    delete _shaderLight;
    _camera = nullptr;
    _light = nullptr;
    _material = nullptr;
    _meshCube = nullptr;
    _meshLight = nullptr;
    _shaderCube = nullptr;
    _shaderLight = nullptr;
}

void materials::onInit(GLFWwindow *window)
{
    // Camera
    _camera = new Camera(window);
    Camera::SetMainCamera(_camera);
    
    Texture *cube_texture0 = Texture::Load("texture0", "container.jpg");
    Texture *cube_texture1 = Texture::Load("texture1", "awesomeface.png");
    std::vector<Texture*> cube_texture_vector = {
        cube_texture0,
        cube_texture1,
    };

    // Cube
    _modelCube = glm::mat4(1.0f);
    _modelCube = glm::translate(_modelCube, glm::vec3(0.0f, 0.0f, 0.0f));
    // 法线矩阵 模型矩阵3*3部分逆矩阵的转置矩阵
    _normalMatrix = glm::transpose(glm::inverse(glm::mat3(_modelCube)));
    _meshCube = new Mesh(cube_vertices_vector, cube_indices_vector, cube_texture_vector);
    _shaderCube = new Shader(_vertexPath, _fragmentPath);
    _material = new Material(32.0f, glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(0.5f, 0.5f, 0.5f));
    
    // Light
    _light = new PointLight(glm::vec3(1.2f, 1.0f, 2.0f), 1.0f, 0.09f, 0.032f, glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f));
    _modelLight = glm::mat4(1.0f);
    _modelLight = glm::translate(_modelLight, _light->position);
    _modelLight = glm::scale(_modelLight, glm::vec3(0.2f, 0.2f, 0.2f));
    _meshLight = new Mesh(cube_vertices_vector, cube_indices_vector, cube_texture_vector);
    _shaderLight = new Shader("2_lighting/1_colors.vs", "2_lighting/1_colors_light.fs");
}

void materials::onUpdate(GLFWwindow *window)
{
    _shaderCube->use();

    // 设置物体材质
    _material->draw(_shaderCube);

    // 光源信息随时间变化
    _light->specular.x = sin(glfwGetTime() * 2.0f);
    _light->specular.y = sin(glfwGetTime() * 0.7f);
    _light->specular.z = sin(glfwGetTime() * 1.3f);
    _light->ambient = _light->specular * 0.2f; // 降低影响
    _light->diffuse = _light->specular * 0.5f;
    _light->draw(_shaderCube);

    // 相机位置
    glm::vec3 viewPos = _camera->position();
    _shaderCube->set3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
    // 法线矩阵
    _shaderCube->setmat3fv("normalMatrix", glm::value_ptr(_normalMatrix));
    // 模型矩阵
    _shaderCube->setmat4fv("model", glm::value_ptr(_modelCube));
    // 视图矩阵
    _shaderCube->setmat4fv("view", glm::value_ptr(_camera->view()));
    // 投影矩阵
    _shaderCube->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _meshCube->draw(_shaderCube);

    _shaderLight->use();
    _shaderLight->set3f("color", _light->specular.x, _light->specular.y, _light->specular.z);
    _shaderLight->setmat4fv("model", glm::value_ptr(_modelLight));
    _shaderLight->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderLight->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _meshLight->draw(_shaderLight);
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new materials("3_materials", 800, 600, "2_lighting/2_basic_lighting.vs", "2_lighting/3_materials.fs");
    window1->start();
    delete window1;

    return 0;
}