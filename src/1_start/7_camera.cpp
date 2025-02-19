#include <common/global.h>
#include <common/window.h>
#include <common/mesh.h>
#include <common/shader.h>

#include "global.cpp"

class camera : public Window
{
private:
    GLuint _VAO, _VBO, _EBO;
    Shader *_shader;
    Texture *_texture0, *_texture1;
    std::string _vertexPath, _fragmentPath;
    glm::mat4 _model, _view, _projection;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    camera(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath);
    ~camera();
};

class camera_move : public Window
{
private:
    glm::mat4 _model;
    Camera *_camera;
    Mesh *_mesh;
    Shader *_shader;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    camera_move(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath);
    ~camera_move();
};

/**************************************************** camera ****************************************************/

camera::camera(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height)
{
    _vertexPath = vertexPath;
    _fragmentPath = fragmentPath;
}

camera::~camera()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
    delete _shader;
    delete _texture0;
    delete _texture1;
    _shader = nullptr;
    _texture0 = nullptr;
    _texture1 = nullptr;
    _vertexPath = nullptr;
    _fragmentPath = nullptr;
}

void camera::onInit(GLFWwindow *window)
{
    // 顶点数组对象
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    // 顶点缓冲对象
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // 索引缓冲对象
    glGenBuffers(1, &_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // 链接顶点属性-位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 链接顶点属性-纹理
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 着色器程序
    _shader = new Shader(_vertexPath, _fragmentPath);
    _texture0 = new Texture("texture0", "container.jpg", GL_RGB);
    _texture1 = new Texture("texture1", "awesomeface.png", GL_RGBA);
}

void camera::onUpdate(GLFWwindow *window)
{
    _shader->use();

    // 视图矩阵
    float radius = 10.0f;
    float camX = sin(glfwGetTime()) * radius;
    float camZ = cos(glfwGetTime()) * radius;
    // 参数1: 相机位置
    // 参数2: 相机观察点
    // 参数3: 相机上方向
    _view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    _shader->setmat4fv("view", glm::value_ptr(_view));

    float width = 800.0f, height = 600.0f;

    // 投射投影矩阵
    // 参数1: FOV 视野 通常45度
    // 参数2: 宽高比
    // 参数3: 近裁面
    // 参数4: 远裁面
    _projection = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 100.0f);
    _shader->setmat4fv("projection", glm::value_ptr(_projection));

    // 绑定纹理单元
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture0->getTexID());
    // 设置uniform变量
    _shader->set1i("texture0", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _texture1->getTexID());
    _shader->set1i("texture1", 1);

    glBindVertexArray(_VAO);

    for (unsigned int index = 0, len = sizeof(cube_positions) / sizeof(cube_positions[0]); index < len; index++)
    {
        // 模型矩阵
        _model = glm::mat4(1.0f);
        _model = glm::translate(_model, cube_positions[index]);
        _model = glm::rotate(_model, glm::radians(20.0f * index), glm::vec3(1.0f, 0.3f, 0.5f));
        _shader->setmat4fv("model", glm::value_ptr(_model));

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

/**************************************************** camera_move ****************************************************/

camera_move::camera_move(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height)
{
    _vertexPath = vertexPath;
    _fragmentPath = fragmentPath;
}

camera_move::~camera_move()
{
    delete _camera;
    delete _mesh;
    delete _shader;
    _camera = nullptr;
    _mesh = nullptr;
    _shader = nullptr;
    _vertexPath = nullptr;
    _fragmentPath = nullptr;
}

void camera_move::onInit(GLFWwindow *window)
{
    // Camera
    _camera = new Camera(window);
    // Mesh
    Texture *cube_texture0 = new Texture("texture0", "container.jpg", GL_RGB);
    Texture *cube_texture1 = new Texture("texture1", "awesomeface.png", GL_RGBA);
    std::vector<Texture*> cube_texture_vector = {
        cube_texture0,
        cube_texture1,
    };
    _mesh = new Mesh(cube_vertices_vector, cube_indices_vector, cube_texture_vector);
    // Shader
    _shader = new Shader(_vertexPath, _fragmentPath);
}

void camera_move::onUpdate(GLFWwindow *window)
{
    _shader->use();

    // 视图矩阵
    _shader->setmat4fv("view", glm::value_ptr(_camera->view()));
    // 投射投影矩阵
    _shader->setmat4fv("projection", glm::value_ptr(_camera->projection()));

    for (unsigned int index = 0, len = sizeof(cube_positions) / sizeof(cube_positions[0]); index < len; index++)
    {
        // 模型矩阵
        _model = glm::mat4(1.0f);
        _model = glm::translate(_model, cube_positions[index]);
        _model = glm::rotate(_model, glm::radians(20.0f * index), glm::vec3(1.0f, 0.3f, 0.5f));
        _shader->setmat4fv("model", glm::value_ptr(_model));

        _mesh->draw(_shader);
    }
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new camera("7_camera", 800, 600, "1_start/coordinateSystems.vs", "1_start/textureMixTexture.fs");
    window1->start();
    Window *window2 = new camera_move("7_camera_move", 800, 600, "1_start/coordinateSystems.vs", "1_start/textureMixTexture.fs");
    window2->start();
    return 0;
}
