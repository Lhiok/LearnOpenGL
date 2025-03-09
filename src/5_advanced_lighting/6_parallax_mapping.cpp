#include <common/global.h>
#include <common/window.h>
#include <common/mesh.h>
#include <common/shader.h>
#include <common/camera.h>

class parallax_mapping : public Window
{
private:
    Camera *_camera;
    Mesh *_mesh, *_mesh2;
    Shader *_shader;
    std::string _vertexPath, _fragmentPath;
protected:
    virtual void onInit(GLFWwindow *window);
    virtual void onUpdate(GLFWwindow *window);
public:
    parallax_mapping(std::string name, GLuint width, GLuint height, std::string vertexPath, std::string fragmentPath) : Window(name, width, height) { _vertexPath = vertexPath; _fragmentPath = fragmentPath; }
    ~parallax_mapping();
};

/**************************************************** parallax_mapping ****************************************************/

parallax_mapping::~parallax_mapping()
{
    delete _camera;
    delete _mesh;
    delete _mesh2;
    delete _shader;
    _camera = nullptr;
    _mesh = nullptr;
    _mesh2 = nullptr;
    _shader = nullptr;
}

void parallax_mapping::onInit(GLFWwindow *window)
{
    _camera = new Camera(window);
    _shader = new Shader(_vertexPath, _fragmentPath);

    // 计算切线信息
    for (int index = 0, len = floor_vertices_vector.size(); index < len; ++index)
    {
        floor_vertices_vector[index].Tangent = glm::vec3(0.0f);
        floor_vertices_vector[index].Bitangent = glm::vec3(0.0f);
    }
    for (int index = 0, len = floor_indices_vector.size(); index < len; index += 3)
    {
        int i0 = floor_indices_vector[index], i1 = floor_indices_vector[index + 1], i2 = floor_indices_vector[index + 2];
        glm::vec3 pos0 = floor_vertices_vector[i0].Position, pos1 = floor_vertices_vector[i1].Position, pos2 = floor_vertices_vector[i2].Position;
        glm::vec2 uv0 = floor_vertices_vector[i0].TexCoords, uv1 = floor_vertices_vector[i1].TexCoords, uv2 = floor_vertices_vector[i2].TexCoords;
        glm::vec2 deltaUV1 = uv1 - uv0, deltaUV2 = uv2 - uv0;
        glm::vec3 deltaPos1 = pos1 - pos0, deltaPos2 = pos2 - pos0;

        GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * f;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * f;
        floor_vertices_vector[i0].Tangent += tangent;
        floor_vertices_vector[i1].Tangent += tangent;
        floor_vertices_vector[i2].Tangent += tangent;
        floor_vertices_vector[i0].Bitangent += bitangent;
        floor_vertices_vector[i1].Bitangent += bitangent;
        floor_vertices_vector[i2].Bitangent += bitangent;
    }
    for (int index = 0, len = floor_vertices_vector.size(); index < len; ++index)
    {
        floor_vertices_vector[index].Tangent = glm::normalize(floor_vertices_vector[index].Tangent);
        floor_vertices_vector[index].Bitangent = glm::normalize(floor_vertices_vector[index].Bitangent);
    }
    
    _mesh = new Mesh(floor_vertices_vector, floor_indices_vector, std::vector<Texture*>{
        Texture::Load("texture_diffuse", "wood.png"),
        Texture::Load("texture_normal", "toy_box_normal.png"),
        Texture::Load("texture_height", "toy_box_disp.png")
    });
    _mesh2 = new Mesh(floor_vertices_vector, floor_indices_vector, std::vector<Texture*>{
        Texture::Load("texture_diffuse", "brickwall.jpg"),
        Texture::Load("texture_normal", "brickwall_normal.jpg"),
        Texture::Load("texture_height", "bricks2_disp.jpg")
    });
}

void parallax_mapping::onUpdate(GLFWwindow *window)
{
    glm::vec3 viewPos = _camera->position();

    _shader->use();
    _shader->set1f("gamma", 2.2f);
    _shader->set1f("heightScale", 0.1f);
    _shader->set3f("viewPos", viewPos.x, viewPos.y, viewPos.z);
    _shader->setmat4fv("view", glm::value_ptr(_camera->view()));
    _shader->setmat4fv("projection", glm::value_ptr(_camera->projection()));
    light_directional->draw(_shader);

    glm::mat4 wallModel = glm::mat4(1.0f);
    wallModel = glm::translate(wallModel, glm::vec3(0.0f, 0.0f, -5.0f));
    wallModel = glm::rotate(wallModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    wallModel = glm::scale(wallModel, glm::vec3(4.0f));
    _shader->setmat4fv("model", glm::value_ptr(wallModel));
    _mesh->draw(_shader);

    wallModel = glm::mat4(1.0f);
    wallModel = glm::translate(wallModel, glm::vec3(10.0f, 0.0f, -5.0f));
    wallModel = glm::rotate(wallModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    wallModel = glm::scale(wallModel, glm::vec3(4.0f));
    _shader->setmat4fv("model", glm::value_ptr(wallModel));
    _mesh2->draw(_shader);
}

/**************************************************** test ****************************************************/

int main()
{
    // 法线贴图
    Window *window1 = new parallax_mapping("6_normal_mapping", 800, 600, "5_advanced_lighting/6_parallax_mapping.vs", "5_advanced_lighting/6_parallax_mapping_1.fs");
    window1->start();
    delete window1;

    // 视差贴图
    Window *window2 = new parallax_mapping("6_parallax_mapping", 800, 600, "5_advanced_lighting/6_parallax_mapping.vs", "5_advanced_lighting/6_parallax_mapping_2.fs");
    window2->start();
    delete window2;

    // 陡峭视差映射
    Window *window3 = new parallax_mapping("6_steep_parallax_mapping", 800, 600, "5_advanced_lighting/6_parallax_mapping.vs", "5_advanced_lighting/6_parallax_mapping_3.fs");
    window3->start();
    delete window3;

    // 视差遮蔽映射
    Window *window4 = new parallax_mapping("6_parallax_occlusion_mapping", 800, 600, "5_advanced_lighting/6_parallax_mapping.vs", "5_advanced_lighting/6_parallax_mapping_4.fs");
    window4->start();
    delete window4;

    return 0;
}