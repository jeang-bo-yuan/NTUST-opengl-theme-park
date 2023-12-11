/**
 * @file Mesh.h
 * @brief Mesh Object
 */
#include <glad/gl.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

#include "qtTextureImage2D.h"

/// 用來表示模型中的一部分
class Mesh {
public:
    /// 頂點資訊
    struct Vertex {
        glm::vec3 aPosition;
        glm::vec3 aNormal;
        glm::vec2 aTexcoord;
    };

    /// 材質
    typedef qtTextureImage2D Texture;

public:
    /**
     * @brief 建構子
     * @param vertices - 頂點座標
     * @param indices - 繪製順序（三角型）
     * @param diffuse_textures - diffuse貼圖
     * @param specular_textures - specular貼圖
     */
    Mesh(const std::vector<Vertex>& vertices,
         const std::vector<unsigned int>& indices,
         std::vector<Texture>&& diffuse_textures,
         std::vector<Texture>&& specular_textures);

    Mesh(const Mesh&) = delete;

    Mesh(Mesh&&) = default;

    /// 綁定貼圖並呼叫glDrawElements
    void draw();

private:
    // mesh data
    std::vector<Vertex>       m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture>      m_diffuse;
    std::vector<Texture>      m_specular;

    //  render data
    GLuint m_VAO, m_VBO, m_EBO;

    void setupMesh();
};
