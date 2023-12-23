/**
 * @file Mesh.h
 * @brief Mesh Object
 */
#include <glad/gl.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>

#include <memory>

#include "assimp/mesh.h"
#include "qtTextureImage2D.h"

/**
 * @brief 用來表示模型中的一部分
 * @details
 * # 提供的 Attribute
 * - (location = 0) Vertex::aPosition
 * - (location = 1) Vertex::aTexcoord
 * - (location = 2) Vertex::aNormal
 *
 * - (location = 3) color set 1
 * - (location = 4) color set 2
 * - (location = 5) color set 3
 * ...
 * - (location = 10) color set 8
 */
class Mesh {
public:
    /// 頂點資訊
    struct Vertex {
        glm::vec3 aPosition;  ///< 位置
        glm::vec3 aNormal;    ///< 法向量
        glm::vec2 aTexcoord;  ///< 材質座標
    };

    /// 材質，指向qtTextureImage2D的指標
    typedef std::shared_ptr<qtTextureImage2D> Texture;

    /// 一個顏色的2維陣列，第一個index代表第幾個color set（index 0 = 第一個），第二個index則是vertex的編號
    typedef std::vector<std::vector<glm::vec4>> Color_Set;

public:
    /**
     * @brief 建構子
     * @param vertices - 頂點座標
     * @param indices - 繪製順序（三角型）
     * @param diffuse_textures - diffuse貼圖
     * @param specular_textures - specular貼圖
     * @param colors - 數個color
     * @note Mesh 物件以 std::shared_ptr 來共享 texture
     */
    Mesh(const std::vector<Vertex>& vertices,
         const std::vector<unsigned int>& indices,
         const std::vector<Texture>& diffuse_textures,
         const std::vector<Texture>& specular_textures,
         const Color_Set& colors);

    Mesh(const Mesh&) = delete;

    /// move constructor, rvalue will become useless (VAO = 0)
    Mesh(Mesh&& rvalue);

    /// 呼叫 glDeleteVertexArrays 和 glDeleteBuffers
    ~Mesh();

    /// 綁定貼圖並呼叫glDrawElements
    void draw();

private:
    // mesh data
    std::vector<Vertex>       m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture>      m_diffuse;
    std::vector<Texture>      m_specular;
    Color_Set                 m_colors;

    //  render data
    GLuint m_VAO, m_VBO, m_EBO;

    // color，如果是0代表沒有VBO
    GLuint m_Color_VBOs[AI_MAX_NUMBER_OF_COLOR_SETS];

    void setupMesh();
};
