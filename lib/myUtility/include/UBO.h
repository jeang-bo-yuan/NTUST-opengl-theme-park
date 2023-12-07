/**
 * @file UBO.h
 * @brief Uniform Buffer Object
 */
#ifndef UBO_H
#define UBO_H

#include <glad/gl.h>

/**
 * @brief The Uniform Buffer Object
 */
class UBO
{
private:
    GLuint m_UBO_id; //!< UBO的名字
    GLenum m_usage;  //!< 使用方式（如：GL_DYNAMIC_DRAW）
    GLsizeiptr m_size;  //!< Buffer的大小

public:
    /**
     * @brief 建構子
     * @param size - buffer的大小
     * @param usage - GL_[STATIC|DYNAMIC|STREAM]_[DRAW|COPY|READ]
     */
    UBO(GLsizeiptr size, GLenum usage);

    /// @brief 解構子，呼叫 glDeleteBuffers
    ~UBO() {
        glDeleteBuffers(1, &m_UBO_id);
    }

    /// @brief 取得buffer的大小
    GLsizeiptr size() const { return m_size; }

    /**
     * @brief 重新分配buffer的大小
     * @param size - buffer的大小
     * @param usage - GL_[STATIC|DYNAMIC|STREAM]_[DRAW|COPY|READ]
     */
    void resize(GLsizeiptr size, GLenum usage);

    /**
     * @brief 將data複製進buffer
     * @param data - 資料來源
     */
    void BufferData(void* data);

    /**
     * @brief 更新一部分的buffer
     * @param offset - Specifies the offset into the buffer object's data store where data replacement will begin, measured in bytes.
     * @param size - Specifies the size in bytes of the data store region being replaced.
     * @param data - Specifies a pointer to the new data that will be copied into the data store.
     */
    void BufferSubData(GLintptr offset, GLsizeiptr size, const void * data);

    /**
     * @brief 綁定至特定的binding point
     * @param binding - binding point的index
     */
    void bind_to(GLuint binding);

    /**
     * @brief 取得UBO的名字
     * @return 由 glGenBuffers 產生的名字
     */
    GLuint name() const { return m_UBO_id; }
};

#endif // UBO_H
