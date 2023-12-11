
#include "Water.h"

Water::Water()
    : m_water_shader("shader/wave.vert", nullptr, nullptr, nullptr, "shader/wave.frag"), m_water_vao(5), m_frame(0), m_state(SINE_WAVE)
{
    m_water_shader.Use();
    glUniform1i(glGetUniformLocation(m_water_shader.Program, "skybox"), 0);
    glUniform1i(glGetUniformLocation(m_water_shader.Program, "height_map"), 0);
    glUniform1i(glGetUniformLocation(m_water_shader.Program, "WAVE_SIZE"), 5);
    glUniform1i(glGetUniformLocation(m_water_shader.Program, "use_height_map"), false);
}

void Water::draw()
{
    m_water_shader.Use();

    switch(m_state) {
    case SINE_WAVE:
        ++m_frame;
        glUniform1ui(glGetUniformLocation(m_water_shader.Program, "frame"), m_frame);
        glUniform1i(glGetUniformLocation(m_water_shader.Program, "use_height_map"), false);
        break;
    case RIPPLE:
        m_ripple_map.update();
        m_ripple_map.bind(0);
        glUniform1i(glGetUniformLocation(m_water_shader.Program, "use_height_map"), true);
    case HEIGHT_MAP:
        break;
    }

    m_water_vao.draw();

    glUseProgram(0);
}

