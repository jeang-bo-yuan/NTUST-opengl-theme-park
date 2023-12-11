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

/// water
class Water
{
private:
    Shader m_water_shader;
    Wave_VAO m_water_vao;
    DynamicHeightMap m_ripple_map;
    GLuint m_frame;

    enum {
        SINE_WAVE = 0,
        RIPPLE = 1,
        HEIGHT_MAP = 2
    } m_state;

public:
    ///
    Water();

    /// 畫出來
    /// @param wireframe - 水面是否用wireframe
    void draw(bool wireframe);

    /// @brief 處理點擊事件
    /// @details 只有當|world_pos.xz| <= 5 && |world_pos.y| <= 1 才 add_drop
    /// @note 要makeCurrent
    /// @param world_pos - 點擊的世界座標
    /// @return true, if it is processed
    bool process_click(glm::vec3 world_pos);

    void use_sine_wave() { m_state = SINE_WAVE; }
    void use_ripple() { m_state = RIPPLE; }
    void use_height_map() { m_state = HEIGHT_MAP; }
};

#endif // WATER_H
