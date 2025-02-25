#include <common/global.h>
#include <common/window.h>
#include <common/shader.h>
#include <common/model.h>

#include "global.cpp"

class geometry_shader : public Window
{
private:
    GLuint _VAO, _VBO;
    Shader *_shader;
    std::string _vertexPath, _geometryPath, _fragmentPath;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    geometry_shader(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string geometryPath, std::string fragmentPath) : Window(name, width, height), _vertexPath(vertexPath), _geometryPath(geometryPath), _fragmentPath(fragmentPath) { }
    ~geometry_shader();
};

class model_explode : public Window
{
private:
    Camera *_camera;
    Model *_model;
    Mesh *_meshCube;
    Shader *_shader;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    model_explode(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~model_explode();
};

class model_normal_display : public Window
{
private:
    Camera *_camera;
    Model *_model;
    Shader *_shader, *_shaderNormal;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    model_normal_display(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~model_normal_display();
};

/**************************************************** geometry_shader ****************************************************/

geometry_shader::~geometry_shader()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    delete _shader;
    _shader = nullptr;
}

void geometry_shader::onInit(GLFWwindow *window)
{
    _shader = new Shader(_vertexPath, _geometryPath, _fragmentPath);
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point_positions_colors), &point_positions_colors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void geometry_shader::onUpdate(GLFWwindow *window)
{
    _shader->use();
    glBindVertexArray(_VAO);
    glDrawArrays(GL_POINTS, 0, 4);
}

/**************************************************** model_explode ****************************************************/

model_explode::~model_explode()
{
    delete _camera;
    delete _model;
    delete _meshCube;
    delete _shader;
    _camera = nullptr;
    _model = nullptr;
    _meshCube = nullptr;
    _shader = nullptr;
}

void model_explode::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _model = new Model("backpack/backpack.obj");
    _meshCube = new Mesh(cube_vertices_vector, cube_indices_vector, std::vector<Texture*> { Texture::Load("texture_diffuse", "container2.png") });
    _shader = new Shader("4_advanced/9_model_explode.vs", "4_advanced/9_model_explode.gs", "4_advanced/9_model_explode.fs");
}

void model_explode::onUpdate(GLFWwindow *window)
{
    _shader->use();
    _shader->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shader->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _shader->set1f("time", glfwGetTime());
    // 绘制模型
    glm::mat4 model = glm::mat4(1.0f);
    _shader->setmat4fv("model", glm::value_ptr(model));
    _model->draw(_shader);
    // 绘制立方体
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
    _shader->setmat4fv("model", glm::value_ptr(model));
    _meshCube->draw(_shader);
}

/**************************************************** model_normal_display ****************************************************/

model_normal_display::~model_normal_display()
{
    delete _camera;
    delete _model;
    delete _shader;
    delete _shaderNormal;
    _camera = nullptr;
    _model = nullptr;
    _shader = nullptr;
    _shaderNormal = nullptr;
}

void model_normal_display::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _model = new Model("backpack/backpack.obj");
    _shader = new Shader("common/model.vs", "common/model.fs");
    _shaderNormal = new Shader("common/normal_display.vs", "common/normal_display.gs", "common/normal_display.fs");
}

void model_normal_display::onUpdate(GLFWwindow *window)
{
    // 绘制模型
    _shader->use();
    _shader->setmat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    _shader->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shader->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _model->draw(_shader);
    // 绘制立方体
    _shaderNormal->use();
    _shaderNormal->setmat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    _shaderNormal->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderNormal->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _model->draw(_shaderNormal);
}

/**************************************************** test ****************************************************/

int main()
{
    // 几何着色器绘制点
    Window *window1 = new geometry_shader("9_geometry_shader", 800, 600, "4_advanced/9_geometry_shader.vs", "4_advanced/9_geometry_shader.gs", "4_advanced/9_geometry_shader.fs");
    window1->start();
    delete window1;

    // 几何着色器绘制不同颜色图形 从顶点着色器获取输入并输出到片段着色器
    Window *window2 = new geometry_shader("9_geometry_shader_color", 800, 600, "4_advanced/9_geometry_shader_color.vs", "4_advanced/9_geometry_shader_color.gs", "4_advanced/9_geometry_shader_color.fs");
    window2->start();
    delete window2;

    // 模型爆炸
    Window *window3 = new model_explode("9_model_explode", 800, 600);
    window3->start();
    delete window3;

    // 法向量可视化
    Window *window4 = new model_normal_display("9_model_normal_display", 800, 600);
    window4->start();
    delete window4;

    return 0;
}