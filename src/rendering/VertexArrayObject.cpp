#include <wv/rendering/VertexArrayObject.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace WillowVox
{
    VertexArrayObject::VertexArrayObject()
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        m_vertexBuffer = std::make_unique<VertexBuffer>();
        m_vertexBuffer->Bind();
        m_elementBuffer = std::make_unique<ElementBuffer>();
        m_elementBuffer->Bind();
    }

    VertexArrayObject::~VertexArrayObject()
    {
        glDeleteVertexArrays(1, &m_vao);
    }

    void VertexArrayObject::Bind()
    {
        glBindVertexArray(m_vao);
    }

    void VertexArrayObject::Draw()
    {
        Bind();
        m_elementBuffer->Draw();
    }

    void VertexArrayObject::BufferVertexData(std::size_t size, void* data)
    {
        Bind();
        m_vertexBuffer->BufferData(size, data);
    }

    void VertexArrayObject::BufferElementData(ElementBufferAttribType type, uint32_t numElements, void* data)
    {
        Bind();
        m_elementBuffer->BufferData(type, numElements, data);
    }

    void VertexArrayObject::SetAttribPointer(uint32_t index, std::size_t attribSize, VertexBufferAttribType attribType, bool normalized, std::size_t vertexSize, std::size_t offset)
    {
        Bind();
        m_vertexBuffer->SetAttribPointer(index, attribSize, attribType, normalized, vertexSize, offset);
    }

}