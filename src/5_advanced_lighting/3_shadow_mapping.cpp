#include <common/global.h>
#include <common/window.h>
#include <common/model.h>
#include <common/shader.h>
#include <common/camera.h>

class shadow_mapping : public Window
{
private:
    GLuint _FBO, _TEX;
    Camera *_camera;
    Mesh *_meshFloor;
    Model *_modelRock;
    Shader *_shader, *_shaderShadow;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    shadow_mapping(std::string name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~shadow_mapping();
};

/**************************************************** shadow_mapping ****************************************************/

shadow_mapping::~shadow_mapping()
{
    glDeleteFramebuffers(1, &_FBO);
    glDeleteTextures(1, &_TEX);
    delete _camera;
    delete _meshFloor;
    delete _modelRock;
    delete _shader;
    delete _shaderShadow;
    _camera = nullptr;
    _meshFloor = nullptr;
    _modelRock = nullptr;
    _shader = nullptr;
    _shaderShadow = nullptr;
}

void shadow_mapping::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _camera->setPosition(0.0f, 5.0f, 3.0f);
    _meshFloor = new Mesh(floor_vertices_vector, floor_indices_vector, std::vector<Texture*>{ Texture::Load("texture_diffuse", "wood.png") });
    _modelRock = new Model("rock/rock.obj");
    _shader = new Shader("5_advanced_lighting/3_shadow_mapping.vs", "5_advanced_lighting/3_shadow_mapping.fs");
    _shaderShadow = new Shader("5_advanced_lighting/3_shadow_collect.vs", "5_advanced_lighting/3_shadow_collect.fs");

    // 创建帧缓冲对象
    glGenFramebuffers(1, &_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    // 纹理附件
    glGenTextures(1, &_TEX);
    glBindTexture(GL_TEXTURE_2D, _TEX);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // 将纹理附件附加到帧缓冲对象
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _TEX, 0);
    // 检查帧缓冲对象是否创建成功
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer is not complete!" << std::endl;
    }
    // 恢复默认帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void shadow_mapping::onUpdate(GLFWwindow *window)
{
    glm::vec3 viewPos = _camera->position();
    glm::mat4 rockModel = glm::mat4(1.0f);
    rockModel = glm::translate(rockModel, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 floorModel = glm::mat4(1.0f);
    floorModel = glm::scale(floorModel, glm::vec3(20.0f));
    
    glm::vec3 lightPos = glm::vec3(0.0f, 100.0f, 100.0f);
    glm::vec3 lightTargetPos = glm::vec3(0.0f);
    glm::vec3 lightDir = glm::normalize(lightTargetPos - lightPos);
    glm::mat4 lightView = glm::lookAt(lightPos, lightTargetPos, glm::vec3(0.0f, 1.0f, 0.0f));
    // 平行光使用正交矩阵
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 200.0f);
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    // 以光源的视角渲染场景 将深度信息写入帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glViewport(0, 0, 1024, 1024);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // 使用物体的背面生成阴影信息 解决偏移产生的悬浮感
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    _shaderShadow->use();
    _shaderShadow->setmat4fv("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
    _shaderShadow->setmat4fv("model", glm::value_ptr(floorModel));
    _meshFloor->draw(_shaderShadow);
    _shaderShadow->setmat4fv("model", glm::value_ptr(rockModel));
    _modelRock->draw(_shaderShadow);

    // 使用帧缓冲信息绘制阴影
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    _shader->use();
    int index = std::max(_meshFloor->getTextureCount(), _modelRock->getMaxTextureSize());
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, _TEX);
    _shader->set1i("texture_shadow", index);
    _shader->setmat4fv("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
    // 其他设置
    _shader->set3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
    _shader->set3f("light.direction", lightDir.x, lightDir.y, lightDir.z);
    _shader->set3f("light.ambient", 0.05f, 0.05f, 0.05f);
    _shader->set3f("light.diffuse", 0.3f, 0.3f, 0.3f);
    _shader->set3f("light.specular", 1.0f, 1.0f, 1.0f);
    _shader->set1f("gamma", 2.2f);
    _shader->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shader->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _shader->setmat4fv("model", glm::value_ptr(floorModel));
    _meshFloor->draw(_shader);
    _shader->setmat4fv("model", glm::value_ptr(rockModel));
    _modelRock->draw(_shader);
}

/**************************************************** test ****************************************************/

int main()
{
    // 平行光阴影
    Window *window1 = new shadow_mapping("3_shadow_mapping", 800, 600);
    window1->start();
    delete window1;

    return 0;
}