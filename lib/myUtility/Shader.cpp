#include "Shader.h"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <vector>

Shader::Shader(const GLchar *vert, const GLchar *tesc, const GLchar *tese, const char *geom, const char *frag)
{
    std::vector<GLuint> shaders;
    if (vert)
    {
        shaders.push_back(this->compileShader(GL_VERTEX_SHADER, vert));
        this->type = (Shader::Type)(this->type | Type::VERTEX_SHADER);
    }
    if (tesc)
    {
        shaders.push_back(this->compileShader(GL_TESS_CONTROL_SHADER, tesc));
        this->type = (Shader::Type)(this->type | Type::TESS_CONTROL_SHADER);
    }
    if (tese)
    {
        shaders.push_back(this->compileShader(GL_TESS_EVALUATION_SHADER, tese));
        this->type = (Shader::Type)(this->type | Type::TESS_EVALUATION_SHADER);
    }
    if (geom)
    {
        shaders.push_back(this->compileShader(GL_GEOMETRY_SHADER, geom));
        this->type = (Shader::Type)(this->type | Type::GEOMETRY_SHADER);
    }
    if (frag)
    {
        shaders.push_back(this->compileShader(GL_FRAGMENT_SHADER, frag));
        this->type = (Shader::Type)(this->type | Type::FRAGMENT_SHADER);
    }
    // Shader Program
    GLint success;
    GLchar infoLog[512];
    this->Program = glCreateProgram();

    for (GLuint shader : shaders)
        glAttachShader(this->Program, shader);

    glLinkProgram(this->Program);
    // Print linking errors if any
    glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
        std::ostringstream log;
        log << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog;
        throw std::runtime_error(
            log.str()
        );
    }

    for (GLuint shader : shaders)
        glDeleteShader(shader);
}

void Shader::Use()
{
    glUseProgram(this->Program);
}

std::string Shader::readCode(const GLchar *path)
{
    std::string code;
    std::ifstream shader_file;
    // ensures ifstream objects can throw exceptions:
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // Open files
        shader_file.open(path);
        //if (!vShaderFile)std::cout << vertexPath << " fail to open" << std::endl;
        std::stringstream shader_stream;
        // Read file's buffer contents into streams
        shader_stream << shader_file.rdbuf();
        // close file handlers
        shader_file.close();
        // Convert stream into string
        code = shader_stream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::ostringstream log;
        log << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ " << path;
        throw std::runtime_error(
            log.str()
        );
    }
    return code;
}

GLuint Shader::compileShader(GLenum shader_type, const char *path)
{
    GLuint shader_number;
    GLint success;
    GLchar infoLog[512];
    std::string code = readCode(path);
    const char* code_ptr = code.c_str();

    // Vertex Shader
    shader_number = glCreateShader(shader_type);
    glShaderSource(shader_number, 1, &(code_ptr), NULL);
    glCompileShader(shader_number);

    // Print compile errors if any
    glGetShaderiv(shader_number, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader_number, 512, NULL, infoLog);
        std::ostringstream log;
        log << "ERROR::SHADER::COMPILATION_FAILED: " << path << '\n'
            << infoLog;
        throw std::runtime_error (
            log.str()
        );
    }
    return shader_number;
}
