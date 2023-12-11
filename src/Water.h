/**
 * @file Water.h
 * @brief 水
 */
#ifndef WATER_H
#define WATER_H


#include <DynamicHeightMap.h>
#include <Shader.h>
#include <Wave_VAO.h>

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

    ///
    void draw();
};

#endif // WATER_H
