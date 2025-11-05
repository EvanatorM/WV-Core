#include <wv/rendering/Window.h>

#include <wv/Logger.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <wv/rendering/Renderer.h>

namespace WillowVox
{   
    Window* Window::m_instance = nullptr;

    Window::Window(int width, int height, const char* title)
    {
        m_windowSize = { width, height };
        m_window = glfwCreateWindow(width, height, title, NULL, NULL);
        glfwSetWindowUserPointer(m_window, this);
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

        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
            auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
            glViewport(0, 0, width, height);
            self->m_windowSize = { width, height };
        });

        Renderer::PostWindowInit();
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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