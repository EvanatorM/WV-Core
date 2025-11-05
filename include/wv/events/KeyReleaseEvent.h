#pragma once

#include <wv/events/Event.h>
#include <wv/input/Key.h>

namespace WillowVox
{
    class KeyReleaseEvent : public Event
    {
    public:
        KeyReleaseEvent(Key key)
            : Event(), m_key(key) {
        }

        std::string ToString() const override { return "KeyReleaseEvent"; }

        Key m_key;
    };
}