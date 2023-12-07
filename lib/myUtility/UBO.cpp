
#include "UBO.h"

UBO::UBO(GLsizeiptr size, GLenum usage)
    : m_UBO_id(0), m_usage(usage), m_size(size)
{
    glGenBuffers(1, &m_UBO_id);
    this->resize(size, usage);
}

void UBO::resize(GLsizeiptr size, GLenum usage)
{
    m_size = size;
    m_usage = usage;

    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO_id);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, usage);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::BufferData(void *data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO_id);
    glBufferData(GL_UNIFORM_BUFFER, m_size, data, m_usage);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::BufferSubData(GLintptr offset, GLsizeiptr size, const void *data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO_id);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::bind_to(GLuint binding)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_UBO_id);
}

