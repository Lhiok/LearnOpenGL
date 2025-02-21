#include <common/window.h>
#include <common/shader.h>
#include <common/texture.h>

#include "global.cpp"

class coordinate_systems_square : public Window
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
    coordinate_systems_square(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath);
    ~coordinate_systems_square();
};

class coordinate_systems_cube : public Window
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
    coordinate_systems_cube(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath);
    ~coordinate_systems_cube();
};

/**************************************************** coordinate_systems_square ****************************************************/

coordinate_systems_square::coordinate_systems_square(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height)
{
    _vertexPath = vertexPath;
    _fragmentPath = fragmentPath;
}

coordinate_systems_square::~coordinate_systems_square()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
    delete _shader;
    Texture::Destroy(_texture0);
    Texture::Destroy(_texture1);
    _shader = nullptr;
    _texture0 = nullptr;
    _texture1 = nullptr;
}

void coordinate_systems_square::onInit(GLFWwindow *window)
{
    // 顶点数组对象
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    // 顶点缓冲对象
    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);

    // 索引缓冲对象
    glGenBuffers(1, &_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW);

    // 链接顶点属性-位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 链接顶点属性-纹理
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 着色器程序
    _shader = new Shader(_vertexPath, _fragmentPath);
    _texture0 = Texture::Load("texture0", "container.jpg");
    _texture1 = Texture::Load("texture1", "awesomeface.png");
}

void coordinate_systems_square::onUpdate(GLFWwindow *window)
{
    _shader->use();

    // 模型矩阵
    _model = glm::mat4(1.0f);
    _model = glm::rotate(_model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // 立方体太小正交矩阵看不见
    // _model = glm::scale(_model, glm::vec3(200.0f, 200.0f, 200.0f));
    _shader->setmat4fv("model", glm::value_ptr(_model));

    // 视图矩阵
    _view = glm::mat4(1.0f);
    _view = glm::translate(_view, glm::vec3(0.0f, 0.0f, -3.0f));
    _shader->setmat4fv("view", glm::value_ptr(_view));

    float width = 800.0f, height = 600.0f;

    // 正交投影矩阵
    // 左 右 上 下 前 后
    // _projection = glm::ortho(-width / 2, width / 2, -height / 2, height / 2, 0.1f, 100.0f);

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
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/**************************************************** coordinate_systems_cube ****************************************************/

coordinate_systems_cube::coordinate_systems_cube(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height)
{
    _vertexPath = vertexPath;
    _fragmentPath = fragmentPath;
}

coordinate_systems_cube::~coordinate_systems_cube()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
    delete _shader;
    Texture::Destroy(_texture0);
    Texture::Destroy(_texture1);
    _shader = nullptr;
    _texture0 = nullptr;
    _texture1 = nullptr;
}

void coordinate_systems_cube::onInit(GLFWwindow *window)
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
    _texture0 = Texture::Load("texture0", "container.jpg");
    _texture1 = Texture::Load("texture1", "awesomeface.png");
}

void coordinate_systems_cube::onUpdate(GLFWwindow *window)
{
    _shader->use();

    // 视图矩阵
    _view = glm::mat4(1.0f);
    _view = glm::translate(_view, glm::vec3(0.0f, 0.0f, -3.0f));
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

    glm::vec3 cubePositions[] = {
        glm::vec3(+0.0f, +0.0f, +0.0f), 
        glm::vec3(+2.0f, +5.0f, -15.f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.f),  
        glm::vec3(+2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f, +3.0f, -7.5f),  
        glm::vec3(+1.3f, -2.0f, -2.5f),  
        glm::vec3(+1.5f, +2.0f, -2.5f), 
        glm::vec3(+1.5f, +0.2f, -1.5f), 
        glm::vec3(-1.3f, +1.0f, -1.5f),  
    };
    for (unsigned int index = 0, len = sizeof(cubePositions) / sizeof(cubePositions[0]); index < len; index++)
    {
        // 模型矩阵
        _model = glm::mat4(1.0f);
        _model = glm::translate(_model, cubePositions[index]);
        _model = glm::rotate(_model, glm::radians(20.0f * index), glm::vec3(1.0f, 0.3f, 0.5f));
        _shader->setmat4fv("model", glm::value_ptr(_model));

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new coordinate_systems_square("6_coordinate_system_square", 800, 600, "1_start/coordinateSystems.vs", "1_start/textureMixTexture.fs");
    window1->start();
    delete window1;

    Window *window2 = new coordinate_systems_cube("6_coordinate_system_cube", 800, 600, "1_start/coordinateSystems.vs", "1_start/textureMixTexture.fs");
    window2->start();
    delete window2;
    
    return 0;
}
