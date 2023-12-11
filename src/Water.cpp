
#include "Water.h"
#include <cmath>
#include <glm/vec2.hpp>
#include <iostream>

Water::Water()
    : m_water_shader("shader/wave.vert", nullptr, nullptr, nullptr, "shader/wave.frag"), m_water_vao(5), m_frame(0), m_state(SINE_WAVE)
{
    m_water_shader.Use();
    glUniform1i(glGetUniformLocation(m_water_shader.Program, "skybox"), 0);
    glUniform1i(glGetUniformLocation(m_water_shader.Program, "height_map"), 0);
    glUniform1f(glGetUniformLocation(m_water_shader.Program, "WAVE_SIZE"), 5.f);
    glUniform1i(glGetUniformLocation(m_water_shader.Program, "use_height_map"), false);
}

void Water::draw(bool wireframe)
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
        m_water_shader.Use();
        glUniform1i(glGetUniformLocation(m_water_shader.Program, "use_height_map"), true);
    case HEIGHT_MAP:
        break;
    }

    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    m_water_vao.draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUseProgram(0);
}

bool Water::process_click(glm::vec3 world_pos)
{
    if (m_state == RIPPLE)
    if (abs(world_pos.x) <= 5 && abs(world_pos.z) <= 5 && abs(world_pos.y) <= 1) {
        glm::vec2 tex(world_pos.x, world_pos.z);
        tex = (tex + 5.f) / 10.f;

        std::cout << "Add drop: " << tex.x << ' ' << tex.y << std::endl;
        m_ripple_map.add_drop(tex.x, tex.y);

        return true;
    }

    return false;
}

