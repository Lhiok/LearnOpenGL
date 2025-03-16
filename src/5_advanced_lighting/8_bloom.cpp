#include <common/global.h>
#include <common/window.h>
#include <common/mesh.h>
#include <common/shader.h>
#include <common/camera.h>

class bloom : public Window
{
private:
    GLuint _FBO, _FBO_BLUR[2], _RBO, _TEX, _TEX_BLUR[2];
    Camera *_camera;
    Mesh *_meshCube, *_meshLight, *_meshScreen;
    Shader *_shaderCube, *_shaderLight, *_shaderBlur, *_shaderFinal;
    glm::vec3 *_cubePositions, *_cubeScales, *_cubeRotations;
    glm::vec3 *_lightPositions, *_lightScales, *_lightColors;
    bool  _enableHDR, _enableBloom;
    float _exposure;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
    virtual void processInput(GLFWwindow *window);
public:
    bloom(std::string name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~bloom();
};

/**************************************************** bloom ****************************************************/

bloom::~bloom()
{
    glDeleteFramebuffers(1, &_FBO);
    glDeleteRenderbuffers(1, &_RBO);
    glDeleteTextures(1, &_TEX);
    for (int i = 0; i < 2; i++)
    {
        glDeleteFramebuffers(1, &_FBO_BLUR[i]);
        glDeleteTextures(1, &_TEX_BLUR[i]);
    }
    delete _camera;
    delete _meshCube;
    delete _meshLight;
    delete _meshScreen;
    delete _shaderCube;
    delete _shaderLight;
    delete _shaderBlur;
    delete _shaderFinal;
    _camera = nullptr;
    _meshCube = nullptr;
    _meshLight = nullptr;
    _meshScreen = nullptr;
    _shaderCube = nullptr;
    _shaderLight = nullptr;
    _shaderBlur = nullptr;
    _shaderFinal = nullptr;
}

void bloom::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _shaderCube = new Shader("5_advanced_lighting/8_cube.vs", "5_advanced_lighting/8_cube.fs");
    _shaderLight = new Shader("5_advanced_lighting/8_light.vs", "5_advanced_lighting/8_light.fs");
    _shaderBlur = new Shader("5_advanced_lighting/8_blur.vs", "5_advanced_lighting/8_blur.fs");
    _shaderFinal = new Shader("5_advanced_lighting/8_final.vs", "5_advanced_lighting/8_final.fs");
    _meshCube = new Mesh(cube_vertices_vector, cube_indices_vector, { Texture::Load("texture_diffuse", "wood.png") });
    _meshLight = new Mesh(cube_vertices_vector, cube_indices_vector, { });
    _meshScreen = new Mesh(screen_vertices_vector, rectangle_indices_vector, {});

    int width = 0, height = 0;
    glfwGetWindowSize(window, &width, &height);

    // 高斯模糊帧缓冲
    for (int i = 0; i < 2; ++i)
    {
        glGenTextures(1, &_TEX_BLUR[i]);
        glBindTexture(GL_TEXTURE_2D, _TEX_BLUR[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    for (int i = 0; i < 2; ++i)
    {
        glGenFramebuffers(1, &_FBO_BLUR[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, _FBO_BLUR[i]);
        // 帧缓冲[x]使用纹理[x^1]作为输出 使用纹理[x]作为输入
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _TEX_BLUR[i^1], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Framebuffer is not complete!" << std::endl;
        }
    }

    // 帧缓存
    glGenFramebuffers(1, &_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    // 渲染缓冲附件 深度缓冲
    glGenRenderbuffers(1, &_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, _RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _RBO);
    // 普通纹理输出
    glGenTextures(1, &_TEX);
    glBindTexture(GL_TEXTURE_2D, _TEX);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _TEX, 0);
    // 泛光纹理输出
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _TEX_BLUR[0], 0);
    // 设置输出
    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _cubePositions = new glm::vec3[10] {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    _cubeScales = new glm::vec3[10] {
        glm::vec3( 0.5f,  0.5f,  0.5f),
        glm::vec3( 1.3f,  1.3f,  1.3f),
        glm::vec3( 0.8f,  0.8f,  0.8f),
        glm::vec3( 1.3f,  1.3f,  1.3f),
        glm::vec3( 1.0f,  1.0f,  1.0f),
        glm::vec3( 1.5f,  1.5f,  1.5f),
        glm::vec3( 0.5f,  0.5f,  0.5f), 
        glm::vec3( 0.5f,  0.5f,  0.5f),
        glm::vec3( 0.5f,  0.5f,  0.5f),
        glm::vec3( 0.5f,  0.5f,  0.5f)
    };
    _cubeRotations = new glm::vec3[10] {
        glm::vec3(  0.0f,  0.0f,  0.0f),
        glm::vec3( 45.0f, 45.0f, 45.0f),
        glm::vec3( 10.0f, 20.0f, 50.0f),
        glm::vec3( 60.0f, 45.0f, 30.0f),
        glm::vec3( 30.0f, 70.0f, 20.0f),
        glm::vec3( 40.0f, 40.0f, 40.0f),
        glm::vec3( 20.0f, 20.0f, 20.0f),
        glm::vec3( 50.0f, 50.0f, 50.0f),
        glm::vec3( 20.0f, 20.0f, 20.0f),
        glm::vec3( 30.0f, 30.0f, 30.0f)
    };

    _lightPositions = new glm::vec3[4] {
        glm::vec3( 0.0f, 0.5f,  1.5f),
        glm::vec3(-4.0f, 0.5f, -3.0f),
        glm::vec3( 3.0f, 0.5f,  1.0f),
        glm::vec3(-0.8f, 2.4f, -1.0f)
    };
    _lightScales = new glm::vec3[4] {
        glm::vec3( 0.5f, 0.5f, 0.5f),
        glm::vec3( 0.6f, 0.6f, 0.6f),
        glm::vec3( 0.7f, 0.7f, 0.7f),
        glm::vec3( 0.8f, 0.8f, 0.8f)
    };
    _lightColors = new glm::vec3[4] {
        glm::vec3( 5.0f, 5.0f,  5.0f),
        glm::vec3(10.0f, 0.0f,  0.0f),
        glm::vec3( 0.0f, 0.0f, 15.0f),
        glm::vec3( 0.0f, 5.0f,  0.0f)
    };

    _enableHDR = true;
    std::cout << (_enableHDR ? "enable hdr" : "disable hdr") << std::endl;
    _exposure = 1.0f;
    std::cout << "exposure: " << _exposure << std::endl;
    _enableBloom = true;
    std::cout << (_enableBloom ? "enable bloom" : "disable bloom") << std::endl;
}

void bloom::onUpdate(GLFWwindow *window)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    // 绘制Cube
    glm::vec3 viewPos = _camera->position();
    _shaderCube->use();
    _shaderCube->set3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
    _shaderCube->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderCube->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    for (int i = 0; i < 4; ++i)
    {
        _shaderCube->set3f(("lights[" + std::to_string(i) + "].Position").c_str(), _lightPositions[i].x, _lightPositions[i].y, _lightPositions[i].z);
        _shaderCube->set3f(("lights[" + std::to_string(i) + "].Color").c_str(), _lightColors[i].x, _lightColors[i].y, _lightColors[i].z);
    }
    for (int i = 0; i < 10; ++i)
    {
        glm::mat4 cubeModel = glm::mat4(1.0f);
        cubeModel = glm::translate(cubeModel, _cubePositions[i]);
        cubeModel = glm::scale(cubeModel, _cubeScales[i]);
        cubeModel = glm::rotate(cubeModel, glm::radians(_cubeRotations[i].x), glm::vec3(1.0f, 0.0f, 0.0f));
        cubeModel = glm::rotate(cubeModel, glm::radians(_cubeRotations[i].y), glm::vec3(0.0f, 1.0f, 0.0f));
        cubeModel = glm::rotate(cubeModel, glm::radians(_cubeRotations[i].z), glm::vec3(0.0f, 0.0f, 1.0f));
        _shaderCube->setmat4fv("model", glm::value_ptr(cubeModel));
        _meshCube->draw(_shaderCube);
    }
    // 绘制light
    _shaderLight->use();
    _shaderLight->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderLight->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    for (int i = 0; i < 4; ++i)
    {
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, _lightPositions[i]);
        lightModel = glm::scale(lightModel, _lightScales[i]);
        _shaderLight->setmat4fv("model", glm::value_ptr(lightModel));
        _shaderLight->set3f("color", _lightColors[i].x, _lightColors[i].y, _lightColors[i].z);
        _meshLight->draw(_shaderLight);
    }
    
    // 高斯模糊
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    _shaderBlur->use();
    _shaderBlur->set1i("texture_diffuse0", 0);
    // 上下各进行5次
    for (int i = 0; i < 10; ++i)
    {
        int idx = i & 1;
        glBindFramebuffer(GL_FRAMEBUFFER, _FBO_BLUR[idx]);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, _TEX_BLUR[idx]);
        _shaderBlur->set1i("horizontal", idx);
        _meshScreen->draw(_shaderBlur);
    }
    
    // 最终颜色输出
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    _shaderFinal->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _TEX);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _TEX_BLUR[0]);
    _shaderFinal->set1i("texture_diffuse0", 0);
    _shaderFinal->set1i("texture_bloom", 1);
    _shaderFinal->set1i("hdr", (int)_enableHDR);
    _shaderFinal->set1f("exposure", _exposure);
    _shaderFinal->set1i("bloom", (int)_enableBloom);
    _meshScreen->draw(_shaderFinal);
}

void bloom::processInput(GLFWwindow *window)
{
    if (Input::isKeyDown(window, GLFW_KEY_H))
    {
        _enableHDR = !_enableHDR;
        std::cout << (_enableHDR ? "enable hdr" : "disable hdr") << std::endl;
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
    if (Input::isKeyDown(window, GLFW_KEY_B))
    {
        _enableBloom = !_enableBloom;
        std::cout << (_enableBloom ? "enable bloom" : "disable bloom") << std::endl;
    }

    Window::processInput(window);
}

/**************************************************** test ****************************************************/

int main()
{
    // 泛光
    Window *window1 = new bloom("8_bloom", 800, 600);
    window1->start();
    delete window1;

    return 0;
}