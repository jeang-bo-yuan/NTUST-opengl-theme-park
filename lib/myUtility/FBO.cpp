#include "FBO.h"
#include <stddef.h>
#include <assert.h>


FBO::FBO(GLint width, GLint height)
    : m_FBO(0), m_color_buffer(0), m_depth_buffer(0), m_width(width), m_height(height)
{
    GLint old_FBO;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &old_FBO);

    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);

    // color buffer
    glGenTextures(1, &m_color_buffer);
    glBindTexture(GL_TEXTURE_2D, m_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, /* level */0, /* internal */GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_buffer, /*mipmap level*/0);

    // depth buffer
    glGenTextures(1, &m_depth_buffer);
    glBindTexture(GL_TEXTURE_2D, m_depth_buffer);
    glTexImage2D(GL_TEXTURE_2D, /* level */0, /* internal */GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_buffer, 0);

    assert(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, old_FBO); // restore FBO
}

void FBO::resize(GLint width, GLint height)
{
    m_width = width; m_height = height;

    glBindTexture(GL_TEXTURE_2D, m_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, /* level */0, /* internal */GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, m_depth_buffer);
    glTexImage2D(GL_TEXTURE_2D, /* level */0, /* internal */GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
}

void FBO::bind_FBO_and_set_viewport(GLenum target)
{
    glBindFramebuffer(target, m_FBO);

    if (target != GL_READ_FRAMEBUFFER)
        glViewport(0, 0, m_width, m_height);
}

void FBO::bind_color_buffer(GLint sampler)
{
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(GL_TEXTURE_2D, m_color_buffer);
}

void FBO::bind_depth_buffer(GLint sampler)
{
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(GL_TEXTURE_2D, m_depth_buffer);
}
