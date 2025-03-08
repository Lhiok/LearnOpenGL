#include <common/global.h>
#include <common/window.h>
#include <common/mesh.h>
#include <common/shader.h>
#include <common/camera.h>

class normal_mapping : public Window
{
private:
    bool enableNormalMapping;
    Camera *_camera;
    PointLight *_pointLight;
    Mesh *_meshFloor, *_meshLight;
    Shader *_shader, *_shaderLight;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
    virtual void processInput(GLFWwindow *window);
public:
    normal_mapping(std::string name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~normal_mapping();
};

/**************************************************** normal_mapping ****************************************************/

normal_mapping::~normal_mapping()
{
    delete _camera;
    delete _pointLight;
    delete _meshFloor;
    delete _meshLight;
    delete _shader;
    delete _shaderLight;
    _camera = nullptr;
    _pointLight = nullptr;
    _meshFloor = nullptr;
    _meshLight = nullptr;
    _shader = nullptr;
    _shaderLight = nullptr;
}

void normal_mapping::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _camera->setPosition(0.0f, 0.0f, 20.0f);
    _pointLight = new PointLight(glm::vec3(20.0f, 0.0f, 5.0f), 1.0f, 0.09f, 0.032f, glm::vec3(0.05f), glm::vec3(0.3f), glm::vec3(1.0f));
    _meshLight = new Mesh(cube_vertices_vector, cube_indices_vector, std::vector<Texture*>{ });
    _shader = new Shader("5_advanced_lighting/5_normal_mapping.vs", "5_advanced_lighting/5_normal_mapping.fs");
    _shaderLight = new Shader("common/light.vs", "common/light.fs");

    // 计算切线信息
    for (int index = 0, len = floor_vertices_vector.size(); index < len; ++index)
    {
        floor_vertices_vector[index].Tangent = glm::vec3(0.0f);
        floor_vertices_vector[index].Bitangent = glm::vec3(0.0f);
    }
    for (int index = 0, len = floor_indices_vector.size(); index < len; index += 3)
    {
        int i0 = floor_indices_vector[index], i1 = floor_indices_vector[index + 1], i2 = floor_indices_vector[index + 2];
        glm::vec3 pos0 = floor_vertices_vector[i0].Position, pos1 = floor_vertices_vector[i1].Position, pos2 = floor_vertices_vector[i2].Position;
        glm::vec2 uv0 = floor_vertices_vector[i0].TexCoords, uv1 = floor_vertices_vector[i1].TexCoords, uv2 = floor_vertices_vector[i2].TexCoords;
        glm::vec2 deltaUV1 = uv1 - uv0, deltaUV2 = uv2 - uv0;
        glm::vec3 deltaPos1 = pos1 - pos0, deltaPos2 = pos2 - pos0;

        GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * f;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * f;
        floor_vertices_vector[i0].Tangent += tangent;
        floor_vertices_vector[i1].Tangent += tangent;
        floor_vertices_vector[i2].Tangent += tangent;
        floor_vertices_vector[i0].Bitangent += bitangent;
        floor_vertices_vector[i1].Bitangent += bitangent;
        floor_vertices_vector[i2].Bitangent += bitangent;
    }
    for (int index = 0, len = floor_vertices_vector.size(); index < len; ++index)
    {
        floor_vertices_vector[index].Tangent = glm::normalize(floor_vertices_vector[index].Tangent);
        floor_vertices_vector[index].Bitangent = glm::normalize(floor_vertices_vector[index].Bitangent);
    }
    
    _meshFloor = new Mesh(floor_vertices_vector, floor_indices_vector, std::vector<Texture*>{
        Texture::Load("texture_diffuse", "brickwall.jpg"),
        Texture::Load("texture_normal", "brickwall_normal.jpg") 
    });
    
    enableNormalMapping = true;
    std::cout << "enable normal mapping" << std::endl;
}

void normal_mapping::onUpdate(GLFWwindow *window)
{
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, _pointLight->position);
    lightModel = glm::scale(lightModel, glm::vec3(0.5f));

    glm::vec3 viewPos = _camera->position();
    glm::mat4 floorModel = glm::mat4(1.0f);
    floorModel = glm::translate(floorModel, glm::vec3(0.0f, 0.0f, -10.0f));
    floorModel = glm::rotate(floorModel, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    floorModel = glm::scale(floorModel, glm::vec3(20.0f));

    _shader->use();
    _shader->set1f("gamma", 2.2f);
    _shader->set3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
    _shader->set1i("mappingEnable", int(enableNormalMapping));
    _shader->setmat4fv("model", glm::value_ptr(floorModel));
    _shader->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shader->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _pointLight->draw(_shader);
    _meshFloor->draw(_shader);

    _shaderLight->use();
    _shaderLight->setmat4fv("model", glm::value_ptr(lightModel));
    _shaderLight->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderLight->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _shaderLight->set3f("color", _pointLight->specular.x, _pointLight->specular.y, _pointLight->specular.z);
    _meshLight->draw(_shaderLight);
}

void normal_mapping::processInput(GLFWwindow *window)
{
    if (Input::isKeyDown(window, GLFW_KEY_SPACE))
    {
        enableNormalMapping = !enableNormalMapping;
        std::cout << (enableNormalMapping ? "enable normal mapping" : "disable normal mapping") << std::endl;
    }

    Window::processInput(window);
}

/**************************************************** test ****************************************************/

int main()
{
    // 法线贴图
    Window *window1 = new normal_mapping("5_normal_mapping", 800, 600);
    window1->start();
    delete window1;

    return 0;
}