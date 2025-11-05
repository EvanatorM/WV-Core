#pragma once

#include <wv/events/Event.h>

namespace WillowVox
{
    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() : Event() {}

        std::string ToString() const override { return "WindowCloseEvent"; }
    };
}