#include <wv/rendering/Window.h>

#include <wv/Logger.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace WillowVox
{   
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    Window::Window(int width, int height, const char* title)
    {
        m_window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (m_window == NULL)
        {
            Logger::Error("Failed to create GLFW window");
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            Logger::Error("Failed to initialize GLAD");
            return;
        }

        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    }

    Window::~Window()
    {

    }

    void Window::SetBackgroundColor(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }

    void Window::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(m_window);
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }
}