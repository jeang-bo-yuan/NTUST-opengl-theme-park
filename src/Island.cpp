
#include "Island.h"

Island::Island()
    : m_shader("shader/model.vert", nullptr, nullptr, nullptr, "shader/model.frag"), m_model("asset/model/island/Island.fbx"),
    m_tree_model("asset/model/tree/JASMIM+MANGA.obj"), m_house_model("asset/model/house/house.obj")
{
    m_shader.Use();
    glUniform1i(glGetUniformLocation(m_shader.Program, "diffuse_texture"), 0);
    glUseProgram(0);
}

void Island::draw(bool wireframe)
{
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    m_shader.Use();

    glUniform1i(glGetUniformLocation(m_shader.Program, "has_texture"), false);
    m_model.draw();

    glUniform1i(glGetUniformLocation(m_shader.Program, "has_texture"), true);
    m_tree_model.draw();
    m_house_model.draw();

    glUseProgram(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

