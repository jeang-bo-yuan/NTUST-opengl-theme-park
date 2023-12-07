/**
 * @file Shader.h
 * @brief Shader class
 */
#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h>

#include <string>


/**
 * @brief The Shader class
 */
class Shader
{
public:
    enum Type {
        NULL_SHADER = (0),
        VERTEX_SHADER = (1 << 0),
        TESS_CONTROL_SHADER = (1 << 1),
        TESS_EVALUATION_SHADER = (1 << 2),
        GEOMETRY_SHADER = (1 << 3),
        FRAGMENT_SHADER = (1 << 4),
    };

public:
    GLuint Program; //!< Program的ID
    Shader::Type type = NULL_SHADER;

public:
    /**
     * @brief 在執行時編譯shader，參數為shader的檔名
     * @param vert - vertex shader
     * @param tesc - tess control shader
     * @param tese - tess evaluation shader
     * @param geom - geometry shader
     * @param frag - fragment shader
     * @throw `std::runtime_error` - 若compile失敗
     */
    Shader(const GLchar* vert, const GLchar* tesc, const GLchar* tese, const char* geom, const char* frag);

    /// Uses the current shader
    void Use();
private:
    /// 讀檔轉字串
    std::string readCode(const GLchar* path);

    /// 編譯shader
    GLuint compileShader(GLenum shader_type, const char* path);
};

#endif
