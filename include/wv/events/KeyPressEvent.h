#pragma once

#include <wv/events/Event.h>
#include <wv/input/Key.h>

namespace WillowVox
{
    class KeyPressEvent : public Event
    {
    public:
        KeyPressEvent(Key key)
            : Event(), m_key(key) {
        }

        std::string ToString() const override { return "KeyPressEvent"; }

        Key m_key;
    };
}