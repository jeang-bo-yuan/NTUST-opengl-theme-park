
#include "Skybox.h"

Skybox::Skybox()
    : m_vao(5),
    m_cubemap(":/right.jpg", ":/left.jpg", ":/top.jpg", ":/bottom.jpg", ":/front.jpg", ":/back.jpg"),
    m_skybox_shader("shader/skybox.vert", nullptr, nullptr, nullptr, "shader/skybox.frag")
{
    m_skybox_shader.Use();
    glUniform1i(glGetUniformLocation(m_skybox_shader.Program, "skybox"), 0);
}

void Skybox::draw(bool wireframe)
{
    glDepthMask(GL_FALSE);
    m_cubemap.bind_to(0);
    m_skybox_shader.Use();

    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    m_vao.draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    m_cubemap.unbind_from(0);
    glUseProgram(0);
    glDepthMask(GL_TRUE);
}

