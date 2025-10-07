#pragma once

struct GLFWwindow;

namespace WillowVox
{
    class Window
    {
    public:
        Window(int width, int height, const char* title);
        ~Window();

        void SetBackgroundColor(float r, float g, float b, float a);

        void Clear();
        void PollEvents();
        void SwapBuffers();

        bool ShouldClose() const;

    private:
        GLFWwindow* m_window;
    };
}