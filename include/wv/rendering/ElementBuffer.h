#pragma once

#include <wv/wvpch.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace WillowVox
{
    enum class ElementBufferAttribType
    {
        UINT32,
        UINT16,
        UINT8
    };

    class ElementBuffer
    {
    public:
        ElementBuffer();
        ElementBuffer(ElementBuffer&& other) noexcept;
        ~ElementBuffer();

        void Bind();
        void BufferData(ElementBufferAttribType type, uint32_t numElements, void* data);
        void Draw();
    private:
        unsigned int m_ebo;
        uint32_t m_elements;
        ElementBufferAttribType m_type;
    };
}