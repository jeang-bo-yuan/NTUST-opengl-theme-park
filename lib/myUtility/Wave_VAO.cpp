
#include "Wave_VAO.h"
#include <glm/vec3.hpp>
#include <vector>
#include <assert.h>
#include <cmath>

Wave_VAO::Wave_VAO(GLfloat size)
    : VAO_Interface()
{
    const float delta = 1 / 32.f;
    // 畫在(-size, 0, -size) ~ (size, 0, size)
    // 每 1/32 取一個點
    std::vector<glm::vec3> point_arr;
    const GLuint width = static_cast<GLuint>(ceil(2 * size / delta) + 1);
    point_arr.reserve(width * width);
    {
        float z = -size;
        for (GLuint r = 0; r < width; ++r) {
            float x = -size;
            for (GLuint c  = 0; c < width; ++c) {
                point_arr.emplace_back(x, 0, z);

                x += delta;
            }

            z += delta;
        }
    }

    // 生成element array
    std::vector<GLuint> elem_arr;
    elem_arr.reserve((width - 1) * (width - 1) * 2 * 3);
    {
        auto to_index = [width](GLuint r, GLuint c)->GLuint {
            return r * width + c;
        };

        for (GLuint r = 0; r < width - 1; ++r) {
            for (GLuint c = 0; c < width - 1; ++c) {
                elem_arr.push_back(to_index(r, c));
                elem_arr.push_back(to_index(r + 1, c));
                elem_arr.push_back(to_index(r, c + 1));

                elem_arr.push_back(to_index(r, c + 1));
                elem_arr.push_back(to_index(r + 1, c));
                elem_arr.push_back(to_index(r + 1, c + 1));
            }
        }
    }
    m_num_of_elements = static_cast<GLuint>(elem_arr.size());

    glGenBuffers(1, &m_vbo_position);
    glGenBuffers(1, &m_ebo);
    glBindVertexArray(m_VAO_id);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_position);
    glBufferData(GL_ARRAY_BUFFER, point_arr.size() * sizeof(glm::vec3), point_arr.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elem_arr.size() * sizeof(GLuint), elem_arr.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Wave_VAO::~Wave_VAO()
{
    glDeleteBuffers(1, &m_vbo_position);
    glDeleteBuffers(1, &m_ebo);
}

void Wave_VAO::draw()
{
    glBindVertexArray(m_VAO_id);
    glDrawElements(GL_TRIANGLES, m_num_of_elements, GL_UNSIGNED_INT, (void*)0);
}

