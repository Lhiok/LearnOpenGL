#include <common/global.h>
#include <common/window.h>
#include <common/mesh.h>
#include <common/shader.h>
#include <common/light.h>

class light_casters : public Window
{
private:
    Camera *_camera;
    float _shininess;
    Light *_light;
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
    light_casters(const GLchar *name, GLuint width, GLuint height, Light *light, std::string vertexPath, std::string fragmentPath) : Window(name, width, height) { _light = light; _vertexPath = vertexPath; _fragmentPath = fragmentPath; }
    ~light_casters();
};

/**************************************************** light_casters ****************************************************/

light_casters::~light_casters()
{
    delete _camera;
    delete _light;
    delete _meshCube;
    delete _meshLight;
    delete _shaderCube;
    delete _shaderLight;
    _camera = nullptr;
    _light = nullptr;
    _meshCube = nullptr;
    _meshLight = nullptr;
    _shaderCube = nullptr;
    _shaderLight = nullptr;
}

void light_casters::onInit(GLFWwindow *window)
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
    _modelCube = glm::mat4(1.0f);
    _modelCube = glm::translate(_modelCube, glm::vec3(0.0f, 0.0f, 0.0f));
    // 法线矩阵 模型矩阵3*3部分逆矩阵的转置矩阵
    _normalMatrix = glm::transpose(glm::inverse(glm::mat3(_modelCube)));
    _meshCube = new Mesh(cube_vertices_vector, cube_indices_vector, cube_texture_vector);
    _shaderCube = new Shader(_vertexPath, _fragmentPath);
    
    // Light
    _modelLight = glm::mat4(1.0f);
    _modelLight = glm::translate(_modelLight, _light->GetPosition());
    _modelLight = glm::scale(_modelLight, glm::vec3(0.2f, 0.2f, 0.2f));
    _meshLight = new Mesh(cube_vertices_vector, cube_indices_vector, cube_texture_vector);
    _shaderLight = new Shader("2_lighting/1_colors.vs", "2_lighting/1_colors_light.fs");
}

void light_casters::onUpdate(GLFWwindow *window)
{
    _shaderCube->use();

    // 设置物体材质
    _shaderCube->set1f("shininess", _shininess);

    // 设置光照信息
    _light->draw(_shaderCube);

    // 相机位置
    glm::vec3 viewPos = _camera->position();
    _shaderCube->set3f("viewPos", viewPos.x, viewPos.y, viewPos.z);

    // 视图矩阵
    _shaderCube->setmat4fv("view", glm::value_ptr(_camera->view()));
    // 投影矩阵
    _shaderCube->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    
    for (unsigned int index = 0, len = sizeof(cube_positions) / sizeof(cube_positions[0]); index < len; index++)
    {
        // 模型矩阵
        _modelCube = glm::mat4(1.0f);
        _modelCube = glm::translate(_modelCube, cube_positions[index]);
        _modelCube = glm::rotate(_modelCube, glm::radians(20.0f * index), glm::vec3(1.0f, 0.3f, 0.5f));
        _shaderCube->setmat4fv("model", glm::value_ptr(_modelCube));

        // 法线矩阵
        _normalMatrix = glm::transpose(glm::inverse(glm::mat3(_modelCube)));
        _shaderCube->setmat3fv("normalMatrix", glm::value_ptr(_normalMatrix));

        _meshCube->draw(_shaderCube);
    }

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
    Window *window1 = new light_casters("5_light_casters_directional", 800, 600, light_directional, "2_lighting/2_basic_lighting.vs", "2_lighting/5_light_casters_directional.fs");
    window1->start();
    delete window1;

    Window *window2 = new light_casters("5_light_casters_point", 800, 600, light_point, "2_lighting/2_basic_lighting.vs", "2_lighting/5_light_casters_point.fs");
    window2->start();
    delete window2;

    Window *window3 = new light_casters("5_light_casters_spot", 800, 600, light_spot, "2_lighting/2_basic_lighting.vs", "2_lighting/5_light_casters_spot.fs");
    window3->start();
    delete window3;

    return 0;
}