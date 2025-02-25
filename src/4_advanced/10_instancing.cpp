#include <common/global.h>
#include <common/window.h>
#include <common/shader.h>
#include <common/model.h>

#include "global.cpp"

class instancing_uniform : public Window
{
private:
    Camera *_camera;
    Model *_model;
    Shader *_shader;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    instancing_uniform(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~instancing_uniform();
};

class instancing_vbo : public Window
{
private:
    GLuint _VAO, _VBO, _EBO, _VBO_INS;
    Camera *_camera;
    Texture *_texture;
    Shader *_shader;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    instancing_vbo(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~instancing_vbo();
};

class asteroid_belt : public Window
{
private:
    Camera *_camera;
    Model *_modelPlanet, *_modelRock;
    Shader *_shaderPlanet, *_shaderRock;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    asteroid_belt(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~asteroid_belt();
};

class asteroid_belt_instancing : public Window
{
private:
    GLuint _VBO;
    Camera *_camera;
    Model *_modelPlanet, *_modelRock;
    Shader *_shaderPlanet, *_shaderRock;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    asteroid_belt_instancing(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~asteroid_belt_instancing();
};

/**************************************************** instancing_uniform ****************************************************/

instancing_uniform::~instancing_uniform()
{
    delete _camera;
    delete _model;
    delete _shader;
    _camera = nullptr;
    _model = nullptr;
    _shader = nullptr;
}

void instancing_uniform::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _model = new Model("backpack/backpack.obj");
    _shader = new Shader("4_advanced/10_instancing_uniform.vs", "common/model.fs");
}

void instancing_uniform::onUpdate(GLFWwindow *window)
{
    _shader->use();
    _shader->setmat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    _shader->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shader->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    for (int index = 0, len = instance_offset_vector.size(); index < len; ++index)
    {
        glm::vec3 offset = instance_offset_vector[index];
        _shader->set3f(("offsets[" + std::to_string(index) + "]").c_str(), offset.x, offset.y, offset.z);
    }
    // 需要调用 glDrawArraysInstanced 或 glDrawElementsInstanced 进行绘制
    _model->draw(_shader, instance_offset_vector.size());
}

/**************************************************** instancing_vbo ****************************************************/

instancing_vbo::~instancing_vbo()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
    glDeleteBuffers(1, &_VBO_INS);
    delete _camera;
    Texture::Destroy(_texture);
    delete _shader;
    _camera = nullptr;
    _texture = nullptr;
    _shader = nullptr;
}

void instancing_vbo::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _texture = Texture::Load("texture_diffuse", "container2.png");
    _shader = new Shader("4_advanced/10_instancing_vbo.vs", "common/model.fs");
    // // VAO
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);
    // // VBO
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, cube_vertices_vector.size() * sizeof(Vertex), &cube_vertices_vector[0], GL_STATIC_DRAW);
    // // EBO
    glGenBuffers(1, &_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube_indices_vector.size() * sizeof(unsigned int), &cube_indices_vector[0], GL_STATIC_DRAW);
    // // 顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);
    // // 实例化数组
    glGenBuffers(1, &_VBO_INS);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO_INS);
    glBufferData(GL_ARRAY_BUFFER, instance_offset_vector.size() * sizeof(glm::vec3), &instance_offset_vector[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(3);
    // // 设置顶点属性更新方式
    // // 参数1: 哪个顶点属性
    // // 参数1: 属性除数 0=每次迭代时更新 1=渲染新实例时更新 2=每2个实例更新一次
    glVertexAttribDivisor(3, 1);
}

void instancing_vbo::onUpdate(GLFWwindow *window)
{
    _shader->use();
    _shader->setmat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    _shader->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shader->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture->getTexID());
    _shader->set1i("texture_diffuse0", 0);
    glBindVertexArray(_VAO);
    glDrawElementsInstanced(GL_TRIANGLES, cube_indices_vector.size(), GL_UNSIGNED_INT, 0, instance_offset_vector.size());
}

/**************************************************** asteroid_belt ****************************************************/

asteroid_belt::~asteroid_belt()
{
    delete _camera;
    delete _modelPlanet;
    delete _modelRock;
    delete _shaderPlanet;
    delete _shaderRock;
    _camera = nullptr;
    _modelPlanet = nullptr;
    _modelRock = nullptr;
    _shaderPlanet = nullptr;
    _shaderRock = nullptr;
}

void asteroid_belt::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _camera->setPosition(0.0f, 10.0f, 50.0f);
    _modelPlanet = new Model("planet/planet.obj");
    _modelRock = new Model("rock/rock.obj");
    _shaderPlanet = new Shader("common/model.vs", "common/model.fs");
    _shaderRock = new Shader("common/model.vs", "common/model.fs");

    init_rock_model_matrices();
}

void asteroid_belt::onUpdate(GLFWwindow *window)
{
    glEnable(GL_CULL_FACE);
    // 绘制星球
    _shaderPlanet->use();
    _shaderPlanet->setmat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    _shaderPlanet->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderPlanet->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _modelPlanet->draw(_shaderPlanet);
    // 绘制岩石
    _shaderRock->use();
    _shaderRock->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderRock->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    for (int index = 0, len = rock_model_matrices.size(); index < len; ++index)
    {
        _shaderRock->setmat4fv("model", glm::value_ptr(rock_model_matrices[index]));
        _modelRock->draw(_shaderRock);
    }
}

/**************************************************** asteroid_belt_instancing ****************************************************/

asteroid_belt_instancing::~asteroid_belt_instancing()
{
    glDeleteBuffers(1, &_VBO);
    delete _camera;
    delete _modelPlanet;
    delete _modelRock;
    delete _shaderPlanet;
    delete _shaderRock;
    _camera = nullptr;
    _modelPlanet = nullptr;
    _modelRock = nullptr;
    _shaderPlanet = nullptr;
    _shaderRock = nullptr;
}

void asteroid_belt_instancing::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _camera->setPosition(0.0f, 10.0f, 50.0f);
    _modelPlanet = new Model("planet/planet.obj");
    _modelRock = new Model("rock/rock.obj");
    _shaderPlanet = new Shader("common/model.vs", "common/model.fs");
    _shaderRock = new Shader("common/model_instance.vs", "common/model.fs");

    init_rock_model_matrices();

    unsigned long long vec4Size = sizeof(glm::vec4);
    unsigned long long mat4Size = sizeof(glm::mat4);
    std::vector<Mesh*> meshes = _modelRock->getMeshes();

    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, rock_model_matrices.size() * mat4Size, &rock_model_matrices[0], GL_STATIC_DRAW);

    for (auto mesh : meshes)
    {
        glBindVertexArray(mesh->getVAO());;
        // 顶点属性 mat4本质就是4个vec4
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(0 * vec4Size));
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(1 * vec4Size));
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(2 * vec4Size));
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(3 * vec4Size));
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
    }
}

void asteroid_belt_instancing::onUpdate(GLFWwindow *window)
{
    glEnable(GL_CULL_FACE);
    // 绘制星球
    _shaderPlanet->use();
    _shaderPlanet->setmat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    _shaderPlanet->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderPlanet->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _modelPlanet->draw(_shaderPlanet);
    // 绘制岩石
    _shaderRock->use();
    _shaderRock->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderRock->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _modelRock->draw(_shaderRock, rock_model_matrices.size());
}

/**************************************************** test ****************************************************/

int main()
{
    // 使用uniform配合gl_InstanceID进行实例化 容易达到uniform限制
    Window *window1 = new instancing_uniform("10_instancing_uniform", 800, 600);
    window1->start();
    delete window1;

    // 使用实例化数组进行实例化
    Window *window2 = new instancing_vbo("10_instancing_vbo", 800, 600);
    window2->start();
    delete window2;

    // 小行星带
    Window *window3 = new asteroid_belt("10_asteroid_belt", 800, 600);
    window3->start();
    delete window3;

    // 小行星带实例化
    Window *window4 = new asteroid_belt_instancing("10_asteroid_belt_instancing", 800, 600);
    window4->start();
    delete window4;

    return 0;
}