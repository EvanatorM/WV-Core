#include <wv/rendering/ElementBuffer.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace WillowVox
{
    ElementBuffer::ElementBuffer()
    {
        glGenBuffers(1, &m_ebo);
    }

    ElementBuffer::ElementBuffer(ElementBuffer&& other) noexcept
        : m_ebo(other.m_ebo)
    {
        other.m_ebo = 0;
    }

    ElementBuffer::~ElementBuffer()
    {
        glDeleteBuffers(1, &m_ebo);
    }

    void ElementBuffer::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    }

    void ElementBuffer::BufferData(ElementBufferAttribType type, uint32_t numElements, void* data)
    {
        m_type = type;
        m_elements = numElements;
        Bind();

        switch (m_type)
        {
            case ElementBufferAttribType::UINT32:
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElements * 4, data, GL_STATIC_DRAW);
                break;
            case ElementBufferAttribType::UINT16:
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElements * 2, data, GL_STATIC_DRAW);
                break;
            case ElementBufferAttribType::UINT8:
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElements, data, GL_STATIC_DRAW);
                break;

        }
    }

    void ElementBuffer::Draw()
    {
        switch (m_type)
        {
            case ElementBufferAttribType::UINT32:
                glDrawElements(GL_TRIANGLES, m_elements, GL_UNSIGNED_INT, 0);
                break;
            case ElementBufferAttribType::UINT16:
                glDrawElements(GL_TRIANGLES, m_elements, GL_UNSIGNED_SHORT, 0);
                break;
            case ElementBufferAttribType::UINT8:
                glDrawElements(GL_TRIANGLES, m_elements, GL_UNSIGNED_BYTE, 0);
                break;

        }
    }
}