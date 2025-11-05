#pragma once

#include <wv/wvpch.h>

namespace WillowVox
{
    class Event
    {
    public:
        Event() : m_handled(false) {}

        bool IsHandled() const { return m_handled; }
        void MarkHandled() { m_handled = true; }

        virtual std::string ToString() const { return "Event"; }

    private:
        bool m_handled;
    };
}