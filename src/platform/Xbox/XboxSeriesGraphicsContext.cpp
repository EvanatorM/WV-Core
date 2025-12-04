#include "XboxSeriesGraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_XBOX

// UWP includes
#include <Windows.h>
#include <wrl/client.h>

// ANGLE includes (OpenGL ES via DirectX)
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include <GLES3/gl3.h>

using namespace Microsoft::WRL;

namespace WillowVox
{
    XboxSeriesGraphicsContext::XboxSeriesGraphicsContext()
    {
        m_startTime = static_cast<float>(GetTickCount64()) / 1000.0f;
    }

    XboxSeriesGraphicsContext::~XboxSeriesGraphicsContext()
    {
        Shutdown();
    }

    bool XboxSeriesGraphicsContext::Initialize(int width, int height, const char* title)
    {
        m_width = width;
        m_height = height;

        Logger::EngineLog("Initializing Xbox Graphics (OpenGL ES via ANGLE)");

        // Get EGL display (ANGLE will translate to DirectX)
        EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY)
        {
            Logger::EngineError("eglGetDisplay failed");
            return false;
        }
        m_eglDisplay = display;

        // Initialize EGL
        EGLint major, minor;
        if (!eglInitialize(display, &major, &minor))
        {
            Logger::EngineError("eglInitialize failed");
            return false;
        }
        Logger::EngineLog("EGL Version: %d.%d", major, minor);

        // Choose EGL config for OpenGL ES 3.0
        const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
        };

        EGLConfig config;
        EGLint numConfigs;
        if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs))
        {
            Logger::EngineError("eglChooseConfig failed");
            return false;
        }

        // In a full UWP implementation, you would get the CoreWindow here
        // For now, we create a window surface with default parameters
        EGLint surfaceAttribs[] = {
            EGL_NONE
        };

        EGLSurface surface = eglCreateWindowSurface(display, config, nullptr, surfaceAttribs);
        if (surface == EGL_NO_SURFACE)
        {
            Logger::EngineError("eglCreateWindowSurface failed: 0x%X", eglGetError());
            return false;
        }
        m_eglSurface = surface;

        // Create OpenGL ES 3.0 context
        const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
        };

        EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
        if (context == EGL_NO_CONTEXT)
        {
            Logger::EngineError("eglCreateContext failed");
            return false;
        }
        m_eglContext = context;

        // Make context current
        if (!eglMakeCurrent(display, surface, surface, context))
        {
            Logger::EngineError("eglMakeCurrent failed");
            return false;
        }

        // Query surface size
        eglQuerySurface(display, surface, EGL_WIDTH, &m_width);
        eglQuerySurface(display, surface, EGL_HEIGHT, &m_height);

        // Enable OpenGL ES features
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Logger::EngineLog("Xbox Graphics initialized (OpenGL ES 3.0, %dx%d)", m_width, m_height);
        m_initialized = true;

        return true;
    }

    void XboxSeriesGraphicsContext::Shutdown()
    {
        if (m_initialized)
        {
            if (m_eglDisplay != EGL_NO_DISPLAY)
            {
                eglMakeCurrent((EGLDisplay)m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

                if (m_eglContext != EGL_NO_CONTEXT)
                {
                    eglDestroyContext((EGLDisplay)m_eglDisplay, (EGLContext)m_eglContext);
                    m_eglContext = EGL_NO_CONTEXT;
                }

                if (m_eglSurface != EGL_NO_SURFACE)
                {
                    eglDestroySurface((EGLDisplay)m_eglDisplay, (EGLSurface)m_eglSurface);
                    m_eglSurface = EGL_NO_SURFACE;
                }

                eglTerminate((EGLDisplay)m_eglDisplay);
                m_eglDisplay = EGL_NO_DISPLAY;
            }

            m_initialized = false;
        }
    }

    void XboxSeriesGraphicsContext::BeginFrame()
    {
        // Nothing specific needed
    }

    void XboxSeriesGraphicsContext::EndFrame()
    {
        // Nothing specific needed
    }

    void XboxSeriesGraphicsContext::SwapBuffers()
    {
        if (m_eglDisplay && m_eglSurface)
        {
            eglSwapBuffers((EGLDisplay)m_eglDisplay, (EGLSurface)m_eglSurface);
        }
    }

    void XboxSeriesGraphicsContext::Clear(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void XboxSeriesGraphicsContext::ClearDepth()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    bool XboxSeriesGraphicsContext::ShouldClose() const
    {
        return m_shouldClose;
    }

    void XboxSeriesGraphicsContext::SetShouldClose(bool shouldClose)
    {
        m_shouldClose = shouldClose;
    }

    void XboxSeriesGraphicsContext::GetFramebufferSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    void XboxSeriesGraphicsContext::GetWindowSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    void XboxSeriesGraphicsContext::SetVSync(bool enabled)
    {
        m_vsyncEnabled = enabled;
        if (m_eglDisplay)
        {
            eglSwapInterval((EGLDisplay)m_eglDisplay, enabled ? 1 : 0);
        }
    }

    bool XboxSeriesGraphicsContext::IsVSyncEnabled() const
    {
        return m_vsyncEnabled;
    }

    float XboxSeriesGraphicsContext::GetTime() const
    {
        float currentTime = static_cast<float>(GetTickCount64()) / 1000.0f;
        return currentTime - m_startTime;
    }

    void XboxSeriesGraphicsContext::SetBackgroundColor(float r, float g, float b, float a)
    {
        m_clearColor[0] = r;
        m_clearColor[1] = g;
        m_clearColor[2] = b;
        m_clearColor[3] = a;
    }

    void XboxSeriesGraphicsContext::SetViewport(int x, int y, int width, int height)
    {
        glViewport(x, y, width, height);
    }
}

#endif // PLATFORM_XBOX
