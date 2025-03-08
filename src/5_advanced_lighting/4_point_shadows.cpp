#include <common/global.h>
#include <common/window.h>
#include <common/mesh.h>
#include <common/shader.h>
#include <common/camera.h>

class point_shadows : public Window
{
private:
    GLuint _FBO, _TEX;
    Camera *_camera;
    PointLight *_pointLight;
    Mesh *_meshBox, *_meshFloor, *_meshLight;
    Shader *_shader, *_shaderLight, *_shaderShadow;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    point_shadows(std::string name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~point_shadows();
};

/**************************************************** point_shadows ****************************************************/

point_shadows::~point_shadows()
{
    glDeleteFramebuffers(1, &_FBO);
    glDeleteTextures(1, &_TEX);
    delete _camera;
    delete _pointLight;
    delete _meshBox;
    delete _meshLight;
    delete _shader;
    delete _shaderLight;
    delete _shaderShadow;
    _camera = nullptr;
    _pointLight = nullptr;
    _meshBox = nullptr;
    _meshFloor = nullptr;
    _meshLight = nullptr;
    _shader = nullptr;
    _shaderLight = nullptr;
    _shaderShadow = nullptr;
}

void point_shadows::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _camera->setPosition(0.0f, 5.0f, 3.0f);
    _pointLight = new PointLight(glm::vec3(0.0f, 5.0f, -20.0f), 1.0f, 0.045f, 0.0075f, glm::vec3(0.05f), glm::vec3(0.3f), glm::vec3(1.0f));
    _meshBox = new Mesh(cube_vertices_vector, cube_indices_vector, std::vector<Texture*>{ Texture::Load("texture_diffuse", "container2.png") });
    _meshFloor = new Mesh(floor_vertices_vector, floor_indices_vector, std::vector<Texture*>{ Texture::Load("texture_diffuse", "wood.png") });
    _meshLight = new Mesh(cube_vertices_vector, cube_indices_vector, std::vector<Texture*>{ });
    _shader = new Shader("5_advanced_lighting/4_point_shadows.vs", "5_advanced_lighting/4_point_shadows.fs");
    _shaderLight = new Shader("common/light.vs", "common/light.fs");
    _shaderShadow = new Shader("5_advanced_lighting/4_point_shadows_collect.vs", "5_advanced_lighting/4_point_shadows_collect.gs", "5_advanced_lighting/4_point_shadows_collect.fs");

    // 创建帧缓冲对象
    glGenFramebuffers(1, &_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    // 纹理附件
    glGenTextures(1, &_TEX);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _TEX);
    for (GLuint i = GL_TEXTURE_CUBE_MAP_POSITIVE_X; i <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; ++i)
    {
        glTexImage2D(i, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // 将纹理附件附加到帧缓冲对象
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _TEX, 0);
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_NONE);
    // 检查帧缓冲对象是否创建成功
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer is not complete!" << std::endl;
    }
    // 恢复默认帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void point_shadows::onUpdate(GLFWwindow *window)
{
    // 根据时间移动点光源位置
    float currentFrame = glfwGetTime();
    glm::mat4 lightModel = glm::mat4(1.0f);
    glm::vec3 lightPos = glm::vec3(10.0f * sin(currentFrame), 10.0f, 10.0f * cos(currentFrame));
    _pointLight->position = lightPos;
    lightModel = glm::translate(lightModel, lightPos);
    lightModel = glm::scale(lightModel, glm::vec3(0.2f));
    glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1024.0f / 1024.0f, 0.0f, 100.0f);
    glm::mat4 lightSpaceMatrix[] = {
        lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    glm::vec3 viewPos = _camera->position();
    glm::mat4 rockModel = glm::mat4(1.0f);
    rockModel = glm::translate(rockModel, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 floorModel = glm::mat4(1.0f);
    floorModel = glm::scale(floorModel, glm::vec3(20.0f));

    // 以光源的视角渲染场景 将深度信息写入帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glViewport(0, 0, 1024, 1024);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // 使用物体的背面生成阴影信息 解决偏移产生的悬浮感
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    _shaderShadow->use();
    _shaderShadow->setmat4fv("lightPos", glm::value_ptr(lightPos));
    for (int i = 0; i < 6; i++) {
        _shaderShadow->setmat4fv(("lightSpaceMatrixs[" + std::to_string(i) + "]").c_str(), glm::value_ptr(lightSpaceMatrix[i]));
    }
    _shaderShadow->set1f("farPlane", 100.0f);
    _shaderShadow->setmat4fv("model", glm::value_ptr(rockModel));
    _meshBox->draw(_shaderShadow);
    _shaderShadow->setmat4fv("model", glm::value_ptr(floorModel));
    _meshFloor->draw(_shaderShadow);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 使用帧缓冲信息绘制阴影
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    _shader->use();
    int index = std::max(_meshFloor->getTextureCount(), _meshBox->getTextureCount());
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _TEX);
    _shader->set1i("texture_cube_shadow", index);
    for (int i = 0; i < 6; i++) {
        _shader->setmat4fv(("lightSpaceMatrixs[" + std::to_string(i) + "]").c_str(), glm::value_ptr(lightSpaceMatrix[i]));
    }
    // 其他设置
    _shader->set1f("gamma", 2.2f);
    _shader->set3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
    _shader->set1f("farPlane", 100.0f);
    _shader->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shader->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _pointLight->draw(_shader);
    // 箱子
    _shader->setmat4fv("model", glm::value_ptr(rockModel));
    _meshBox->draw(_shader);
    // 地板
    _shader->setmat4fv("model", glm::value_ptr(floorModel));
    _meshFloor->draw(_shader);
    // 光源
    _shaderLight->use();
    _shaderLight->setmat4fv("model", glm::value_ptr(lightModel));
    _shaderLight->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderLight->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _shaderLight->set3f("color", _pointLight->specular.x, _pointLight->specular.y, _pointLight->specular.z);
    _meshLight->draw(_shaderLight);
}

/**************************************************** test ****************************************************/

int main()
{
    // Gamma校正
    Window *window1 = new point_shadows("4_point_shadows", 800, 600);
    window1->start();
    delete window1;

    return 0;
}