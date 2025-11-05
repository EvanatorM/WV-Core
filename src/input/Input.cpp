#include <wv/input/Input.h>

#include <wv/input/OpenGLKey.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace WillowVox
{
    bool Input::m_pressed[348];
    bool Input::m_released[348];
    bool Input::m_mousePressed[7];
    bool Input::m_mouseReleased[7];
    double Input::m_mouseX = 0;
    double Input::m_mouseY = 0;
    double Input::m_prevMouseX = 0;
    double Input::m_prevMouseY = 0;
    double Input::m_scrollX = 0;
    double Input::m_scrollY = 0;
    Window* Input::m_window = nullptr;

    void Input::Init()
    {
        m_window = &Window::GetInstance();
        glfwSetKeyCallback(m_window->m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS)
                m_pressed[key] = true;
            else if (action == GLFW_RELEASE)
                m_released[key] = true;
        });
        glfwSetMouseButtonCallback(m_window->m_window, [](GLFWwindow* window, int button, int action, int mods) {
            if (action == GLFW_PRESS)
                m_mousePressed[button] = true;
            else if (action == GLFW_RELEASE)
                m_mouseReleased[button] = true;
        });
        glfwSetCursorPosCallback(m_window->m_window, [](GLFWwindow* window, double xpos, double ypos) {
            m_mouseX = xpos;
            m_mouseY = ypos;
        });
        glfwSetScrollCallback(m_window->m_window, [](GLFWwindow* window, double xoffset, double yoffset) {
            m_scrollX = xoffset;
            m_scrollY = yoffset;
        });

        glfwGetCursorPos(m_window->m_window, &m_prevMouseX, &m_prevMouseY);
        glfwGetCursorPos(m_window->m_window, &m_mouseX, &m_mouseY);
    }

    void Input::ResetStates()
    {
        for (int i = 0; i < 348; i++)
        {
            m_pressed[i] = false;
            m_released[i] = false;
        }
        for (int i = 0; i < 7; i++)
        {
            m_mousePressed[i] = false;
            m_mouseReleased[i] = false;
        }

        m_prevMouseX = m_mouseX;
        m_prevMouseY = m_mouseY;
        m_scrollX = 0;
        m_scrollY = 0;
    }

    bool Input::GetKey(Key key)
    {
        return glfwGetKey(m_window->m_window, keyToOpenGL[key]) == GLFW_PRESS;
    }

    bool Input::GetKeyDown(Key key)
    {
        return m_pressed[keyToOpenGL[key]];
    }

    bool Input::GetKeyUp(Key key)
    {
        return m_released[keyToOpenGL[key]];
    }

    bool Input::GetMouseButton(int button)
    {
        return glfwGetMouseButton(m_window->m_window, button) == GLFW_PRESS;
    }

    bool Input::GetMouseButtonDown(int button)
    {
        return m_mousePressed[button];
    }

    bool Input::GetMouseButtonUp(int button)
    {
        return m_mouseReleased[button];
    }

    glm::vec2 Input::GetMousePos()
    {
        double x, y;
        glfwGetCursorPos(m_window->m_window, &x, &y);
        return { x, y };
    }

    glm::vec2 Input::GetMouseDelta()
    {
        return { m_mouseX - m_prevMouseX, m_mouseY - m_prevMouseY };
    }

    glm::vec2 Input::GetMouseScrollDelta()
    {
        return { m_scrollX, m_scrollY };
    }

    void Input::SetMouseMode(MouseMode mode)
    {
        switch (mode)
        {
            case MouseMode::NORMAL:
                glfwSetInputMode(m_window->m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;
            case MouseMode::DISABLED:
                glfwSetInputMode(m_window->m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                break;
            case MouseMode::HIDDEN:
                glfwSetInputMode(m_window->m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                break;
        }
    }
}