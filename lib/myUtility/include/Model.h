/**
 * @file Model.h
 * @brief 載入模型
 */
#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


/**
 * @brief 透過Assimp載入模型
 * @details
 * # 提供的attribute
 * 和 Mesh 一樣，因為 Model 在繪製時會依序對模型的每個 Mesh 呼叫 Mesh::draw
 */
class Model
{
public:
    /**
     * @brief 建構子
     * @param path - 模型的路徑
     * @throws std::runtime_error - 若載入模型失敗
     */
    Model(const char* path)
    {
        loadModel(path);
    }

    /// 對模型包含的每個Mesh呼叫 Mesh::draw
    void draw();
private:
    std::vector<Mesh> m_meshes; ///< 每個Mesh
    std::string m_directory;    ///< obj所在目錄（以"/"結尾），從這載入texture

    /// 從特定路徑載入模型
    void loadModel(const char* path);

    /// 遞迴的處理scene中的每個節點
    void processNode(aiNode *node, const aiScene *scene);

    /// 處理Mesh
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    /// 載入Texture
    std::vector<Mesh::Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type);
};
#endif // MODEL_H
