#include <common/global.h>
#include <common/window.h>

class advanced_data : public Window
{
private:
    GLuint _VAO, _VBO, _EBO;
    Shader *_shader;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    advanced_data(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~advanced_data();
};

/**************************************************** advanced_data ****************************************************/

advanced_data::~advanced_data()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
    delete _shader;
    _shader = nullptr;
}

void advanced_data::onInit(GLFWwindow *window)
{
    _shader = new Shader("1_start/vertexColor.vs", "1_start/vertexColor.fs");
    
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);

    glGenBuffers(1, &_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

    // 顶点
    float vertices[][3] = {
        { -0.5f, -0.5f, +0.0f }, // 左下角
        { +0.5f, -0.5f, +0.0f }, // 右下角
        { +0.5f, +0.5f, +0.0f }, // 右上角
        { -0.5f, +0.5f, +0.0f }, // 左上角
    };
    float vertexSize = sizeof(vertices[0]);
    // 分配内存但不进行填充
    glBufferData(GL_ARRAY_BUFFER, vertexSize * 4, nullptr, GL_STATIC_DRAW);
    // 填充指定区域
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize * 0, vertexSize, &vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize * 1, vertexSize, &vertices[1]);
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize * 2, vertexSize, &vertices[2]);
    glBufferSubData(GL_ARRAY_BUFFER, vertexSize * 3, vertexSize, &vertices[3]);



    // 顶点索引
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,
    };
    // 分配内存但不进行填充
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), nullptr, GL_STATIC_DRAW);
    // 获取指针
    void *ptr = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    // 通过指针复制数据到内存
    memcpy(ptr, indices, sizeof(indices));
    // 删除指针
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    // 设置顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void advanced_data::onUpdate(GLFWwindow *window)
{
    _shader->use();
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/**************************************************** test ****************************************************/

int main()
{
    // 高级属性
    Window *window1 = new advanced_data("7_advanced_data", 800, 600);
    window1->start();
    delete window1;

    // 分批顶点属性
    // 顶点属性可以交错存储 也可以按类型连续存储（位置、法向量、纹理坐标依次存储）
    // 推荐交错存储 顶点着色器运行时所需顶点属性在内存中紧密排列

    // 复制缓冲
    // 使用 glCopyBufferSubData 可以实现不同缓冲间复制
    // 可以将同类型缓冲分别绑定到 GL_COPY_READ_BUFFER 和 GL_COPY_WRITE_BUFFER 从而达到同类型间缓冲复制
    // glCopyBufferSubData(GLenum readtarget, GLenum writetarget, GLintptr readoffset, GLintptr writeoffset, GLsizeiptr size);

    return 0;
}