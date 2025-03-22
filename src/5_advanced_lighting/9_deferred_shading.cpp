#include <common/global.h>
#include <common/window.h>
#include <common/mesh.h>
#include <common/shader.h>
#include <common/camera.h>

class deferred_shading : public Window
{
private:
    GLuint _FBO, _RBO, _TEX_POS, _TEX_NOR, _TEX_COL;
    Camera *_camera;
    Mesh *_mesh, *_meshScreen, *_meshLight;
    Shader *_shaderGBuffer, *_shaderDeferredNormal, *_shaderDeferredLightVolume, *_shaderLight;
    glm::vec3 *_lightPositions, *_lightColors;
    bool _enableHDR, _useLightVolume;
    float _exposure;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
    virtual void processInput(GLFWwindow *window);
public:
    deferred_shading(std::string name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~deferred_shading();
};

/**************************************************** deferred_shading ****************************************************/

deferred_shading::~deferred_shading()
{
    glDeleteFramebuffers(1, &_FBO);
    glDeleteRenderbuffers(1, &_RBO);
    glDeleteTextures(1, &_TEX_POS);
    glDeleteTextures(1, &_TEX_NOR);
    glDeleteTextures(1, &_TEX_COL);
    delete _camera;
    delete _mesh;
    delete _meshScreen;
    delete _meshLight;
    delete _shaderGBuffer;
    delete _shaderDeferredNormal;
    delete _shaderDeferredLightVolume;
    delete _shaderLight;
    _camera = nullptr;
    _mesh = nullptr;
    _meshScreen = nullptr;
    _meshLight = nullptr;
    _shaderGBuffer = nullptr;
    _shaderDeferredNormal = nullptr;
    _shaderDeferredLightVolume = nullptr;
    _shaderLight = nullptr;
}

void deferred_shading::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _shaderGBuffer = new Shader("5_advanced_lighting/9_gBuffer.vs", "5_advanced_lighting/9_gBuffer.fs");
    _shaderDeferredNormal = new Shader("5_advanced_lighting/9_deferred.vs", "5_advanced_lighting/9_deferred.fs");
    _shaderDeferredLightVolume = new Shader("5_advanced_lighting/9_deferred.vs", "5_advanced_lighting/9_deferred_light_volume.fs");
    _shaderLight = new Shader("common/light.vs", "common/light.fs");
    _mesh = new Mesh(cube_vertices_vector, cube_indices_vector, std::vector<Texture*> { Texture::Load("texture_diffuse", "wood.png") });
    _meshScreen = new Mesh(screen_vertices_vector, rectangle_indices_vector, {});
    _meshLight = new Mesh(cube_vertices_vector, cube_indices_vector, {});

    int width = 0, height = 0;
    glfwGetWindowSize(window, &width, &height);

    // 帧缓冲-GBuffer
    glGenFramebuffers(1, &_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    // 颜色缓冲-位置
    glGenTextures(1, &_TEX_POS);
    glBindTexture(GL_TEXTURE_2D, _TEX_POS);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // 不设置过滤取样颜色为0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _TEX_POS, 0);
    // 颜色缓冲-法线
    glGenTextures(1, &_TEX_NOR);
    glBindTexture(GL_TEXTURE_2D, _TEX_NOR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
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
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _RBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer is not complete: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _lightPositions = new glm::vec3[4] {
        glm::vec3(0.0f, 0.0f, -49.5f),
        glm::vec3(-1.4f, -1.9f, -9.0f),
        glm::vec3(0.0f, -1.8f, -4.0f),
        glm::vec3(0.8f, -1.7f, -6.0f)
    };
    _lightColors = new glm::vec3[4] {
        glm::vec3(200.0f, 200.0f, 200.0f),
        glm::vec3(0.1f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.2f),
        glm::vec3(0.0f, 0.1f, 0.0f)
    };

    _enableHDR = true;
    _useLightVolume = false;
    _exposure = 1.0f;
    std::cout << (_enableHDR ? "enable hdr" : "disable hdr") << std::endl;
    std::cout << (_useLightVolume ? "enable light volume" : "disable light volume") << std::endl;
    std::cout << "exposure: " << _exposure << std::endl;
}

void deferred_shading::onUpdate(GLFWwindow *window)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 关闭剔除 显示立方体内部
    glDisable(GL_CULL_FACE);

    // 生成GBuffer信息
    _shaderGBuffer->use();
    glm::mat4 cubeModel = glm::mat4(1.0f);
    cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 0.0f, -25.0f));
    cubeModel = glm::scale(cubeModel, glm::vec3(5.0f, 5.0f, 60.0f));
    _shaderGBuffer->setmat4fv("model", glm::value_ptr(cubeModel));
    _shaderGBuffer->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderGBuffer->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _mesh->draw(_shaderGBuffer);
    
    // 绑定默认缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 使用GBuffer绘制物体并添加光照信息
    // 根据是否使用光体积选择着色器
    Shader *shaderDeferred = _useLightVolume ? _shaderDeferredLightVolume : _shaderDeferredNormal;
    shaderDeferred->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _TEX_POS);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _TEX_NOR);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _TEX_COL);
    shaderDeferred->set1i("texture_position", 0);
    shaderDeferred->set1i("texture_normal", 1);
    shaderDeferred->set1i("texture_color", 2);
    glm::vec3 viewPos = _camera->position();
    shaderDeferred->set3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
    for (int i = 0; i < 4; ++i)
    {
        shaderDeferred->set3f(("lights[" + std::to_string(i) + "].Position").c_str(), _lightPositions[i].x, _lightPositions[i].y, _lightPositions[i].z);
        shaderDeferred->set3f(("lights[" + std::to_string(i) + "].Color").c_str(), _lightColors[i].x, _lightColors[i].y, _lightColors[i].z);
        if (_useLightVolume)
        {
            // 计算光体积半径
            // 解方程 F = 5 / 256 = I / (a + b * d + c * d ^ 2)
            // a: constant
            // b: linear
            // c: quadratic
            // I: 为光源最亮的颜色分量
            // x = (-a + sqrt(a ^ 2 - 4 * b * (c - I * 256 / 5))) / (2 * b)
            float constant = 1.0;
            float linear = 0.7;
            float quadratic = 1.8;
            float I = std::fmaxf(std::fmaxf(_lightColors[i].x, _lightColors[i].y), _lightColors[i].z);
            float radius = (-constant + sqrt(constant * constant - 4 * linear * (quadratic - I * 256.0f / 5.0f))) / (2 * linear);
            shaderDeferred->set1f(("lights[" + std::to_string(i) + "].Radius").c_str(), radius);
        }
    }
    shaderDeferred->set1i("hdr", (int)_enableHDR);
    shaderDeferred->set1f("exposure", _exposure);
    _meshScreen->draw(shaderDeferred);

    // 复制GBuffer深度信息到默认缓冲
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    int width = 0, height = 0;
    glfwGetWindowSize(window, &width, &height);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // 正常绘制光源
    _shaderLight->use();
    _shaderLight->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderLight->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    for (int i = 0; i < 4; ++i)
    {
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, _lightPositions[i]);
        lightModel = glm::scale(lightModel, glm::vec3(0.5f));
        _shaderLight->setmat4fv("model", glm::value_ptr(lightModel));
        _shaderLight->set3f("color", _lightColors[i].x, _lightColors[i].y, _lightColors[i].z);
        _meshLight->draw(_shaderLight);
    }
}

void deferred_shading::processInput(GLFWwindow *window)
{
    if (Input::isKeyDown(window, GLFW_KEY_H))
    {
        _enableHDR = !_enableHDR;
        std::cout << (_enableHDR ? "enable hdr" : "disable hdr") << std::endl;
    }
    if (Input::isKeyDown(window, GLFW_KEY_SPACE))
    {
        _useLightVolume = !_useLightVolume;
        std::cout << (_useLightVolume ? "enable light volume" : "disable light volume") << std::endl;
    }
    if (Input::isKeyDown(window, GLFW_KEY_KP_ADD))
    {
        _exposure += 0.1f;
        std::cout << "exposure: " << _exposure << std::endl;
    }
    if (Input::isKeyDown(window, GLFW_KEY_KP_SUBTRACT))
    {
        _exposure -= 0.1f;
        std::cout << "exposure: " << _exposure << std::endl;
    }

    Window::processInput(window);
}

/**************************************************** test ****************************************************/

int main()
{
    // 延迟着色
    Window *window1 = new deferred_shading("9_deferred_shading", 800, 600);
    window1->start();
    delete window1;

    return 0;
}