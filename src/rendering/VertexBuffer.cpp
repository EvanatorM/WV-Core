#include <wv/rendering/VertexBuffer.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace WillowVox
{
    VertexBuffer::VertexBuffer()
    {
        glGenBuffers(1, &m_vbo);
    }

    VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
        : m_vbo(other.m_vbo)
    {
        other.m_vbo = 0;
    }

    VertexBuffer::~VertexBuffer()
    {
        glDeleteBuffers(1, &m_vbo);
    }

    void VertexBuffer::Bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    }

    void VertexBuffer::BufferData(std::size_t size, void* data)
    {
        Bind();
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    void VertexBuffer::SetAttribPointer(uint32_t index, std::size_t attribSize, VertexBufferAttribType attribType, bool normalized, std::size_t vertexSize, std::size_t offset)
    {
        Bind();
        switch (attribType)
        {
            case VertexBufferAttribType::FLOAT64:
                glVertexAttribPointer(index, attribSize, GL_DOUBLE, normalized, vertexSize, (void*)offset);
                break;
            case VertexBufferAttribType::FLOAT32:
                glVertexAttribPointer(index, attribSize, GL_FLOAT, normalized, vertexSize, (void*)offset);
                break;
            case VertexBufferAttribType::FLOAT16:
                glVertexAttribPointer(index, attribSize, GL_HALF_FLOAT, normalized, vertexSize, (void*)offset);
                break;
            case VertexBufferAttribType::INT32:
                glVertexAttribPointer(index, attribSize, GL_INT, normalized, vertexSize, (void*)offset);
                break;
            case VertexBufferAttribType::UINT32:
                glVertexAttribPointer(index, attribSize, GL_UNSIGNED_INT, normalized, vertexSize, (void*)offset);
                break;
            case VertexBufferAttribType::INT16:
                glVertexAttribPointer(index, attribSize, GL_SHORT, normalized, vertexSize, (void*)offset);
                break;
            case VertexBufferAttribType::UINT16:
                glVertexAttribPointer(index, attribSize, GL_UNSIGNED_SHORT, normalized, vertexSize, (void*)offset);
                break;
            case VertexBufferAttribType::INT8:
                glVertexAttribPointer(index, attribSize, GL_BYTE, normalized, vertexSize, (void*)offset);
                break;
            case VertexBufferAttribType::UINT8:
                glVertexAttribPointer(index, attribSize, GL_UNSIGNED_BYTE, normalized, vertexSize, (void*)offset);
                break;
        }
        glEnableVertexAttribArray(index);
    }
}