#include <common/global.h>
#include <common/window.h>
#include <common/mesh.h>
#include <common/shader.h>

class basic_lighting : public Window
{
private:
    Camera *_camera;
    float _ambientStrength, _specularStrength;
    glm::vec3 _posCube, _posLight, _colorLight;
    glm::mat4 _modelCube, _modelLight;
    glm::mat3 _normalMatrix;
    Mesh *_meshCube, *_meshLight;
    Shader *_shaderCube, *_shaderLight;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    basic_lighting(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height) { _vertexPath = vertexPath; _fragmentPath = fragmentPath; }
    ~basic_lighting();
};

/**************************************************** basic_lighting ****************************************************/

basic_lighting::~basic_lighting()
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

void basic_lighting::onInit(GLFWwindow *window)
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
    
    // Light
    _ambientStrength = 0.1f;
    _specularStrength = 0.5f;
    _posLight = glm::vec3(1.2f, 1.0f, 2.0f);
    _colorLight = glm::vec3(1.0f, 1.0f, 1.0f);
    _modelLight = glm::mat4(1.0f);
    _modelLight = glm::translate(_modelLight, glm::vec3(1.2f, 1.0f, 2.0f));
    _modelLight = glm::scale(_modelLight, glm::vec3(0.2f, 0.2f, 0.2f));
    _meshLight = new Mesh(cube_vertices_vector, cube_indices_vector, cube_texture_vector);
    _shaderLight = new Shader("2_lighting/1_colors.vs", "2_lighting/1_colors_light.fs");
}

void basic_lighting::onUpdate(GLFWwindow *window)
{
    _shaderCube->use();

    // 环境光强度
    _shaderCube->set1f("ambientStrength", _ambientStrength);
    // 反射光强度
    _shaderCube->set1f("specularStrength", _specularStrength);
    // 光照颜色
    _shaderCube->set3f("lightColor", _colorLight.x, _colorLight.y, _colorLight.z);
    // 光源位置
    _shaderCube->set3f("lightPos", _posLight.x, _posLight.y, _posLight.z);
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
    _shaderLight->set3f("color", _colorLight.x, _colorLight.y, _colorLight.z);
    _shaderLight->setmat4fv("model", glm::value_ptr(_modelLight));
    _shaderLight->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderLight->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _meshLight->draw(_shaderLight);
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new basic_lighting("2_basic_lighting_ambient", 800, 600, "2_lighting/2_basic_lighting.vs", "2_lighting/2_basic_lighting_ambient.fs");
    window1->start();
    delete window1;

    Window *window2 = new basic_lighting("2_basic_lighting_diffuse", 800, 600, "2_lighting/2_basic_lighting.vs", "2_lighting/2_basic_lighting_diffuse.fs");
    window2->start();
    delete window2;

    Window *window3 = new basic_lighting("2_basic_lighting_specular", 800, 600, "2_lighting/2_basic_lighting.vs", "2_lighting/2_basic_lighting_specular.fs");
    window3->start();
    delete window3;

    return 0;
}