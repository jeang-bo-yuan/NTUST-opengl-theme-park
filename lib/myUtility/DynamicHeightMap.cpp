
#include "DynamicHeightMap.h"
#include <stddef.h>
#include <assert.h>
#include <QDebug>

#define IMG_SIZE 100

DynamicHeightMap::DynamicHeightMap()
    : m_current_frame(0), m_plane_VAO(), m_shader_drop("shader/DHM/simple.vert", nullptr, nullptr, nullptr, "shader/DHM/drop.frag"),
    m_shader_update("shader/DHM/simple.vert", nullptr, nullptr, nullptr, "shader/DHM/update.frag")
{
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(2, m_color_texture);
    for (GLuint i = 0; i < 2; ++i) {
        // initialize each texture
        glBindTexture(GL_TEXTURE_2D, m_color_texture[i]);
        glTexImage2D(GL_TEXTURE_2D, /* level */ 0, /* internal */ GL_RGBA32F, IMG_SIZE, IMG_SIZE, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_texture[m_current_frame], 0);

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // set up sampler
    m_shader_drop.Use();
    glUniform1i(glGetUniformLocation(m_shader_drop.Program, "u_water"), 1);
    m_shader_update.Use();
    glUniform1i(glGetUniformLocation(m_shader_update.Program, "u_water"), 1);
    glUseProgram(0);
}

DynamicHeightMap::~DynamicHeightMap()
{
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteTextures(2, m_color_texture);
}

void DynamicHeightMap::update()
{
    glDepthFunc(GL_ALWAYS);

    GLint old_fbo;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &old_fbo);
    GLint old_viewport[4];
    glGetIntegerv(GL_VIEWPORT, old_viewport);

    // 綁定自己的frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    // 使用下一frame作為color buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_texture[(m_current_frame + 1) % 2], 0);
    // 確定為complete
    assert(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glViewport(0, 0, IMG_SIZE, IMG_SIZE);
    m_shader_update.Use();
    this->bind(1); // 綁定current frame
    m_plane_VAO.draw();

    glBindFramebuffer(GL_FRAMEBUFFER, old_fbo);
    glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);
    this->unbind(1);
    glDepthFunc(GL_LESS);

    // update current frame
    m_current_frame = (m_current_frame + 1) % 2;
}

void DynamicHeightMap::add_drop(GLfloat x, GLfloat y)
{
    glDepthFunc(GL_ALWAYS);

    GLint old_fbo;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &old_fbo);
    GLint old_viewport[4];
    glGetIntegerv(GL_VIEWPORT, old_viewport);

    // 綁定自己的frame buffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    assert(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glViewport(0, 0, IMG_SIZE, IMG_SIZE);
    m_shader_drop.Use();
    this->bind(1);
    // call glUniform to pass (x, y)
    glUniform2f(glGetUniformLocation(m_shader_drop.Program, "u_center"), x, y);
    m_plane_VAO.draw();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, old_fbo);
    glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);
    this->unbind(1);
    glDepthFunc(GL_LESS);
}

void DynamicHeightMap::bind(GLuint sampler)
{
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(GL_TEXTURE_2D, m_color_texture[m_current_frame]);
}

void DynamicHeightMap::unbind(GLuint sampler)
{
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(GL_TEXTURE_2D, 0);
}

