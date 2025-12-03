#pragma once

#include <wv/wvpch.h>

namespace WillowVox
{
    enum class VertexBufferAttribType
    {
        FLOAT64,
        FLOAT32,
        FLOAT16,
        INT32,
        UINT32,
        INT16,
        UINT16,
        INT8,
        UINT8
    };

    class VertexBuffer
    {
    public:
        VertexBuffer();
        VertexBuffer(VertexBuffer&& other) noexcept;
        // Make sure destructor only runs on the main thread
        ~VertexBuffer();

        void Bind();
        void BufferData(std::size_t size, void* data);
        void SetAttribPointer(uint32_t index, std::size_t attribSize, VertexBufferAttribType attribType, bool normalized, std::size_t vertexSize, std::size_t offset);
    private:
        unsigned int m_vbo;
    };
}