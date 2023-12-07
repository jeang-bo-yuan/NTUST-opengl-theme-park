/**
 * @file qtTextureImage2D.h
 * @brief 從圖載入texture
 */
#ifndef QTTEXTUREIMAGE2D_H
#define QTTEXTUREIMAGE2D_H

#include <glad/gl.h>
#include <QString>

/**
 * @brief 從圖載入texture
 */
class qtTextureImage2D
{
private:
    GLuint m_texture_id;

public:
    /**
     * @brief 建構子
     * @param path - 圖片路徑
     * @throw std::invalid_argument - 若無法開啟圖片
     */
    qtTextureImage2D(const QString& path);

    qtTextureImage2D(const qtTextureImage2D&) = delete;

    /**
     * @brief 將rvalue搬過來，並使rvalue變成空的texture（name = 0）
     * @param rvalue
     */
    qtTextureImage2D(qtTextureImage2D&& rvalue);

    /// 呼叫glDeleteTextures
    ~qtTextureImage2D();

    /// 綁定到特定sampler
    void bind_to(GLuint sampler);

    /// 從特定sampler解除綁定
    void unbind_from(GLuint sampler);
};

#endif // QTTEXTUREIMAGE2D_H
