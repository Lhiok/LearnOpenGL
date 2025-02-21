#include <common/global.h>
#include <common/window.h>
#include <common/mesh.h>
#include <common/shader.h>
#include <common/light.h>

class multiple_lights : public Window
{
private:
    Camera *_camera;
    float _shininess;
    Mesh *_meshCube, *_meshLight;
    Shader *_shaderCube, *_shaderLight;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    multiple_lights(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height) { _vertexPath = vertexPath; _fragmentPath = fragmentPath; }
    ~multiple_lights();
};

/**************************************************** multiple_lights ****************************************************/

multiple_lights::~multiple_lights()
{
    delete _camera;
    delete _meshCube;
    delete _meshLight;
    delete _shaderCube;
    delete _shaderLight;
    _camera = nullptr;
    _meshCube = nullptr;
    _meshLight = nullptr;
    _shaderCube = nullptr;
    _shaderLight = nullptr;
}

void multiple_lights::onInit(GLFWwindow *window)
{
    // Camera
    _camera = new Camera(window);
    Camera::SetMainCamera(_camera);
    
    Texture *texture_diffuse = Texture::Load("texture_diffuse", "container2.png");
    Texture *texture_specular = Texture::Load("texture_specular", "container2_specular.png");
    std::vector<Texture*> cube_texture_vector = {
        texture_diffuse,
        texture_specular,
    };

    // Cube
    _shininess = 32.0f;
    _meshCube = new Mesh(cube_vertices_vector, cube_indices_vector, cube_texture_vector);
    _shaderCube = new Shader(_vertexPath, _fragmentPath);
    
    // Light
    _meshLight = new Mesh(cube_vertices_vector, cube_indices_vector, cube_texture_vector);
    _shaderLight = new Shader("2_lighting/1_colors.vs", "2_lighting/1_colors_light.fs");
}

void multiple_lights::onUpdate(GLFWwindow *window)
{
    _shaderCube->use();

    // 设置物体材质
    _shaderCube->set1f("shininess", _shininess);

    // 相机位置
    glm::vec3 viewPos = _camera->position();
    _shaderCube->set3f("viewPos", viewPos.x, viewPos.y, viewPos.z);

    // 视图矩阵
    _shaderCube->setmat4fv("view", glm::value_ptr(_camera->view()));
    // 投影矩阵
    _shaderCube->setmat4fv("projection", glm::value_ptr(_camera->projection()));

    // 改变聚光灯位置
    light_spot->position = _camera->position();
    light_spot->direction = _camera->front();

    // 设置光照信息
    light_directional_weak->draw(_shaderCube, "directionalLight");
    for (int index = 0, len = light_point_vector.size(); index < len; ++index)
    {
        light_point_vector[index]->draw(_shaderCube, "pointLights[" + std::to_string(index) + "]");
    }
    light_spot->draw(_shaderCube, "spotLight");
    
    // 物体绘制
    for (int index = 0, len = sizeof(cube_positions) / sizeof(cube_positions[0]); index < len; ++index)
    {
        // 模型矩阵
        glm::mat4 modelCube = glm::mat4(1.0f);
        modelCube = glm::translate(modelCube, cube_positions[index]);
        modelCube = glm::rotate(modelCube, glm::radians(20.0f * index), glm::vec3(1.0f, 0.3f, 0.5f));
        _shaderCube->setmat4fv("model", glm::value_ptr(modelCube));

        // 法线矩阵
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelCube)));
        _shaderCube->setmat3fv("normalMatrix", glm::value_ptr(normalMatrix));

        _meshCube->draw(_shaderCube);
    }

    // 光源绘制
    _shaderLight->use();
    _shaderLight->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderLight->setmat4fv("projection", glm::value_ptr(_camera->projection()));

    // 平行光绘制
    _shaderLight->set3f("color", light_directional_weak->specular.x, light_directional_weak->specular.y, light_directional_weak->specular.z);
    glm::mat4 modelLight = glm::mat4(1.0f);
    modelLight = glm::translate(modelLight, light_directional_weak->GetPosition());
    modelLight = glm::scale(modelLight, glm::vec3(0.2f));
    _shaderLight->setmat4fv("model", glm::value_ptr(modelLight));
    _meshLight->draw(_shaderLight);

    // 点光源绘制
    for (int index = 0, len = light_point_vector.size(); index < len; ++index)
    {
        _shaderLight->set3f("color", light_point_vector[index]->specular.x, light_point_vector[index]->specular.y, light_point_vector[index]->specular.z);
        modelLight = glm::mat4(1.0f);
        modelLight = glm::translate(modelLight, light_point_vector[index]->GetPosition());
        modelLight = glm::scale(modelLight, glm::vec3(0.2f));
        _shaderLight->setmat4fv("model", glm::value_ptr(modelLight));
        _meshLight->draw(_shaderLight);
    }

    // 聚光灯不绘制
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new multiple_lights("6_multiple_lights", 800, 600, "2_lighting/2_basic_lighting.vs", "2_lighting/6_multiple_lights.fs");
    window1->start();
    delete window1;

    return 0;
}