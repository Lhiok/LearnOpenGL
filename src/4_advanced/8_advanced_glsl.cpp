#include <common/global.h>
#include <common/window.h>
#include <common/mesh.h>
#include <common/shader.h>
#include <common/camera.h>

class uniform_buffer : public Window
{
private:
    GLuint _UBO; // uniform缓冲对象
    Camera *_camera;
    Mesh *_meshCube;
    Shader *_shaderCubeA, *_shaderCubeB;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    uniform_buffer(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~uniform_buffer();
};

/**************************************************** uniform_buffer ****************************************************/

uniform_buffer::~uniform_buffer()
{
    glDeleteBuffers(1, &_UBO);
    delete _camera;
    delete _meshCube;
    delete _shaderCubeA;
    delete _shaderCubeB;
    _camera = nullptr;
    _meshCube = nullptr;
    _shaderCubeA = nullptr;
    _shaderCubeB = nullptr;
}

void uniform_buffer::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _meshCube = new Mesh(cube_vertices_vector, cube_indices_vector, std::vector<Texture*> { Texture::Load("texture_diffuse", "container2.png") });
    _shaderCubeA = new Shader("4_advanced/8_uniform_buffer.vs", "4_advanced/8_uniform_buffer.fs");
    _shaderCubeB = new Shader("4_advanced/8_uniform_buffer.vs", "4_advanced/8_uniform_buffer.fs");
    
    // 创建uniform缓冲对象
    glGenBuffers(1, &_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, _UBO);
    // 预分配内存 大小更具std140布局计算
    glBufferData(GL_UNIFORM_BUFFER, 160, nullptr, GL_STATIC_DRAW);

    // 链接ubo到绑定点
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, _UBO);
    // 使用glBindBufferRange可以只链接部分数据 偏移必须是GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT的倍数
    // glBindBufferRange(GL_UNIFORM_BUFFER, 1, _UBO, 0, 160);
    // 链接shader到绑定点
    _shaderCubeA->bindUniformBlock("CameraBlock", 1);
    _shaderCubeB->bindUniformBlock("CameraBlock", 1);
}

void uniform_buffer::onUpdate(GLFWwindow *window)
{
    // 更新ubo数据
    glBindBuffer(GL_UNIFORM_BUFFER, _UBO);
    // 偏移根据std140布局计算
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(_camera->view()));
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(_camera->projection()));

    _shaderCubeA->use();
    _shaderCubeA->set3f("color", 1.0f, 0.0f, 0.0f);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
    _shaderCubeA->setmat4fv("model", glm::value_ptr(model));
    _meshCube->draw(_shaderCubeA);

    _shaderCubeB->use();
    _shaderCubeB->set3f("color", 0.0f, 1.0f, 0.0f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(+1.0f, 0.0f, 0.0f));
    _shaderCubeB->setmat4fv("model", glm::value_ptr(model));
    _meshCube->draw(_shaderCubeB);
}

/**************************************************** test ****************************************************/

int main()
{
    // 内建变量-顶点着色器
    // gl_Position 输出
    // gl_PointSize 可写 顶点大小 使用glEnable(GL_PROGRAM_POINT_SIZE)启用修改
    // gl_VertexID 只读 顶点ID

    // 内建变量-片段着色器
    // gl_FragCoord 只读 xy分量对应屏幕坐标 z分量对应片段深度值
    // gl_FrontFacing 只读 顶点环绕方式
    // gl_FragDepth 可写 修改深度值[0, 1] 修改深度值会禁用所有的提前深度测试 影响性能
    // OpenGL Version >= 4.2 可重新声明gl_FragDepth调和提前深度测试
    // layout (depth_<condition>) out float gl_FragDepth;
    // any 默认值 禁用提前深度测试
    // greater 你只能让深度值比gl_FragCoord.z更大
    // less	你只能让深度值比gl_FragCoord.z更小
    // unchanged 如果你要写入gl_FragDepth 你将只能写入gl_FragCoord.z的值

    // uniform缓冲对象
    Window *window1 = new uniform_buffer("8_uniform_buffer", 800, 600);
    window1->start();
    delete window1;

    return 0;
}