
#include "Box_VAO.h"

Box_VAO::Box_VAO(GLfloat size)
{
    GLfloat vbo_data[] = {
        // position                // normal
        // negative x
        -size, -size, -size,        -1, 0, 0,
        -size, -size, +size,        -1, 0, 0,
        -size, +size, +size,        -1, 0, 0,
        -size, +size, -size,        -1, 0, 0,
        // positive x
        +size, -size, -size,        1, 0, 0,
        +size, -size, +size,        1, 0, 0,
        +size, +size, +size,        1, 0, 0,
        +size, +size, -size,        1, 0, 0,
        // negative z
        -size, +size, -size,        0, 0, -1,
        +size, +size, -size,        0, 0, -1,
        +size, -size, -size,        0, 0, -1,
        -size, -size, -size,        0, 0, -1,
        // positive z
        -size, +size, +size,        0, 0, 1,
        +size, +size, +size,        0, 0, 1,
        +size, -size, +size,        0, 0, 1,
        -size, -size, +size,        0, 0, 1,
        // negative y
        -size, -size, -size,        0, -1, 0,
        +size, -size, -size,        0, -1, 0,
        +size, -size, +size,        0, -1, 0,
        -size, -size, +size,        0, -1, 0,
        // positive y
        -size, +size, -size,        0, 1, 0,
        +size, +size, -size,        0, 1, 0,
        +size, +size, +size,        0, 1, 0,
        -size, +size, +size,        0, 1, 0,
    };

    glBindVertexArray(m_VAO_id);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_data), vbo_data, GL_STATIC_DRAW);

    // attribute position
    glVertexAttribPointer(/*index*/0, /*size*/3, GL_FLOAT, /*normalized*/false, /*stride*/6 * sizeof(GLfloat), /*offset*/(void*)(0));
    glEnableVertexAttribArray(0);

    // attribute normal
    glVertexAttribPointer(/*index*/2, /*size*/3, GL_FLOAT, /*normalized*/false, /*stride*/6 * sizeof(GLfloat), /*offset*/(void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Box_VAO::~Box_VAO()
{
    glDeleteBuffers(1, &m_vbo);
}

void Box_VAO::draw()
{
    glBindVertexArray(m_VAO_id);
    glDrawArrays(GL_QUADS, /*first*/0, /*count*/24);
}

void Box_VAO::draw_face(FACE face)
{
    glBindVertexArray(m_VAO_id);
    glDrawArrays(GL_QUADS, /*first*/(int)face, /*count*/4);
}
