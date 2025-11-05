#pragma once

#include <wv/events/Event.h>

namespace WillowVox
{
    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(int newWidth, int newHeight)
            : Event(), m_newWidth(newWidth), m_newHeight(newHeight) {
        }

        std::string ToString() const override { return "WindowResizeEvent"; }

        int m_newWidth, m_newHeight;
    };
}