#include <common/global.h>
#include <common/window.h>
#include <common/model.h>
#include <common/shader.h>
#include <common/camera.h>
#include <common/light.h>

#include <cstdlib>
#include <ctime>

class screen_space_ambient_occlusion : public Window
{
private:
    GLuint _FBO, _FBO_SSAO, _FBO_SSAO_BLUR, _RBO, _TEX_POS, _TEX_NOR, _TEX_COL, _TEX_NOISE, _TEX_SSAO, _TEX_SSAO_BLUR;
    Camera *_camera;
    Model *_modelRobot;
    Mesh *_meshFloor, *_meshScreen;
    Shader *_shaderGBuffer, *_shaderSSAO, *_shaderBlur, *_shaderDeferred;
    std::vector<glm::vec3> _ssaoKernel, _ssaoNoise;
    DirectionalLight *_dirLight;
    float _ssaoRadius;
    int _ssaoKernelSize;
    bool _enableSSAO;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
    virtual void processInput(GLFWwindow *window);
public:
    screen_space_ambient_occlusion(std::string name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~screen_space_ambient_occlusion();
};

/**************************************************** screen_space_ambient_occlusion ****************************************************/

screen_space_ambient_occlusion::~screen_space_ambient_occlusion()
{
    glDeleteFramebuffers(1, &_FBO);
    glDeleteFramebuffers(1, &_FBO_SSAO);
    glDeleteFramebuffers(1, &_FBO_SSAO_BLUR);
    glDeleteRenderbuffers(1, &_RBO);
    glDeleteTextures(1, &_TEX_POS);
    glDeleteTextures(1, &_TEX_NOR);
    glDeleteTextures(1, &_TEX_COL);
    glDeleteTextures(1, &_TEX_NOISE);
    glDeleteTextures(1, &_TEX_SSAO);
    glDeleteTextures(1, &_TEX_SSAO_BLUR);
    delete _camera;
    delete _modelRobot;
    delete _meshFloor;
    delete _meshScreen;
    delete _shaderGBuffer;
    delete _shaderSSAO;
    delete _shaderBlur;
    delete _shaderDeferred;
    delete _dirLight;
    _camera = nullptr;
    _modelRobot = nullptr;
    _meshFloor = nullptr;
    _meshScreen = nullptr;
    _shaderGBuffer = nullptr;
    _shaderSSAO = nullptr;
    _shaderBlur = nullptr;
    _shaderDeferred = nullptr;
    _dirLight = nullptr;
}

void screen_space_ambient_occlusion::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _modelRobot = new Model("nanosuit_reflection/nanosuit.obj");
    _meshFloor = new Mesh(floor_vertices_vector, rectangle_indices_vector, { Texture::Load("texture_diffuse", "wood.png") });
    _meshScreen = new Mesh(screen_vertices_vector, rectangle_indices_vector, {});
    _shaderGBuffer = new Shader("5_advanced_lighting/10_gbuffer.vs", "5_advanced_lighting/10_gbuffer.fs");
    _shaderSSAO = new Shader("5_advanced_lighting/10_ssao.vs", "5_advanced_lighting/10_ssao.fs");
    _shaderBlur = new Shader("5_advanced_lighting/10_ssao.vs", "5_advanced_lighting/10_ssao_blur.fs");
    _shaderDeferred = new Shader("5_advanced_lighting/10_ssao.vs", "5_advanced_lighting/10_deferred.fs");
    _dirLight = new DirectionalLight(
        glm::vec3(-0.2f, -1.0f, -0.3f),
        glm::vec3(0.3f, 0.3f, 0.3f),
        glm::vec3(0.3f, 0.3f, 0.3f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    );

    int width = 0, height = 0;
    glfwGetWindowSize(window, &width, &height);

    // 帧缓冲-GBuffer
    glGenFramebuffers(1, &_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    // 颜色缓冲-位置
    glGenTextures(1, &_TEX_POS);
    glBindTexture(GL_TEXTURE_2D, _TEX_POS);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // 不设置过滤取样颜色为0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _TEX_POS, 0);
    // 颜色缓冲-法线
    glGenTextures(1, &_TEX_NOR);
    glBindTexture(GL_TEXTURE_2D, _TEX_NOR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _TEX_NOR, 0);
    // 颜色缓冲-颜色&镜面强度
    glGenTextures(1, &_TEX_COL);
    glBindTexture(GL_TEXTURE_2D, _TEX_COL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _TEX_COL, 0);
    // 设置多渲染目标
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // 渲染缓冲附件 深度缓冲
    glGenRenderbuffers(1, &_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, _RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _RBO);
    // 检查帧缓存是否完整
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer not complete: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    }

    // 帧缓冲-SSAO
    glGenFramebuffers(1, &_FBO_SSAO);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO_SSAO);
    // 颜色缓冲-SSAO
    glGenTextures(1, &_TEX_SSAO);
    glBindTexture(GL_TEXTURE_2D, _TEX_SSAO);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _TEX_SSAO, 0);
    // 检查帧缓存是否完整
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer not complete: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    }

    // 帧缓冲-SSAO_BLUR
    glGenFramebuffers(1, &_FBO_SSAO_BLUR);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO_SSAO_BLUR);
    // 颜色缓冲-SSAO_BLUR
    glGenTextures(1, &_TEX_SSAO_BLUR);
    glBindTexture(GL_TEXTURE_2D, _TEX_SSAO_BLUR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _TEX_SSAO_BLUR, 0);
    // 检查帧缓存是否完整
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer not complete: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    }

    std::srand(std::time(0));
    // 法向半球生成
    for (int i = 0; i < 64; ++i)
    {
        glm::vec3 sample = glm::vec3(
            std::rand() * 1.0f / RAND_MAX * 2.0f - 1.0f,
            std::rand() * 1.0f / RAND_MAX * 2.0f - 1.0f,
            std::rand() * 1.0f / RAND_MAX
        );
        sample = glm::normalize(sample);
        sample *= std::rand() * 1.0f / RAND_MAX;
        float scale = i / 64.0f;
        sample *= 0.1f + scale * scale * 0.9f;
        _ssaoKernel.push_back(sample);
    }
    // 随机转动核心
    for (int i = 0; i < 16; ++i)
    {
        _ssaoNoise.push_back(glm::vec3(
            std::rand() * 1.0f / RAND_MAX * 2.0f - 1.0f,
            std::rand() * 1.0f / RAND_MAX * 2.0f - 1.0f,
            0.0f
        ));
    }
    glGenTextures(1, &_TEX_NOISE);
    glBindTexture(GL_TEXTURE_2D, _TEX_NOISE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGB, GL_FLOAT, &_ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    _enableSSAO = true;
    std::cout << (_enableSSAO? "SSAO enabled": "SSAO disabled") << std::endl;
    _ssaoRadius = 0.5f;
    std::cout << "SSAO radius: " << _ssaoRadius << std::endl;
    _ssaoKernelSize = 32;
    std::cout << "SSAO kernel size: " << _ssaoKernelSize << std::endl;
}

void screen_space_ambient_occlusion::onUpdate(GLFWwindow *window)
{
    // 渲染GBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    _shaderGBuffer->use();
    _shaderGBuffer->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderGBuffer->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    glm::mat4 modelRobot = glm::mat4(1.0f);
    modelRobot = glm::translate(modelRobot, glm::vec3(0.0f, 1.0f, 0.0f));
    modelRobot = glm::rotate(modelRobot, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    _shaderGBuffer->setmat4fv("model", glm::value_ptr(modelRobot));
    _modelRobot->draw(_shaderGBuffer);
    glm::mat4 modelFloor = glm::mat4(1.0f);
    modelFloor = glm::scale(modelFloor, glm::vec3(50.0f, 0.1f, 50.0f));
    _shaderGBuffer->setmat4fv("model", glm::value_ptr(modelFloor));
    _meshFloor->draw(_shaderGBuffer);

    // SSAO
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO_SSAO);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    _shaderSSAO->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _TEX_POS);
    _shaderSSAO->set1i("gPosition", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _TEX_NOR);
    _shaderSSAO->set1i("gNormal", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _TEX_NOISE);
    _shaderSSAO->set1i("texNoise", 2);
    _shaderSSAO->set1i("kernelSize", _ssaoKernelSize);
    _shaderSSAO->set1f("radius", _ssaoRadius);
    for (int i = 0; i < 64; ++i)
    {
        _shaderSSAO->setmat4fv(("samples[" + std::to_string(i) + "]").c_str(), glm::value_ptr(_ssaoKernel[i]));
    }
    _shaderSSAO->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _meshScreen->draw(_shaderSSAO);

    // SSAO-BLUR
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO_SSAO_BLUR);
    glClear(GL_COLOR_BUFFER_BIT);
    _shaderBlur->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _TEX_SSAO);
    _shaderBlur->set1i("ssao", 0);
    _meshScreen->draw(_shaderBlur);

    // 延迟渲染
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    _shaderDeferred->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _TEX_POS);
    _shaderDeferred->set1i("texture_position", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _TEX_NOR);
    _shaderDeferred->set1i("texture_normal", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _TEX_COL);
    _shaderDeferred->set1i("texture_color", 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _TEX_SSAO_BLUR);
    _shaderDeferred->set1i("texture_ssao", 3);
    _shaderDeferred->set1i("enableSSAO", (int)_enableSSAO);
    _dirLight->draw(_shaderDeferred);
    _meshScreen->draw(_shaderDeferred);
}

void screen_space_ambient_occlusion::processInput(GLFWwindow *window)
{
    if (Input::isKeyDown(window, GLFW_KEY_SPACE))
    {
        _enableSSAO = !_enableSSAO;
        std::cout << (_enableSSAO? "SSAO enabled": "SSAO disabled") << std::endl;
    }
    if (Input::isKeyDown(window, GLFW_KEY_KP_ADD))
    {
        _ssaoRadius += 0.1f;
        std::cout << "SSAO radius: " << _ssaoRadius << std::endl;
    }
    if (Input::isKeyDown(window, GLFW_KEY_KP_SUBTRACT))
    {
        _ssaoRadius = std::max(0.1f, _ssaoRadius - 0.1f);
        std::cout << "SSAO radius: " << _ssaoRadius << std::endl;
    }
    if (Input::isKeyDown(window, GLFW_KEY_KP_MULTIPLY))
    {
        _ssaoKernelSize = std::min(64, _ssaoKernelSize << 1);
        std::cout << "SSAO kernel size: " << _ssaoKernelSize << std::endl;
    }
    if (Input::isKeyDown(window, GLFW_KEY_KP_DIVIDE))
    {
        _ssaoKernelSize = std::max(2, _ssaoKernelSize >> 1);
        std::cout << "SSAO kernel size: " << _ssaoKernelSize << std::endl;
    }

    Window::processInput(window);
}

/**************************************************** test ****************************************************/

int main()
{
    // 屏幕空间环境光遮蔽
    Window *window1 = new screen_space_ambient_occlusion("10_screen_space_ambient_occlusion", 800, 600);
    window1->start();
    delete window1;

    return 0;
}