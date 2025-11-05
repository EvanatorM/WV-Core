#pragma once

#include <wv/events/Event.h>

namespace WillowVox
{
    class MouseScrollEvent : public Event
    {
    public:
        MouseScrollEvent(float xOffset, float yOffset)
            : Event(), m_xOffset(xOffset), m_yOffset(yOffset) {
        }

        std::string ToString() const override { return "MouseScrollEvent"; }

        float m_xOffset, m_yOffset;
    };
}