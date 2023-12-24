
#include "Particle.h"


Particle::Particle(PosTransformer transformer, float size, QString img)
    : m_transformer(transformer), m_shader("shader/particle.vert", nullptr, nullptr, nullptr, "shader/particle.frag"), m_img(img)
{
    m_shader.Use();
    glUniform1i(glGetUniformLocation(m_shader.Program, "img"), 0);
    glUniform1f(glGetUniformLocation(m_shader.Program, "size"), size);
    glUseProgram(0);

    // set vbo
    glBindVertexArray(m_plane_VAO.name());

    glGenBuffers(1, &m_translate_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_translate_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
    // 將translate綁在編號2
    glVertexAttribPointer(2, 3, GL_FLOAT, false, /*stride*/0, (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1); // 每過一個instance才取一個translate

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Particle::update()
{
    for (int i = 0; i < m_positions.size(); ) {
        if (m_TTLs[i] == 0) {
            // delete this particle
            m_TTLs.erase(m_TTLs.begin() + i);
            m_positions.erase(m_positions.begin() + i);
            // i 不需遞增，因為後面的往前補了
        }
        else {
            --m_TTLs[i];
            if (m_transformer) // 不為nullptr，則更新位置
                m_positions[i] = m_transformer(m_positions[i], m_TTLs[i]);

            ++i;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_translate_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(glm::vec3), m_positions.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Particle::add(glm::vec3 position, unsigned int TTL)
{
    if (TTL == 0) return;

    m_positions.push_back(position);
    m_TTLs.push_back(TTL);

    glBindBuffer(GL_ARRAY_BUFFER, m_translate_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(glm::vec3), m_positions.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Particle::draw()
{
    m_shader.Use();
    m_img.bind_to(0);

    m_plane_VAO.drawInstanced(m_positions.size());

    m_img.unbind_from(0);
    glUseProgram(0);
}
