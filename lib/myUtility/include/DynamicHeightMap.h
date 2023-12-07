
#ifndef DYNAMICHEIGHTMAP_H
#define DYNAMICHEIGHTMAP_H

#include <glad/gl.h>
#include "Plane_VAO.h"
#include "Shader.h"

/// @brief 水面漣漪
class DynamicHeightMap
{
private:
    GLuint m_fbo;
    GLuint m_color_texture[2];
    GLuint m_current_frame; //!< 在0,1間來回，目前的幀使用了m_color_texture[m_current_frame]做color buffer
    Plane_VAO m_plane_VAO;
    Shader m_shader_drop;
    Shader m_shader_update;

public:
    DynamicHeightMap();

    ~DynamicHeightMap();

    /**
     * @brief 依照current frame的height map更新出下一frame的height map
     */
    void update();

    /**
     * @brief 在current frame新增drop
     * @param x - texcture coordinate上一點的x座標
     * @param y - texcture coordinate上一點的y座標
     * @pre 0 <= x,y <= 1
     */
    void add_drop(GLfloat x, GLfloat y);

    /**
     * @brief 綁定height map到特定sampler
     * @param sampler
     */
    void bind(GLuint sampler);

    /**
     * @brief 從特定sampler解除綁定
     * @param sampler
     */
    void unbind(GLuint sampler);
};

#endif // DYNAMICHEIGHTMAP_H
