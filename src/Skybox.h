/**
 * @file Skybox.h
 * @brief skybox 物件
 */
#ifndef SKYBOX_H
#define SKYBOX_H

#include <Box_VAO.h>
#include <qtTextureCubeMap.h>
#include <Shader.h>


class Skybox
{
private:
    Box_VAO m_vao;
    qtTextureCubeMap m_cubemap;
    Shader m_skybox_shader;

public:
    /// 建立一個skybox物件
    Skybox();

    /// 繪製skybox
    void draw();

    /// 取得cubemap texture的參考
    qtTextureCubeMap& cubemap() { return m_cubemap; }
};

#endif // SKYBOX_H
