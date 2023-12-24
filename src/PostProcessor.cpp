
#include "PostProcessor.h"

PostProcessor::PostProcessor(GLint width, GLint height)
    : m_FBO(width, height), m_old_FBO(0),
    m_shader("shader/post_process.vert", nullptr, nullptr, nullptr, "shader/post_process.frag"),
    m_whole_screen_VAO()
{
    m_shader.Use();
    glUniform1i(glGetUniformLocation(m_shader.Program, "color_buffer"), 0);
    glUniform1i(glGetUniformLocation(m_shader.Program, "depth_buffer"), 1);
    glUniform2i(glGetUniformLocation(m_shader.Program, "size"), width, height);
    glUseProgram(0);
}

void PostProcessor::resize(GLint width, GLint height)
{
    m_FBO.resize(width, height);

    m_shader.Use();
    glUniform2i(glGetUniformLocation(m_shader.Program, "size"), width, height);
    glUseProgram(0);
}

void PostProcessor::changeType(Type type)
{
    m_shader.Use();
    glUniform1i(glGetUniformLocation(m_shader.Program, "type"), (int)type);
    glUseProgram(0);
}

void PostProcessor::prepare()
{
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_old_FBO); // 記住原本的
    m_FBO.bind_FBO_and_set_viewport(GL_FRAMEBUFFER); // 綁定自己的
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 重置
}

void PostProcessor::start_post_process()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_old_FBO); // 畫在原本的FBO上

    m_FBO.bind_color_buffer(0); // 以自己的FBO當texture
    m_FBO.bind_depth_buffer(1);
    m_shader.Use();

    m_whole_screen_VAO.draw();

    glUseProgram(0);
}
