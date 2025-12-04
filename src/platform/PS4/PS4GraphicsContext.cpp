#include "PS4GraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_PS4

// OpenOrbis includes
#include <orbis/Pigletv2VSH.h>
#include <orbis/VideoOut.h>
#include <orbis/libkernel.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

namespace WillowVox
{
    PS4GraphicsContext::PS4GraphicsContext()
    {
        m_startTime = sceKernelGetProcessTime() / 1000000000.0f;
    }

    PS4GraphicsContext::~PS4GraphicsContext()
    {
        Shutdown();
    }

    bool PS4GraphicsContext::Initialize(int width, int height, const char* title)
    {
        m_width = width;
        m_height = height;

        Logger::EngineLog("Initializing PS4 Graphics (OpenGL via piglet)");

        // Initialize video out
        int ret = sceVideoOutOpen(ORBIS_USER_SERVICE_USER_ID_SYSTEM, ORBIS_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);
        if (ret < 0)
        {
            Logger::EngineError("Failed to open video out: 0x%08X", ret);
            return false;
        }
        m_videoHandle = ret;

        // Initialize piglet (OpenGL wrapper for PS4)
        ret = scePigletSetConfigurationVSH(SCE_PIGLET_CONFIGURATION_VSH_API_VERSION,
                                            SCE_PIGLET_API_VERSION_GLES20);
        if (ret != 0)
        {
            Logger::EngineError("Failed to set piglet configuration: 0x%08X", ret);
            return false;
        }

        // Get EGL display
        EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY)
        {
            Logger::EngineError("eglGetDisplay failed");
            return false;
        }
        m_eglDisplay = display;

        // Initialize EGL
        if (!eglInitialize(display, nullptr, nullptr))
        {
            Logger::EngineError("eglInitialize failed");
            return false;
        }

        // Choose EGL config
        const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
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

        // Create EGL window surface
        EGLSurface surface = eglCreateWindowSurface(display, config,
                                                     (EGLNativeWindowType)m_videoHandle, nullptr);
        if (surface == EGL_NO_SURFACE)
        {
            Logger::EngineError("eglCreateWindowSurface failed");
            return false;
        }
        m_eglSurface = surface;

        // Create EGL context
        const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
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

        // Get actual surface size
        eglQuerySurface(display, surface, EGL_WIDTH, &m_width);
        eglQuerySurface(display, surface, EGL_HEIGHT, &m_height);

        // Enable OpenGL ES features
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Logger::EngineLog("PS4 Graphics initialized (OpenGL ES 2.0, %dx%d)", m_width, m_height);
        m_initialized = true;

        return true;
    }

    void PS4GraphicsContext::Shutdown()
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

            if (m_videoHandle >= 0)
            {
                sceVideoOutClose(m_videoHandle);
                m_videoHandle = -1;
            }

            m_initialized = false;
        }
    }

    void PS4GraphicsContext::BeginFrame()
    {
        // Nothing specific needed
    }

    void PS4GraphicsContext::EndFrame()
    {
        // Nothing specific needed
    }

    void PS4GraphicsContext::SwapBuffers()
    {
        if (m_eglDisplay && m_eglSurface)
        {
            eglSwapBuffers((EGLDisplay)m_eglDisplay, (EGLSurface)m_eglSurface);
        }
    }

    void PS4GraphicsContext::Clear(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void PS4GraphicsContext::ClearDepth()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    bool PS4GraphicsContext::ShouldClose() const
    {
        return m_shouldClose;
    }

    void PS4GraphicsContext::SetShouldClose(bool shouldClose)
    {
        m_shouldClose = shouldClose;
    }

    void PS4GraphicsContext::GetFramebufferSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    void PS4GraphicsContext::GetWindowSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    void PS4GraphicsContext::SetVSync(bool enabled)
    {
        m_vsyncEnabled = enabled;
        if (m_eglDisplay)
        {
            eglSwapInterval((EGLDisplay)m_eglDisplay, enabled ? 1 : 0);
        }
    }

    bool PS4GraphicsContext::IsVSyncEnabled() const
    {
        return m_vsyncEnabled;
    }

    float PS4GraphicsContext::GetTime() const
    {
        float currentTime = sceKernelGetProcessTime() / 1000000000.0f;
        return currentTime - m_startTime;
    }

    void PS4GraphicsContext::SetBackgroundColor(float r, float g, float b, float a)
    {
        m_clearColor[0] = r;
        m_clearColor[1] = g;
        m_clearColor[2] = b;
        m_clearColor[3] = a;
    }

    void PS4GraphicsContext::SetViewport(int x, int y, int width, int height)
    {
        glViewport(x, y, width, height);
    }
}

#endif // PLATFORM_PS4
