#pragma once

#include <wv/wvpch.h>

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
        // Make sure destructor only runs on the main thread
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