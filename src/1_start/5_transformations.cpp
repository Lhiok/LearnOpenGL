#include <common/window.h>
#include <common/shader.h>
#include <common/texture.h>
#include <common/global.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class transformations : public Window
{
private:
    GLuint _VAO, _VBO, _EBO;
    Shader *_shader;
    Texture *_texture0, *_texture1;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    transformations(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath);
    ~transformations();
};

/**************************************************** transformations ****************************************************/

transformations::transformations(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height)
{
    _vertexPath = vertexPath;
    _fragmentPath = fragmentPath;
}

transformations::~transformations()
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

void transformations::onInit(GLFWwindow *window)
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
    _texture0 = new Texture("texture0", "container.jpg", GL_RGB);
    _texture1 = new Texture("texture1", "awesomeface.png", GL_RGBA);
}

void transformations::onUpdate(GLFWwindow *window)
{
    _shader->use();

    // 绑定纹理单元
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture0->getTexID());
    // 设置uniform变量
    _shader->setli("texture0", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _texture1->getTexID());
    _shader->setli("texture1", 1);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
    _shader->setmat4fv("transform", glm::value_ptr(trans));

    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new transformations("5_transformations", 800, 600, "1_start/transformations.vs", "1_start/textureMixTexture.fs");
    window1->start();
    return 0;
}
