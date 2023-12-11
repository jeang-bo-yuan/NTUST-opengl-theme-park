
#include "Model.h"

#include <stdexcept>

void Model::draw()
{
    for (size_t i = 0; i < m_meshes.size(); ++i) {
        m_meshes[i].draw();
    }
}

void Model::loadModel(const char* path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error(std::string("ERROR::ASSIMP::").append(importer.GetErrorString()));
    }

    std::string File(path);
    size_t where_is_slash = File.find_last_of('/');
    if (where_is_slash == std::string::npos)
        m_directory = "./";
    else
        m_directory = File.substr(0, where_is_slash + 1);

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.emplace_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Mesh::Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Mesh::Texture> diffuse;
    std::vector<Mesh::Texture> specular;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Mesh::Vertex vertex;
        // process vertex positions, normals and texture coordinates
        vertex.aPosition.x = mesh->mVertices[i].x;
        vertex.aPosition.y = mesh->mVertices[i].y;
        vertex.aPosition.z = mesh->mVertices[i].z;

        vertex.aNormal.x = mesh->mNormals[i].x;
        vertex.aNormal.y = mesh->mNormals[i].y;
        vertex.aNormal.z = mesh->mNormals[i].z;

        // Assimp allows a model to have up to 8 different texture coordinates per vertex.
        // We're not going to use 8, we only care about the first set of texture coordinates.
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            vertex.aTexcoord.x = mesh->mTextureCoords[0][i].x;
            vertex.aTexcoord.y = mesh->mTextureCoords[0][i].y;
        }
        else
            vertex.aTexcoord = glm::vec2(0.0f, 0.0f);

        vertices.emplace_back(vertex);
    }

    // process indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process material
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        diffuse = loadMaterialTextures(material, aiTextureType_DIFFUSE);
        specular = loadMaterialTextures(material, aiTextureType_SPECULAR);
    }

    return Mesh(vertices, indices, std::move(diffuse), std::move(specular));
}

std::vector<Mesh::Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type)
{
    std::vector<Mesh::Texture> textures;

    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString texture_file;
        mat->GetTexture(type, i, &texture_file);

        textures.emplace_back(m_directory.append(texture_file.C_Str()).c_str());
    }
    return textures;
}
