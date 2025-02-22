#include <common/window.h>
#include <common/global.h>
#include <common/model.h>
#include <common/shader.h>
#include <common/light.h>

#include "global.cpp"

class face_culling : public Window
{
private:
    Camera *_camera;
    Mesh *_meshCube, *_meshGrass;
    Shader *_shaderCube, *_shaderGrass;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    face_culling(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~face_culling();
};

/**************************************************** face_culling ****************************************************/

face_culling::~face_culling()
{
    delete _camera;
    delete _meshCube;
    delete _meshGrass;
    delete _shaderCube;
    delete _shaderGrass;
    _camera = nullptr;
    _meshCube = nullptr;
    _meshGrass = nullptr;
    _shaderCube = nullptr;
    _shaderGrass = nullptr;
}

void face_culling::onInit(GLFWwindow *window)
{
    // Camera
    Camera::SetMainCamera(_camera = new Camera(window));
    // Cube
    _meshCube = new Mesh(cube_vertices_vector, cube_indices_vector, std::vector<Texture*> { Texture::Load("texture_diffuse", "container2.png") });
    _shaderCube = new Shader("4_advanced/2_stencil_testing.vs", "4_advanced/2_stencil_testing.fs");
    // Grass
    _meshGrass = new Mesh(rectangle_vertices_vector, rectangle_indices_vector, std::vector<Texture*> { Texture::Load("texture_diffuse", "grass.png") });
    _shaderGrass = new Shader("4_advanced/2_stencil_testing.vs", "4_advanced/3_discard.fs");
}

void face_culling::onUpdate(GLFWwindow *window)
{
    // 开启面剔除 开启后所有反面的三角形都会在顶点着色器之后被剔除
    glEnable(GL_CULL_FACE);

    // 设置剔除类型
    // GL_BACK: 剔除背面 默认值
    // GL_FRONT: 剔除正面
    // GL_FRONT_AND_BACK: 剔除正面和背面
    glCullFace(GL_BACK);

    // 设置三角形正面顶点环绕方式
    // GL_CCW: 逆时针为正面 默认值
    // GL_CW: 顺时针为正面
    glFrontFace(GL_CCW);

    // 绘制立方体
    _shaderCube->use();
    _shaderCube->setmat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    _shaderCube->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderCube->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _meshCube->draw(_shaderCube);

    // 渲染非封闭物体时关闭面剔除
    glDisable(GL_CULL_FACE);

    // 渲染草丛
    _shaderGrass->use();
    _shaderGrass->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderGrass->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    for (int index = 0, len = grass_position_vector.size(); index < len; ++index)
    {
        glm::mat4 modelGrass = glm::mat4(1.0f);
        modelGrass = glm::translate(modelGrass, grass_position_vector[index]);
        _shaderGrass->setmat4fv("model", glm::value_ptr(modelGrass));
        _meshGrass->draw(_shaderGrass);
    }
}


/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new face_culling("4_face_culling", 800, 600);
    window1->start();
    delete window1;

    return 0;
}