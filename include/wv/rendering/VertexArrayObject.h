#pragma once

#include <wv/rendering/VertexBuffer.h>
#include <wv/rendering/ElementBuffer.h>

namespace WillowVox
{
    class VertexArrayObject
    {
    public:
        VertexArrayObject();
        ~VertexArrayObject();

        void Bind();
        void Draw();

        void BufferVertexData(std::size_t size, void* data);
        void BufferElementData(ElementBufferAttribType type, uint32_t numElements, void* data);

        void SetAttribPointer(uint32_t index, std::size_t attribSize, VertexBufferAttribType attribType, bool normalized, std::size_t vertexSize, std::size_t offset);

    private:
        std::unique_ptr<VertexBuffer> m_vertexBuffer;
        std::unique_ptr<ElementBuffer> m_elementBuffer;

        unsigned int m_vao;
    };
}