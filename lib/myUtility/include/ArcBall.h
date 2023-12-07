/**
 * @file ArcBall.h
 * @brief 球面座標系
 */
#ifndef ARCBALL_H
#define ARCBALL_H

#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>

/**
 * @brief 用來表示一個球面座標系，由4個參數center、r、alpha、beta控制點的位置
 * @details
 * # 參數的意義
 * - center: 球心的位置
 * - r: 球的半徑，介於`[0.1, infinity)`間
 * - alpha: 方位角，`0`為正x軸，`pi/2`為正z軸
 * - beta: 仰角，介於`[-pi/2, pi/2]`間
 *
 * # 座標系
 * x向右、y向上、z射出紙面（OpenGL慣用的座標系）
 */
class ArcBall
{
public:
    /**
     * @brief 建構子
     * @param center - 球心
     * @param r - 球的半徑，會被clamped到`[0.1, infinity)`間
     * @param alpha - 從正x軸向正z軸旋轉的角度
     * @param beta - 從正x軸向正y軸旋轉的角度，會被clamped到`[-pi/2, pi/2]`間
     */
    ArcBall(glm::vec3 center, float r, float alpha, float beta);

    /**
     * @brief 依據目前的參數，回傳點的位置
     * @return 點的位置
     */
    glm::vec3 calc_pos() const;

    /**
     * @brief 生成一個View Matrix，從點的位置看向球心
     * @details 在轉換成的視空間中，以目前的位置為原點，負z軸看向球心
     * @return View Matrix
     */
    glm::mat4 view_matrix() const;

public:
    /**
     * @name Getter and Setter
     * @{
     */

    /// Get center
    glm::vec3 center() const { return m_center; }

    /// Set Center
    void set_center(const glm::vec3& center) { m_center = center; }

    /// Get r
    float r() const { return m_r; }

    /// Set r. The value will be clamped
    void set_r(float r) { m_r = glm::max(r, 0.1f); }

    /// Get alpha
    float alpha() const { return m_alpha; }

    /// Set alpha
    void set_alpha(float alpha) { m_alpha = alpha; }

    /// Get beta
    float beta() const { return m_beta; }

    /// Set beta. The value will be clamped
    void set_beta(float beta) { m_beta = glm::clamp(beta, -glm::pi<float>() / 2.f, glm::pi<float>() / 2.f); }

    /// @}
private:
    glm::vec3 m_center;
    float m_r;
    float m_alpha;
    float m_beta;
};

#endif // ARCBALL_H
