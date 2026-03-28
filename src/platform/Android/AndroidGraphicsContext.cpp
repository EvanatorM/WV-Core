#include "AndroidGraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_ANDROID

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/native_window.h>
#include <android/asset_manager.h>
#include <time.h>

namespace WillowVox
{
    AndroidGraphicsContext::AndroidGraphicsContext()
    {
        // Get start time
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        m_startTime = ts.tv_sec + ts.tv_nsec / 1000000000.0f;
    }

    AndroidGraphicsContext::~AndroidGraphicsContext()
    {
        Shutdown();
    }

    void AndroidGraphicsContext::SetNativeWindow(ANativeWindow* window)
    {
        m_nativeWindow = window;
    }

    void AndroidGraphicsContext::SetAssetManager(AAssetManager* assetManager)
    {
        m_assetManager = assetManager;
    }

    bool AndroidGraphicsContext::Initialize(int width, int height, const char* title)
    {
        m_width = width;
        m_height = height;

        if (!m_nativeWindow)
        {
            Logger::EngineError("Android: No native window set!");
            return false;
        }

        if (!InitializeEGL())
        {
            Logger::EngineError("Failed to initialize EGL");
            return false;
        }

        // Enable OpenGL ES features
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Logger::EngineLog("Android Graphics Context initialized (OpenGL ES %s)",
                          glGetString(GL_VERSION));

        m_initialized = true;
        return true;
    }

    bool AndroidGraphicsContext::InitializeEGL()
    {
        // Get default display
        EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY)
        {
            Logger::EngineError("eglGetDisplay failed");
            return false;
        }

        // Initialize EGL
        if (!eglInitialize(display, nullptr, nullptr))
        {
            Logger::EngineError("eglInitialize failed");
            return false;
        }

        // Choose config
        const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
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

        // Create surface
        EGLSurface surface = eglCreateWindowSurface(display, config, m_nativeWindow, nullptr);
        if (surface == EGL_NO_SURFACE)
        {
            Logger::EngineError("eglCreateWindowSurface failed");
            return false;
        }

        // Create context
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

        // Make current
        if (!eglMakeCurrent(display, surface, surface, context))
        {
            Logger::EngineError("eglMakeCurrent failed");
            return false;
        }

        // Store handles
        m_eglDisplay = display;
        m_eglSurface = surface;
        m_eglContext = context;

        // Get actual surface size
        eglQuerySurface(display, surface, EGL_WIDTH, &m_width);
        eglQuerySurface(display, surface, EGL_HEIGHT, &m_height);

        // Set VSync
        eglSwapInterval(display, m_vsyncEnabled ? 1 : 0);

        return true;
    }

    void AndroidGraphicsContext::Shutdown()
    {
        if (m_initialized)
        {
            ShutdownEGL();
            m_initialized = false;
        }
    }

    void AndroidGraphicsContext::ShutdownEGL()
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
    }

    void AndroidGraphicsContext::BeginFrame()
    {
        // Nothing specific needed
    }

    void AndroidGraphicsContext::EndFrame()
    {
        // Nothing specific needed
    }

    void AndroidGraphicsContext::SwapBuffers()
    {
        if (m_eglDisplay && m_eglSurface)
        {
            eglSwapBuffers((EGLDisplay)m_eglDisplay, (EGLSurface)m_eglSurface);
        }
    }

    void AndroidGraphicsContext::Clear(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void AndroidGraphicsContext::ClearDepth()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    bool AndroidGraphicsContext::ShouldClose() const
    {
        return m_shouldClose;
    }

    void AndroidGraphicsContext::SetShouldClose(bool shouldClose)
    {
        m_shouldClose = shouldClose;
    }

    void AndroidGraphicsContext::GetFramebufferSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    void AndroidGraphicsContext::GetWindowSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    void AndroidGraphicsContext::SetVSync(bool enabled)
    {
        m_vsyncEnabled = enabled;
        if (m_eglDisplay)
        {
            eglSwapInterval((EGLDisplay)m_eglDisplay, enabled ? 1 : 0);
        }
    }

    bool AndroidGraphicsContext::IsVSyncEnabled() const
    {
        return m_vsyncEnabled;
    }

    float AndroidGraphicsContext::GetTime() const
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        float currentTime = ts.tv_sec + ts.tv_nsec / 1000000000.0f;
        return currentTime - m_startTime;
    }

    void AndroidGraphicsContext::SetBackgroundColor(float r, float g, float b, float a)
    {
        m_clearColor[0] = r;
        m_clearColor[1] = g;
        m_clearColor[2] = b;
        m_clearColor[3] = a;
    }

    void* AndroidGraphicsContext::GetNativeWindowHandle() const
    {
        return m_nativeWindow;
    }

    void* AndroidGraphicsContext::GetNativeGraphicsHandle() const
    {
        return m_eglContext;
    }

    void AndroidGraphicsContext::SetViewport(int x, int y, int width, int height)
    {
        glViewport(x, y, width, height);
    }
}

#endif // PLATFORM_ANDROID
