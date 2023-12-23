/**
 * @file FBO.h
 * @brief Frame Buffer Object
 */
#ifndef MY_UTILITY_FBO_H
#define MY_UTILITY_FBO_H

#include <glad/gl.h>

/// @brief Frame Buffer Object
/// @details color buffer和depth buffer都是texture
class FBO {
    GLuint m_FBO;
    GLuint m_color_buffer;
    GLuint m_depth_buffer;
    GLint m_width;
    GLint m_height;

public:
    /**
     * @brief 建構子
     * @param width - 內部的color buffer和depth buffer的寬
     * @param height - 內部的color buffer和depth buffer的高
     */
    FBO(GLint width, GLint height);

    /**
     * @brief 調整color buffer和depth buffer的大小
     * @param width - 新的寬
     * @param height - 新的高
     */
    void resize(GLint width, GLint height);

    /**
     * @brief 綁定FBO到特定的target。若target為 GL_FRAMEBUFFER 或 GL_DRAW_FRAMEBUFFER，則同時調整viewport。
     * @param target - GL_FRAMEBUFFER, GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER
     */
    void bind_FBO_and_set_viewport(GLenum target);

    /// 綁定color buffer到特定的sampler2D
    void bind_color_buffer(GLint sampler);

    /// 綁定depth buffer到特定的sampler2D
    void bind_depth_buffer(GLint sampler);
};

#endif
