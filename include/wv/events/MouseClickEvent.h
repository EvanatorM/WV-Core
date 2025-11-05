#pragma once

#include <wv/events/Event.h>

namespace WillowVox
{
    class MouseClickEvent : public Event
    {
    public:
        MouseClickEvent(int button)
            : Event(), m_button(button) {
        }

        std::string ToString() const override { return "MouseClickEvent"; }

        int m_button;
    };
}