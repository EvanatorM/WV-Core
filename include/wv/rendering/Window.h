#pragma once

#include <wv/wvpch.h>

struct GLFWwindow;

namespace WillowVox
{
    class Window
    {
    public:
        static Window& GetInstance()
        {
            return *m_instance;
        }
        static void InitWindow(int width, int height, const char* title)
        {
            m_instance = new Window(width, height, title);
        }

        Window(int width, int height, const char* title);
        ~Window();

        void SetBackgroundColor(float r, float g, float b, float a);

        glm::ivec2 GetWindowSize() { return m_windowSize; }

        GLFWwindow* GetWindow() { return m_window; }

        void Clear();
        void PollEvents();
        void SwapBuffers();

        bool ShouldClose() const;

        friend class Input;

    private:
        static Window* m_instance;

        glm::ivec2 m_windowSize;

        GLFWwindow* m_window;
    };
}