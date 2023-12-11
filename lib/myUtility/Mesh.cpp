
#include "Mesh.h"
#include <stddef.h>

Mesh::Mesh(const std::vector<Vertex> &vertices,
           const std::vector<unsigned int> &indices,
           const std::vector<Texture> &diffuse_textures,
           const std::vector<Texture> &specular_textures)
    : m_vertices(vertices), m_indices(indices), m_diffuse(diffuse_textures), m_specular(specular_textures)
{
    setupMesh();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void Mesh::draw()
{
    glBindVertexArray(m_VAO);

    // bind textures
    // for "texture_diffuse1" to "texture_diffuse(i+1)"
    for (int i = 0; i < m_diffuse.size(); ++i) {
        // 綁定 texture_diffuse(i+1) 到 2 * ((i+1) - 1) 上
        m_diffuse[i]->bind_to(2 * i);
    }

    // for "texture_specular1" to "texture_specular(i+1)"
    for (int i = 0; i < m_specular.size(); ++i) {
        // 綁定 texture_specular(i+1) 到 2 * ((i+1) - 1) + 1
        m_specular[i]->bind_to(2 * i + 1);
    }

    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);


    // set up vbo
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
    // set up attribute
    // 0 -> aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Mesh::Vertex, aPosition));
    glEnableVertexAttribArray(0);
    // 1 -> aTexcoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Mesh::Vertex, aTexcoord));
    glEnableVertexAttribArray(1);
    // 2 -> aNormal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Mesh::Vertex, aNormal));
    glEnableVertexAttribArray(2);


    // set up EBO
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);


    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


