#include <common/window.h>
#include <common/global.h>
#include <common/model.h>
#include <common/shader.h>
#include <common/light.h>

#include "global.cpp"

class discard : public Window
{
private:
    Camera *_camera;
    Mesh *_meshGrass;
    Shader *_shaderGrass;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    discard(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~discard();
};

class blending : public Window
{
private:
    Camera *_camera;
    Mesh *_meshCube, *_meshGlass;
    Shader *_shaderCube, *_shaderGlass;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    blending(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~blending();
};

class blending_sort : public Window
{
private:
    Camera *_camera;
    Mesh *_meshCube, *_meshGlass;
    Shader *_shaderCube, *_shaderGlass;
    std::vector<std::pair<int, float>> _sortVector;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    blending_sort(const GLchar *name, GLuint width, GLuint height) : Window(name, width, height) { }
    ~blending_sort();
};

/**************************************************** discard ****************************************************/

discard::~discard()
{
    delete _camera;
    delete _meshGrass;
    delete _shaderGrass;
    _camera = nullptr;
    _meshGrass = nullptr;
    _shaderGrass = nullptr;
}

void discard::onInit(GLFWwindow *window)
{
    // Camera
    Camera::SetMainCamera(_camera = new Camera(window));
    // Mesh
    Texture *textureGrass = Texture::Load("texture_diffuse", "grass.png");
    _meshGrass = new Mesh(rectangle_vertices_vector, rectangle_indices_vector, std::vector<Texture*> { textureGrass });
    // Shader
    _shaderGrass = new Shader("4_advanced/2_stencil_testing.vs", "4_advanced/3_discard.fs");
}

void discard::onUpdate(GLFWwindow *window)
{
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

/**************************************************** blending ****************************************************/

blending::~blending()
{
    delete _camera;
    delete _meshCube;
    delete _meshGlass;
    delete _shaderCube;
    delete _shaderGlass;
    _camera = nullptr;
    _meshCube = nullptr;
    _meshGlass = nullptr;
    _shaderCube = nullptr;
    _shaderGlass = nullptr;
}

void blending::onInit(GLFWwindow *window)
{
    // Camera
    Camera::SetMainCamera(_camera = new Camera(window));
    // Cube
    _meshCube = new Mesh(cube_vertices_vector, cube_indices_vector, std::vector<Texture*> { Texture::Load("texture_diffuse", "container2.png") });
    _shaderCube = new Shader("4_advanced/2_stencil_testing.vs", "4_advanced/2_stencil_testing.fs");
    // Glass
    _meshGlass = new Mesh(rectangle_vertices_vector, rectangle_indices_vector, std::vector<Texture*> { Texture::Load("texture_diffuse", "blending_transparent_window.png") });
    _shaderGlass = new Shader("4_advanced/2_stencil_testing.vs", "4_advanced/2_stencil_testing.fs");
}

void blending::onUpdate(GLFWwindow *window)
{
    _shaderCube->use();
    _shaderCube->setmat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    _shaderCube->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderCube->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _meshCube->draw(_shaderCube);

    // 开启混合 result = 混合方程(sourceFactor * sourceColor, destinationFactor * destinationColor)
    // sourceFactor: 源因子
    // destinationFactor: 目标因子
    // sourceColor: 源颜色
    // destinationColor: 目标颜色 颜色缓冲区中的颜色
    glEnable(GL_BLEND);

    // 设置常量颜色
    glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);

    // 设置源因子和目标因子
    // GL_ZERO: 0
    // GL_ONE: 1
    // GL_SRC_COLOR: 源颜色向量
    // GL_SRC_ALPHA: 源颜色alpha值
    // GL_DST_COLOR: 目标颜色向量
    // GL_DST_ALPHA: 目标颜色alpha值
    // GL_CONSTANT_COLOR: 常量颜色向量
    // GL_CONSTANT_ALPHA: 常量颜色alpha值
    // GL_ONE_MINUS_XXX: 1-XXX
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 使用glBlendFuncSeparate可为RGB和Alpha设置不同的选项
    // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    // 设置混合方程
    // GL_FUNC_ADD: Src + Dst
    // GL_FUNC_SUBTRACT: Src - Dst
    // GL_FUNC_REVERSE_SUBTRACT: Dst - Src
    // GL_MIN: min(Src, Dst)
    // GL_MAX: max(Src, Dst)
    glBlendEquation(GL_FUNC_ADD);

    _shaderGlass->use();
    _shaderGlass->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderGlass->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    // 由于深度测试的原因 先绘制的玻璃会挡住后绘制且在其后方的玻璃
    for (int index = 0, len = glass_position_vector.size(); index < len; ++index)
    {
        glm::mat4 modelGlass = glm::mat4(1.0f);
        modelGlass = glm::translate(modelGlass, glass_position_vector[index]);
        _shaderGlass->setmat4fv("model", glm::value_ptr(modelGlass));
        _meshGlass->draw(_shaderGlass);
    }

    glDisable(GL_BLEND);
}

/**************************************************** blending_sort ****************************************************/

blending_sort::~blending_sort()
{
    delete _camera;
    delete _meshCube;
    delete _meshGlass;
    delete _shaderCube;
    delete _shaderGlass;
    _camera = nullptr;
    _meshCube = nullptr;
    _meshGlass = nullptr;
    _shaderCube = nullptr;
    _shaderGlass = nullptr;
    _sortVector.clear();
    _sortVector.shrink_to_fit();
}

void blending_sort::onInit(GLFWwindow *window)
{
    // Camera
    Camera::SetMainCamera(_camera = new Camera(window));
    // Cube
    _meshCube = new Mesh(cube_vertices_vector, cube_indices_vector, std::vector<Texture*> { Texture::Load("texture_diffuse", "container2.png") });
    _shaderCube = new Shader("4_advanced/2_stencil_testing.vs", "4_advanced/2_stencil_testing.fs");
    // Glass
    _meshGlass = new Mesh(rectangle_vertices_vector, rectangle_indices_vector, std::vector<Texture*> { Texture::Load("texture_diffuse", "blending_transparent_window.png") });
    _shaderGlass = new Shader("4_advanced/2_stencil_testing.vs", "4_advanced/2_stencil_testing.fs");
}

void blending_sort::onUpdate(GLFWwindow *window)
{
    // 绘制不透明物体
    _shaderCube->use();
    _shaderCube->setmat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    _shaderCube->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderCube->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    _meshCube->draw(_shaderCube);

    // 设置混合参数
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    // 对透明物体进行排序
    _sortVector.clear();
    glm::vec3 cameraPos = _camera->position();
    for (int index = 0, len = glass_position_vector.size(); index < len; ++index)
    {
        _sortVector.push_back({ index, glm::length(cameraPos - glass_position_vector[index]) });
    }
    // 简单按离相机距离从大到小排序 实际还需考虑旋转缩放等各种复杂因素
    std::sort(_sortVector.begin(), _sortVector.end(), [](const std::pair<int, float> &a, const std::pair<int, float> &b) { return a.second > b.second; });

    // 绘制透明物体
    _shaderGlass->use();
    _shaderGlass->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shaderGlass->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    for (int index = 0, len = _sortVector.size(); index < len; ++index)
    {
        glm::mat4 modelGlass = glm::mat4(1.0f);
        glm::vec3 glassPosition = glass_position_vector[_sortVector[index].first];
        modelGlass = glm::translate(modelGlass, glassPosition);
        _shaderGlass->setmat4fv("model", glm::value_ptr(modelGlass));
        _meshGlass->draw(_shaderGlass);
    }

    glDisable(GL_BLEND);
}


/**************************************************** test ****************************************************/

int main()
{
    Window *window1 = new discard("3_discard", 800, 600);
    window1->start();
    delete window1;

    Window *window2 = new blending("3_blending", 800, 600);
    window2->start();
    delete window2;

    Window *window3 = new blending_sort("3_blending_sort", 800, 600);
    window3->start();
    delete window3;

    return 0;
}