#include <common/window.h>
#include <common/global.h>
#include <common/model.h>
#include <common/shader.h>
#include <common/light.h>

#include "global.cpp"

class face_culling : public Window
{
private:
    GLuint _fbo, _tex, _rbo;
    Camera *_camera;
    Mesh *_meshCube, *_meshGrass, *_meshScreen;
    Shader *_shaderCube, *_shaderGrass, *_shaderScreen;
    std::string _vertexPath, _fragmentPath;
    void renderScene();
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    face_culling(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height) { _vertexPath = vertexPath; _fragmentPath = fragmentPath; }
    ~face_culling();
};

/**************************************************** face_culling ****************************************************/

face_culling::~face_culling()
{
    glDeleteFramebuffers(1, &_fbo);
    glDeleteTextures(1, &_tex);
    delete _camera;
    delete _meshCube;
    delete _meshGrass;
    delete _meshScreen;
    delete _shaderCube;
    delete _shaderGrass;
    delete _shaderScreen;
    _camera = nullptr;
    _meshCube = nullptr;
    _meshGrass = nullptr;
    _shaderCube = nullptr;
    _shaderGrass = nullptr;
}

void face_culling::renderScene()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    _shaderCube->use();
    _shaderCube->setmat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    _shaderCube->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderCube->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _meshCube->draw(_shaderCube);

    glDisable(GL_CULL_FACE);

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

void face_culling::onInit(GLFWwindow *window)
{
    Camera::SetMainCamera(_camera = new Camera(window));
    _meshCube = new Mesh(cube_vertices_vector, cube_indices_vector, std::vector<Texture*> { Texture::Load("texture_diffuse", "container2.png") });
    _shaderCube = new Shader("4_advanced/2_stencil_testing.vs", "4_advanced/2_stencil_testing.fs");
    _meshGrass = new Mesh(rectangle_vertices_vector, rectangle_indices_vector, std::vector<Texture*> { Texture::Load("texture_diffuse", "grass.png") });
    _shaderGrass = new Shader("4_advanced/2_stencil_testing.vs", "4_advanced/3_discard.fs");
    _meshScreen = new Mesh(screen_vertices_vector, rectangle_indices_vector, std::vector<Texture*> { });
    _shaderScreen = new Shader(_vertexPath, _fragmentPath);

    // 创建帧缓冲对象
    glGenFramebuffers(1, &_fbo);
    // 绑定帧缓冲对象
    // GL_FRAMEBUFFER: 帧缓冲对象
    // GL_READ_FRAMEBUFFER: 读取帧缓冲对象
    // GL_DRAW_FRAMEBUFFER: 渲染帧缓冲对象
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    // 纹理附件
    glGenTextures(1, &_tex);
    glBindTexture(GL_TEXTURE_2D, _tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 将纹理附件附加到帧缓冲对象
    // 参数1: 目标
    // 参数2: 附件类型
    //      * GL_COLOR_ATTACHMENTX: 颜色附件
    //      * GL_DEPTH_ATTACHMENT: 深度附件 对应格式 GL_DEPTH_COMPONENT
    //      * GL_STENCIL_ATTACHMENT: 模板附件 对应格式 GL_STENCIL_INDEX
    //      * GL_DEPTH_STENCIL_ATTACHMENT: 深度模板附件
    // 参数3: 附加的纹理类型
    // 参数4: 附加的纹理ID
    // 参数5: 多级渐远纹理的级别
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex, 0);

    // 深度模板附件 24位深度+8位模板
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _tex, 0);

    // 渲染缓冲附件 数据存储使用OpenGL原生渲染格式 只写 无法读取
    glGenRenderbuffers(1, &_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    // 将渲染缓冲附件附加到帧缓冲对象
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

    // 检查帧缓冲是否完整
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer is not complete!" << std::endl;    
    }

    // 解绑帧缓冲对象
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void face_culling::onUpdate(GLFWwindow *window)
{
    // 将场景绘制到帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    renderScene();

    // 后处理
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    _shaderScreen->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tex);
    _shaderScreen->set1i("texture_screen", 0);
    _meshScreen->draw(_shaderScreen);
}


/**************************************************** test ****************************************************/

int main()
{
    // 无任何后处理
    Window *window1 = new face_culling("5_face_culling", 800, 600, "4_advanced/5_frame_buffers.vs", "4_advanced/5_frame_buffers.fs");
    window1->start();
    delete window1;

    // 反相
    Window *window2 = new face_culling("5_face_culling", 800, 600, "4_advanced/5_frame_buffers.vs", "4_advanced/5_frame_buffers_reverse.fs");
    window2->start();
    delete window2;

    // 灰度
    Window *window3 = new face_culling("5_face_culling", 800, 600, "4_advanced/5_frame_buffers.vs", "4_advanced/5_frame_buffers_grayscale.fs");
    window3->start();
    delete window3;

    // 锐化
    Window *window4 = new face_culling("5_face_culling", 800, 600, "4_advanced/5_frame_buffers.vs", "4_advanced/5_frame_buffers_sharpen.fs");
    window4->start();
    delete window4;

    // 模糊
    Window *window5 = new face_culling("5_face_culling", 800, 600, "4_advanced/5_frame_buffers.vs", "4_advanced/5_frame_buffers_blur.fs");
    window5->start();
    delete window5;

    // 边缘检测
    Window *window6 = new face_culling("5_face_culling", 800, 600, "4_advanced/5_frame_buffers.vs", "4_advanced/5_frame_buffers_edge.fs");
    window6->start();
    delete window6;

    return 0;
}