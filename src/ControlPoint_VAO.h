
#ifndef CONTROLPOINT_VAO_H
#define CONTROLPOINT_VAO_H

#include <VAO_Interface.h>

/**
 * @brief 控制點的三角錐
 * @details
 * 提供的Attribute:
 * - (location = 0) aPos
 * - (location = 2) aNormal
 */
class ControlPoint_VAO : public VAO_Interface
{
    GLuint m_vbo;

public:
    ControlPoint_VAO(float size);

    void draw() override;
};

#endif // CONTROLPOINT_VAO_H
