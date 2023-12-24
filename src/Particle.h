/**
 * @file Particle.h
 * @brief 粒子特效
 */
#ifndef PARTICLE_H
#define PARTICLE_H

#include <functional>
#include <glad/gl.h>
#include <vector>
#include <glm/vec3.hpp>
#include <QString>

#include <Plane_VAO.h>
#include <Shader.h>
#include <qtTextureImage2D.h>

/**
 * @brief 粒子
 * @details 它是一個2D的平面加上貼圖，而且這個面永遠朝向相機的位置
 */
class Particle
{
public:
    /// @brief 每次呼叫 Particle::update 時，會用這個函數去更新位置
    /// @details
    /// - 第一個參數（glm::vec3）是粒子的舊位置
    /// - 第二個參數（unsigned） 是粒子的TTL
    typedef std::function<glm::vec3(const glm::vec3&, unsigned)> PosTransformer;

public:
    /**
     * @brief 建構子
     * @param transformer - 更新粒子位置用的函數物件。若為nullptr，則粒子的位置固定不動
     * @param size - 顯示粒子貼圖的面要多大（實際大小2size * 2size）
     * @param img - 材質的路徑
     */
    Particle(PosTransformer transformer, float size, QString img);

    /**
     * @brief 使每個粒子的TTL減一，再用 PosTransformer 去改變他的位置
     * @details 若呼叫update時粒子的TTL為0，則刪除它
     */
    void update();

    /**
     * @brief 新增一個粒子，如果TTL是0則不會新增
     * @param position - 位置
     * @param TTL - 可以活過幾次update
     */
    void add(glm::vec3 position, unsigned TTL);

    /**
     * @brief 繪製粒子
     */
    void draw();

private:
    std::vector<glm::vec3> m_positions;  ///< 每個粒子的位置
    std::vector<unsigned> m_TTLs;  ///< 每個粒子的TTL
    PosTransformer m_transformer;  ///< 轉換

    Plane_VAO m_plane_VAO; ///< 繪製平面
    GLuint m_translate_vbo; ///< Instanced Array，告訴Shader每個粒子的位置

    Shader m_shader;
    qtTextureImage2D m_img;
};

#endif // PARTICLE_H
