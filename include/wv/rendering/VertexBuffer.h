#pragma once

#include <wv/wvpch.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
        ~VertexBuffer();

        void Bind();
        void BufferData(std::size_t size, void* data);
        void SetAttribPointer(uint32_t index, std::size_t attribSize, VertexBufferAttribType attribType, bool normalized, std::size_t vertexSize, std::size_t offset);
    private:
        unsigned int m_vbo;
    };
}