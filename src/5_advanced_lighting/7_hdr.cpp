#include <common/global.h>
#include <common/window.h>
#include <common/mesh.h>
#include <common/shader.h>
#include <common/camera.h>

class high_dynamic_range : public Window
{
private:
    GLuint _FBO, _TEX;
    Camera *_camera;
    Mesh *_mesh, *_meshHDR;
    Shader *_shader, *_shaderHDR;
    glm::vec3 *_lightPositions, *_lightColors;
    bool _enableHDR;
    float _exposure;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
    virtual void processInput(GLFWwindow *window);
public:
    high_dynamic_range(std::string name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~high_dynamic_range();
};

/**************************************************** high_dynamic_range ****************************************************/

high_dynamic_range::~high_dynamic_range()
{
    glDeleteFramebuffers(1, &_FBO);
    glDeleteTextures(1, &_TEX);
    delete _camera;
    delete _mesh;
    delete _meshHDR;
    delete _shader;
    delete _shaderHDR;
    _camera = nullptr;
    _mesh = nullptr;
    _meshHDR = nullptr;
    _shader = nullptr;
    _shaderHDR = nullptr;
}

void high_dynamic_range::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _shader = new Shader("5_advanced_lighting/7_lighting.vs", "5_advanced_lighting/7_lighting.fs");
    _shaderHDR = new Shader("5_advanced_lighting/7_hdr_reinhard.vs", "5_advanced_lighting/7_hdr_reinhard.fs");
    _mesh = new Mesh(cube_vertices_vector, cube_indices_vector, std::vector<Texture*> { Texture::Load("texture_diffuse", "wood.png") });
    _meshHDR = new Mesh(screen_vertices_vector, rectangle_indices_vector, {});

    int width = 0, height = 0;
    glfwGetWindowSize(window, &width, &height);

    glGenFramebuffers(1, &_FBO);
    glGenTextures(1, &_TEX);
    glBindTexture(GL_TEXTURE_2D, _TEX);
    // 浮点帧缓存: 设定格式为GL_RGB16F/GL_RGB32F等 可以存储超过1.0f的颜色
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _TEX, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer is not complete!" << std::endl;
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
    _exposure = 1.0f;
    std::cout << (_enableHDR ? "enable hdr" : "disable hdr") << std::endl;
    std::cout << "exposure: " << _exposure << std::endl;
}

void high_dynamic_range::onUpdate(GLFWwindow *window)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 剔除正面 仅显示立方体内部
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glm::vec3 viewPos = _camera->position();
    _shader->use();
    _shader->set3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
    _shader->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shader->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    for (int i = 0; i < 4; ++i)
    {
        _shader->set3f(("lights[" + std::to_string(i) + "].Position").c_str(), _lightPositions[i].x, _lightPositions[i].y, _lightPositions[i].z);
        _shader->set3f(("lights[" + std::to_string(i) + "].Color").c_str(), _lightColors[i].x, _lightColors[i].y, _lightColors[i].z);
    }
    glm::mat4 cubeModel = glm::mat4(1.0f);
    cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 0.0f, -25.0f));
    cubeModel = glm::scale(cubeModel, glm::vec3(5.0f, 5.0f, 60.0f));
    _shader->setmat4fv("model", glm::value_ptr(cubeModel));
    _mesh->draw(_shader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_CULL_FACE);

    _shaderHDR->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _TEX);
    _shaderHDR->set1i("texture_diffuse0", 0);
    _shaderHDR->set1i("hdr", (int)_enableHDR);
    _shaderHDR->set1f("exposure", _exposure);
    _meshHDR->draw(_shaderHDR);
}

void high_dynamic_range::processInput(GLFWwindow *window)
{
    if (Input::isKeyDown(window, GLFW_KEY_SPACE))
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

    Window::processInput(window);
}

/**************************************************** test ****************************************************/

int main()
{
    // hdr
    Window *window1 = new high_dynamic_range("7_high_dynamic_range", 800, 600);
    window1->start();
    delete window1;

    return 0;
}