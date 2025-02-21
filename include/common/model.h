#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
private:
    std::vector<Mesh*> _meshes;
    std::string _directory;
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh *processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture*> loadMaterialTextures(aiMaterial *material, aiTextureType type, std::string typeName);
    static std::string _MODEL_BASE_PATH;
public:
    Model(std::string path);
    ~Model();
    void draw(Shader *shader);
};

std::string Model::_MODEL_BASE_PATH = "./../../../assets/";

Model::Model(std::string path)
{
    loadModel(path);
}

Model::~Model()
{
    for (int index = 0, len = _meshes.size(); index < len; ++index)
    {
        delete _meshes[index];
    }
    _meshes.clear();
    _meshes.shrink_to_fit();
}

void Model::draw(Shader *shader)
{
    for (int index = 0, len = _meshes.size(); index < len; ++index)
    {
        _meshes[index]->draw(shader);
    }
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    // ai_Process_Triangulate: 将所有多边形转换为三角形
    // ai_Process_FlipUVs: 翻转纹理
    // ai_Process_GenNormals: 如果模型不包含法线的话生成法线
    // ai_Process_SplitLargeMeshes: 拆分大的模型 避免渲染最大顶点属性数量限制
    // ai_Process_OptimizeMeshes: 合并小模型 减少绘制调用
    const aiScene *scene = importer.ReadFile(_MODEL_BASE_PATH + path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ASSIMP Model Load Fail: " << path << std::endl;
        std::cout << importer.GetErrorString() << std::endl;
        return;
    }

    _directory = path.substr(0, path.find_last_of('/') + 1);
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // 处理节点所有的网格(如果有的话)
    for (int index = 0, len = node->mNumMeshes; index < len; ++index)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[index]];
        _meshes.push_back(processMesh(mesh, scene));
    }

    // 递归处理每个子节点
    for (int index = 0, len = node->mNumChildren; index < len; ++index)
    {
        processNode(node->mChildren[index], scene);
    }
}

Mesh *Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    // 1. 处理顶点位置、法线和纹理坐标
    std::vector<Vertex> vertices = std::vector<Vertex>(mesh->mNumVertices);
    for (int index = 0, len = mesh->mNumVertices; index < len; ++index)
    {
        Vertex &vertex = vertices[index];
        // 顶点位置
        aiVector3D position = mesh->mVertices[index];
        vertex.Position = glm::vec3(position.x, position.y, position.z);
        // 顶点法线
        aiVector3D normal = mesh->mNormals[index];
        vertex.Normal = glm::vec3(normal.x, normal.y, normal.z);
        // 纹理坐标
        if (mesh->mTextureCoords[0])
        {
            aiVector3D texCoords = mesh->mTextureCoords[0][index];
            vertex.TexCoords = glm::vec2(texCoords.x, texCoords.y);
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
    }

    // 2. 处理索引
    std::vector<unsigned int> indices = std::vector<unsigned int>(mesh->mNumFaces * 3);
    for (int index = 0, len = mesh->mNumFaces; index < len; ++index)
    {
        aiFace face = mesh->mFaces[index];
        indices[index * 3 + 0] = face.mIndices[0];
        indices[index * 3 + 1] = face.mIndices[1];
        indices[index * 3 + 2] = face.mIndices[2];
    }

    // 3. 处理材质
    std::vector<Texture*> textures = std::vector<Texture*>();
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture*> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture*> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return new Mesh(vertices, indices, textures);
}

std::vector<Texture*> Model::loadMaterialTextures(aiMaterial *material, aiTextureType type, std::string typeName)
{
    std::vector<Texture*> textures = std::vector<Texture*>();
    for (int index = 0, len = material->GetTextureCount(type); index < len; ++index)
    {
        aiString str;
        material->GetTexture(type, index, &str);
        Texture *texture = Texture::Load(typeName, _directory + str.C_Str());
        textures.push_back(texture);
    }
    return textures;
}

#endif