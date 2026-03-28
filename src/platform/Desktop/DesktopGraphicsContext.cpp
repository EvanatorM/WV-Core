#include "DesktopGraphicsContext.h"
#include <wv/Logger.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace WillowVox
{
    static void GLFWErrorCallback(int error, const char* description)
    {
        Logger::EngineError("GLFW Error (%d): %s", error, description);
    }

    DesktopGraphicsContext::DesktopGraphicsContext()
    {
    }

    DesktopGraphicsContext::~DesktopGraphicsContext()
    {
        Shutdown();
    }

    bool DesktopGraphicsContext::Initialize(int width, int height, const char* title)
    {
        m_width = width;
        m_height = height;

        // Initialize GLFW
        glfwSetErrorCallback(GLFWErrorCallback);

        if (!glfwInit())
        {
            Logger::EngineError("Failed to initialize GLFW");
            return false;
        }

        // Configure GLFW
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        // Create window
        m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!m_window)
        {
            Logger::EngineError("Failed to create GLFW window");
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(m_window);

        // Load OpenGL functions
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            Logger::EngineError("Failed to initialize GLAD");
            glfwDestroyWindow(m_window);
            glfwTerminate();
            return false;
        }

        // Set VSync
        glfwSwapInterval(m_vsyncEnabled ? 1 : 0);

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Logger::EngineLog("Desktop Graphics Context initialized (OpenGL %s)", glGetString(GL_VERSION));

        return true;
    }

    void DesktopGraphicsContext::Shutdown()
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
        glfwTerminate();
    }

    void DesktopGraphicsContext::BeginFrame()
    {
        // Nothing special needed for GLFW
    }

    void DesktopGraphicsContext::EndFrame()
    {
        // Nothing special needed for GLFW
    }

    void DesktopGraphicsContext::SwapBuffers()
    {
        if (m_window)
        {
            glfwSwapBuffers(m_window);
        }
    }

    void DesktopGraphicsContext::Clear(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void DesktopGraphicsContext::ClearDepth()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    bool DesktopGraphicsContext::ShouldClose() const
    {
        return m_window ? glfwWindowShouldClose(m_window) : true;
    }

    void DesktopGraphicsContext::SetShouldClose(bool shouldClose)
    {
        if (m_window)
        {
            glfwSetWindowShouldClose(m_window, shouldClose ? GLFW_TRUE : GLFW_FALSE);
        }
    }

    void DesktopGraphicsContext::GetFramebufferSize(int& width, int& height) const
    {
        if (m_window)
        {
            glfwGetFramebufferSize(m_window, &width, &height);
        }
        else
        {
            width = m_width;
            height = m_height;
        }
    }

    void DesktopGraphicsContext::GetWindowSize(int& width, int& height) const
    {
        if (m_window)
        {
            glfwGetWindowSize(m_window, &width, &height);
        }
        else
        {
            width = m_width;
            height = m_height;
        }
    }

    void DesktopGraphicsContext::SetVSync(bool enabled)
    {
        m_vsyncEnabled = enabled;
        glfwSwapInterval(enabled ? 1 : 0);
    }

    bool DesktopGraphicsContext::IsVSyncEnabled() const
    {
        return m_vsyncEnabled;
    }

    float DesktopGraphicsContext::GetTime() const
    {
        return static_cast<float>(glfwGetTime());
    }

    void DesktopGraphicsContext::SetBackgroundColor(float r, float g, float b, float a)
    {
        m_clearColor[0] = r;
        m_clearColor[1] = g;
        m_clearColor[2] = b;
        m_clearColor[3] = a;
    }

    void* DesktopGraphicsContext::GetNativeWindowHandle() const
    {
        return m_window;
    }

    void* DesktopGraphicsContext::GetNativeGraphicsHandle() const
    {
        // For OpenGL, this could return the GL context
        return nullptr;
    }

    void DesktopGraphicsContext::SetViewport(int x, int y, int width, int height)
    {
        glViewport(x, y, width, height);
    }
}
