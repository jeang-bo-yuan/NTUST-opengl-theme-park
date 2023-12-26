/**
 * @file Water.h
 * @brief 水
 */
#ifndef WATER_H
#define WATER_H


#include <DynamicHeightMap.h>
#include <Shader.h>
#include <Wave_VAO.h>
#include <glm/vec3.hpp>
#include <vector>
#include <qtTextureImage2D.h>
#include <FBO.h>

/// water
class Water
{
public:
    enum class ReflectRefract {
        NO = 0,            ///< 不要折反射
        CUSTOM_FACTOR = 1, ///< 使用自定義的比例
        FRESNEL = 2        ///< 使用Fresnel公式計算比例
    };

private:
    Shader m_water_shader;  //!< 繪製水波的shader
    Wave_VAO m_water_vao;   //!< VAO
    DynamicHeightMap m_ripple_map; //!<
    GLuint m_frame;  //!<

    std::vector<qtTextureImage2D> m_height_maps;
    size_t m_current_height_map;

    enum {
        SINE_WAVE = 0,
        RIPPLE = 1,
        HEIGHT_MAP = 2
    } m_state; //!<

public:
    ///
    Water();

    /// 畫出來
    /// @param wireframe - 水面是否用wireframe
    void draw(bool wireframe, FBO& reflection, FBO& refraction);

    /// @brief 處理點擊事件
    /// @details 只有當|world_pos.xz| <= 5 && |world_pos.y| <= 1 才 add_drop
    /// @note 要makeCurrent
    /// @param world_pos - 點擊的世界座標
    /// @return true, if it is processed
    bool process_click(glm::vec3 world_pos);

    void use_sine_wave() { m_state = SINE_WAVE; }
    void use_ripple() { m_state = RIPPLE; }
    void use_height_map() { m_state = HEIGHT_MAP; }

    /**
     * @brief 設定呈現折反射的方式
     * @param type - 方式
     * @param factor - 折射的比例（只有在 type==ReflectRefract::NO 時才有意義）
     */
    void setReflectRefract(ReflectRefract type, float factor = 0.f);
};

#endif // WATER_H
