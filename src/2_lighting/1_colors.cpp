#include <common/global.h>
#include <common/window.h>
#include <common/mesh.h>
#include <common/shader.h>

class colors : public Window
{
private:
    Camera *_camera;
    glm::mat4 _modelCube, _modelLight;
    Mesh *_meshCube, *_meshLight;
    Shader *_shaderCube, *_shaderLight;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    colors(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) {}
    ~colors();
};

/**************************************************** camera_move ****************************************************/

colors::~colors()
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

void colors::onInit(GLFWwindow *window)
{
    // Camera
    _camera = new Camera(window);
    
    Texture *cube_texture0 = new Texture("texture0", "container.jpg", GL_RGB);
    Texture *cube_texture1 = new Texture("texture1", "awesomeface.png", GL_RGBA);
    std::vector<Texture*> cube_texture_vector = {
        cube_texture0,
        cube_texture1,
    };

    // Cube
    _modelCube = glm::mat4(1.0f);
    _modelCube = glm::translate(_modelCube, glm::vec3(0.0f, 0.0f, 0.0f));
    _meshCube = new Mesh(cube_vertices_vector, cube_indices_vector, cube_texture_vector);
    _shaderCube = new Shader("2_lighting/1_colors.vs", "2_lighting/1_colors_cube.fs");
    
    // Light
    _modelLight = glm::mat4(1.0f);
    _modelLight = glm::translate(_modelLight, glm::vec3(1.2f, 1.0f, 2.0f));
    _modelLight = glm::scale(_modelLight, glm::vec3(0.2f, 0.2f, 0.2f));
    _meshLight = new Mesh(cube_vertices_vector, cube_indices_vector, cube_texture_vector);
    _shaderLight = new Shader("2_lighting/1_colors.vs", "2_lighting/1_colors_light.fs");
}

void colors::onUpdate(GLFWwindow *window)
{
    _shaderCube->use();
    _shaderCube->set3f("objectColor", 1.0f, 0.5f, 0.31f);
    _shaderCube->set3f("lightColor", 1.0f, 1.0f, 1.0f);
    _shaderCube->setmat4fv("model", glm::value_ptr(_modelCube));
    _shaderCube->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderCube->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _meshCube->draw(_shaderCube);

    _shaderLight->use();
    _shaderLight->setmat4fv("model", glm::value_ptr(_modelLight));
    _shaderLight->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderLight->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _meshLight->draw(_shaderLight);
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new colors("1_colors", 800, 600);
    window1->start();
    return 0;
}