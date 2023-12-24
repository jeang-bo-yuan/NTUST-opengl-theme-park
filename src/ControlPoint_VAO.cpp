
#include "ControlPoint_VAO.h"

ControlPoint_VAO::ControlPoint_VAO(float size)
{
    GLfloat vbo_data[] = {
        //  position          normal
        size, size, size,     0, 0, 1,
        -size, size, size,    0, 0, 1,
        -size, -size, size,   0, 0, 1,
        size, -size, size,    0, 0, 1,

        size, size, -size,    0, 0, -1,
        size,-size, -size,    0, 0, -1,
        -size,-size, -size,   0, 0, -1,
        -size, size, -size,   0, 0, -1,

        size,-size, size,     0, -1, 0,
        -size,-size, size,    0, -1, 0,
        -size,-size,-size,    0, -1, 0,
        size,-size,-size,     0, -1, 0,

        size, size, size,     1, 0, 0,
        size,-size, size,     1, 0, 0,
        size,-size,-size,     1, 0, 0,
        size, size,-size,     1, 0, 0,

        -size, size, size,    -1, 0, 0,
        -size, size,-size,    -1, 0, 0,
        -size,-size,-size,    -1, 0, 0,
        -size,-size, size,    -1, 0, 0,
        // Top (GL_TRIANGLE_FAN)
        0, 3.0f * size, 0,     0, 1, 0,  // index = 20
        size, size , size,     1, 0, 1,
        -size, size , size,    -1.0f, 0.0f , 1.0f,
        -size, size ,-size,    -1.0f, 0.0f ,-1.0f,
        size, size ,-size,     1.0f, 0.0f ,-1.0f,
        size, size , size,     1.0f, 0.0f , 1.0f
    };

    glBindVertexArray(m_VAO_id);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_data), vbo_data, GL_STATIC_DRAW);
    // 0 -> aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    // 2 -> aNormal
    glVertexAttribPointer(2, 3, GL_FLOAT, false, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ControlPoint_VAO::draw()
{
    glBindVertexArray(m_VAO_id);

    glDrawArrays(GL_QUADS, 0, 20);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 6);

    glBindVertexArray(0);
}

