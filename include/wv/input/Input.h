#pragma once

#include <wv/rendering/Window.h>
#include <wv/input/Key.h>
#include <wv/wvpch.h>

namespace WillowVox
{
    enum class MouseMode
    {
        NORMAL,
        DISABLED,
        HIDDEN
    };

    class Input
    {
    public:
        static void Init();
        static void ResetStates();
        
        static bool GetKey(Key key);
        static bool GetKeyDown(Key key);
        static bool GetKeyUp(Key key);

        static bool GetMouseButton(int button);
        static bool GetMouseButtonDown(int button);
        static bool GetMouseButtonUp(int button);

        static glm::vec2 GetMousePos();
        static glm::vec2 GetMouseDelta();
        static glm::vec2 GetMouseScrollDelta();

        static void SetMouseMode(MouseMode mode);

    private:
        static bool m_pressed[348];
        static bool m_released[348];
        static bool m_mousePressed[7];
        static bool m_mouseReleased[7];
        static double m_mouseX, m_mouseY, m_prevMouseX, m_prevMouseY;
        static double m_scrollX, m_scrollY;

        static Window* m_window;
    };
}