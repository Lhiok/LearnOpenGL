#include <common/window.h>
#include <common/global.h>
#include <common/model.h>
#include <common/texture_cube.h>
#include <common/shader.h>
#include <common/light.h>

#include "global.cpp"

class cubemaps_skybox : public Window
{
private:
    GLuint _vaoSkybox, _vboSkybox;
    Camera *_camera;
    Mesh *_meshCube, *_meshGrass;
    TextureCube *_textureCubeSkybox;
    Shader *_shaderCube, *_shaderGrass, *_shaderSkybox;
    void renderScene();
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    cubemaps_skybox(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~cubemaps_skybox();
};

class cubemaps_reflect_refract : public Window
{
private:
    GLuint _vaoSkybox, _vboSkybox;
    Camera *_camera;
    Mesh *_meshCube;
    TextureCube *_textureCubeSkybox;
    Shader *_shaderCube, *_shaderSkybox;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    cubemaps_reflect_refract(const GLchar *name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height) { _vertexPath = vertexPath; _fragmentPath = fragmentPath; }
    ~cubemaps_reflect_refract();
};

class cubemaps_texture_reflect : public Window
{
private:
    GLuint _vaoSkybox, _vboSkybox;
    Camera *_camera;
    Model *_modelRobot;
    TextureCube *_textureCubeSkybox;
    Shader *_shaderRobot, *_shaderSkybox;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    cubemaps_texture_reflect(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~cubemaps_texture_reflect();
};

/**************************************************** cubemaps_skybox ****************************************************/

cubemaps_skybox::~cubemaps_skybox()
{
    glDeleteVertexArrays(1, &_vaoSkybox);
    glDeleteBuffers(1, &_vboSkybox);
    delete _camera;
    delete _meshCube;
    delete _meshGrass;
    delete _textureCubeSkybox;
    delete _shaderCube;
    delete _shaderGrass;
    delete _shaderSkybox;
    _camera = nullptr;
    _meshCube = nullptr;
    _meshGrass = nullptr;
    _textureCubeSkybox = nullptr;
    _shaderCube = nullptr;
    _shaderGrass = nullptr;
    _shaderSkybox = nullptr;
}

void cubemaps_skybox::renderScene()
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

void cubemaps_skybox::onInit(GLFWwindow *window)
{
    Camera::SetMainCamera(_camera = new Camera(window));
    _meshCube = new Mesh(cube_vertices_vector, cube_indices_vector, std::vector<Texture*> { Texture::Load("texture_diffuse", "container2.png") });
    _shaderCube = new Shader("4_advanced/2_stencil_testing.vs", "4_advanced/2_stencil_testing.fs");
    _meshGrass = new Mesh(rectangle_vertices_vector, rectangle_indices_vector, std::vector<Texture*> { Texture::Load("texture_diffuse", "grass.png") });
    _shaderGrass = new Shader("4_advanced/2_stencil_testing.vs", "4_advanced/3_discard.fs");
    
    _textureCubeSkybox = new TextureCube(skybox_resource_vector);
    _shaderSkybox = new Shader("common/skybox.vs", "common/skybox.fs");

    // 创建VAO
    glGenVertexArrays(1, &_vaoSkybox);
    glBindVertexArray(_vaoSkybox);
    // 创建VBO
    glGenBuffers(1, &_vboSkybox);
    glBindBuffer(GL_ARRAY_BUFFER, _vboSkybox);
    glBufferData(GL_ARRAY_BUFFER, skybox_vertices_vector.size() * sizeof(float), &skybox_vertices_vector[0], GL_STATIC_DRAW);
    // 设置顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 解绑
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void cubemaps_skybox::onUpdate(GLFWwindow *window)
{
    // 绘制场景
    renderScene();

    // 设置深度测试方法为小于等于
    glDepthFunc(GL_LEQUAL);

    // 绘制天空盒
    _shaderSkybox->use();
    // 天空盒不需要model矩阵
    // 去除相机位移对天空影响 仅保留旋转、缩放
    glm::mat4 view = _camera->view();
    view = glm::mat4(glm::mat3(view));
    _shaderSkybox->setmat4fv("view", glm::value_ptr(view));
    _shaderSkybox->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureCubeSkybox->getTexID());
    _shaderSkybox->set1i("skybox", 0);
    // 绑定VAO
    glBindVertexArray(_vaoSkybox);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
}

/**************************************************** cubemaps_reflect_refract ****************************************************/

cubemaps_reflect_refract::~cubemaps_reflect_refract()
{
    glDeleteVertexArrays(1, &_vaoSkybox);
    glDeleteBuffers(1, &_vboSkybox);
    delete _camera;
    delete _meshCube;
    delete _textureCubeSkybox;
    delete _shaderCube;
    delete _shaderSkybox;
    _camera = nullptr;
    _meshCube = nullptr;
    _textureCubeSkybox = nullptr;
    _shaderCube = nullptr;
    _shaderSkybox = nullptr;
}

void cubemaps_reflect_refract::onInit(GLFWwindow *window)
{
    Camera::SetMainCamera(_camera = new Camera(window));
    _meshCube = new Mesh(cube_vertices_vector, cube_indices_vector, std::vector<Texture*> { });
    _shaderCube = new Shader(_vertexPath, _fragmentPath);
    
    _textureCubeSkybox = new TextureCube(skybox_resource_vector);
    _shaderSkybox = new Shader("common/skybox.vs", "common/skybox.fs");

    // 创建VAO
    glGenVertexArrays(1, &_vaoSkybox);
    glBindVertexArray(_vaoSkybox);
    // 创建VBO
    glGenBuffers(1, &_vboSkybox);
    glBindBuffer(GL_ARRAY_BUFFER, _vboSkybox);
    glBufferData(GL_ARRAY_BUFFER, skybox_vertices_vector.size() * sizeof(float), &skybox_vertices_vector[0], GL_STATIC_DRAW);
    // 设置顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 解绑
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void cubemaps_reflect_refract::onUpdate(GLFWwindow *window)
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // 绘制场景
    _shaderCube->use();
    _shaderCube->setmat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    _shaderCube->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderCube->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    glm::vec3 viewPos = _camera->position();
    _shaderCube->set3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureCubeSkybox->getTexID());
    _shaderCube->set1i("skybox", 0);
    _meshCube->draw(_shaderCube);

    // 设置深度测试方法为小于等于
    glDepthFunc(GL_LEQUAL);

    // 绘制天空盒
    _shaderSkybox->use();
    // 天空盒不需要model矩阵
    // 去除相机位移对天空影响 仅保留旋转、缩放
    glm::mat4 view = _camera->view();
    view = glm::mat4(glm::mat3(view));
    _shaderSkybox->setmat4fv("view", glm::value_ptr(view));
    _shaderSkybox->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureCubeSkybox->getTexID());
    _shaderSkybox->set1i("skybox", 0);
    // 绑定VAO
    glBindVertexArray(_vaoSkybox);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
}

/**************************************************** cubemaps_texture_reflect ****************************************************/

cubemaps_texture_reflect::~cubemaps_texture_reflect()
{
    glDeleteVertexArrays(1, &_vaoSkybox);
    glDeleteBuffers(1, &_vboSkybox);
    delete _camera;
    delete _modelRobot;
    delete _textureCubeSkybox;
    delete _shaderRobot;
    delete _shaderSkybox;
    _camera = nullptr;
    _modelRobot = nullptr;
    _textureCubeSkybox = nullptr;
    _shaderRobot = nullptr;
    _shaderSkybox = nullptr;
}

void cubemaps_texture_reflect::onInit(GLFWwindow *window)
{
    Camera::SetMainCamera(_camera = new Camera(window));
    _modelRobot = new Model("nanosuit_reflection/nanosuit.obj");
    _shaderRobot = new Shader("4_advanced/6_cubemaps_robot.vs", "4_advanced/6_cubemaps_robot.fs");
    
    _textureCubeSkybox = new TextureCube(skybox_resource_vector);
    _shaderSkybox = new Shader("common/skybox.vs", "common/skybox.fs");

    // 创建VAO
    glGenVertexArrays(1, &_vaoSkybox);
    glBindVertexArray(_vaoSkybox);
    // 创建VBO
    glGenBuffers(1, &_vboSkybox);
    glBindBuffer(GL_ARRAY_BUFFER, _vboSkybox);
    glBufferData(GL_ARRAY_BUFFER, skybox_vertices_vector.size() * sizeof(float), &skybox_vertices_vector[0], GL_STATIC_DRAW);
    // 设置顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 解绑
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void cubemaps_texture_reflect::onUpdate(GLFWwindow *window)
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // 绘制场景
    _shaderRobot->use();
    _shaderRobot->setmat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    _shaderRobot->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderRobot->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    glm::vec3 viewPos = _camera->position();
    _shaderRobot->set3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
    // 绑定纹理
    int textureIndex = _modelRobot->getMaxTextureSize();
    glActiveTexture(GL_TEXTURE0 + textureIndex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureCubeSkybox->getTexID());
    _shaderRobot->set1i("skybox", textureIndex);
    _modelRobot->draw(_shaderRobot);

    // 设置深度测试方法为小于等于
    glDepthFunc(GL_LEQUAL);

    // 绘制天空盒
    _shaderSkybox->use();
    // 天空盒不需要model矩阵
    // 去除相机位移对天空影响 仅保留旋转、缩放
    glm::mat4 view = _camera->view();
    view = glm::mat4(glm::mat3(view));
    _shaderSkybox->setmat4fv("view", glm::value_ptr(view));
    _shaderSkybox->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureCubeSkybox->getTexID());
    _shaderSkybox->set1i("skybox", 0);
    // 绑定VAO
    glBindVertexArray(_vaoSkybox);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
}

/**************************************************** test ****************************************************/

int main()
{
    // 天空盒
    // Window *window1 = new cubemaps_skybox("6_cubemaps_skybox", 800, 600);
    // window1->start();
    // delete window1;

    // 反射
    Window *window2 = new cubemaps_reflect_refract("6_cubemaps_reflect", 800, 600, "4_advanced/6_cubemaps_cube.vs", "4_advanced/6_cubemaps_reflect.fs");
    window2->start();
    delete window2;

    // 折射
    Window *window3 = new cubemaps_reflect_refract("6_cubemaps_refract", 800, 600, "4_advanced/6_cubemaps_cube.vs", "4_advanced/6_cubemaps_refract.fs");
    window3->start();
    delete window3;

    // 动态环境贴图
    // 为物体6个角度创建出场景纹理并存储到立方体贴图中
    // 开销大 可预处理生成

    // 反射贴图
    Window *window4 = new cubemaps_texture_reflect("6_cubemaps_texture_reflect", 800, 600);
    window4->start();
    delete window4;

    return 0;
}