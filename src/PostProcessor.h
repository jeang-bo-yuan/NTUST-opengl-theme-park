/**
 * @file PostProcessor.h
 * @brief 畫面後處理
 */
#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

#include <FBO.h>
#include <Shader.h>
#include <Plane_VAO.h>
#include <vector>
#include <qtTextureImage2D.h>

/// @brief 用來做後處理的class
/// @details How to Use:
/// 1. 呼叫 prepare() ，PostProcessor 會綁定自己的FBO到 GL_FRAMEBUFFER 並呼叫 glClear()
/// 2. 開始繪製其他物件
/// 3. 呼叫 start_post_process() ，PostProcessor 會將呼叫 prepare() 之前的 GL_DRAW_FRAMEBUFFER_BINDING 給綁定到 GL_FRAMEBUFFER，並將自己FBO的內容畫上去
class PostProcessor
{
public:
    enum class Type {
        NoProcess = 0,       // 不做後處理
        Pixelization = 1,    // 像素化
        Grayscale = 2,       // 灰階
        DepthImage = 3,      // 深度圖
        SobelOperator = 4,   // 邊緣偵測
        SpeedLine = 5,       // 速度線
    };

private:
    Type m_type; ///< 後處理的種類

    FBO m_FBO; ///< 自己的FBO
    GLint m_old_FBO; ///< 呼叫 prepare() 時，將原本的 GL_DRAW_FRAMEBUFFER_BINDING 給記起來
    Shader m_shader;  ///< shader，用來做後處理
    Plane_VAO m_whole_screen_VAO; ///< 繪製整個螢幕

    std::vector<qtTextureImage2D> m_speeds; ///< textures of speed line
    int m_which_speed; ///< use which speed line texture

    static constexpr int SPEED_NUM = 18; ///< m_speeds的大小

public:
    /// constructor，Type預設為 PostProcessor::Type::NoProcess
    PostProcessor(GLint width, GLint height);

    /// 重新調整內部的FBO的大小
    void resize(GLint width, GLint height);

    /**
     * @brief 改變後處理的方式
     * @param type - 方式
     */
    void changeType(PostProcessor::Type type);

    /// 準備
    void prepare();

    /// 開始
    void start_post_process();

    Shader& shader() { return m_shader; }
    Plane_VAO& vao() { return m_whole_screen_VAO; }
};

#endif // POSTPROCESSOR_H
