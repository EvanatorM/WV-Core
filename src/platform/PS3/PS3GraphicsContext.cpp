#include "PS3GraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_PS3

#include <rsx/rsx.h>
#include <sysutil/video.h>
#include <PSGL/psgl.h>
#include <PSGL/psglu.h>
#include <sys/time.h>

namespace WillowVox
{
    PS3GraphicsContext::PS3GraphicsContext()
    {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        m_startTime = tv.tv_sec + tv.tv_usec / 1000000.0f;
    }

    PS3GraphicsContext::~PS3GraphicsContext()
    {
        Shutdown();
    }

    bool PS3GraphicsContext::Initialize(int width, int height, const char* title)
    {
        m_width = width;
        m_height = height;

        Logger::EngineLog("Initializing PS3 Graphics (PSGL)");

        // Initialize PSGL
        PSGLinitOptions options = {
            enable: PSGL_INIT_MAX_SPUS | PSGL_INIT_INITIALIZE_SPUS,
            maxSPUs: 1,
            initializeSPUs: false,
            persistentMemorySize: 0,
            transientMemorySize: 0,
            errorConsole: 0,
            fifoSize: 0,
            hostMemorySize: 0
        };

        psglInit(&options);

        // Get available video modes
        videoState state;
        videoConfiguration vconfig;
        videoResolution resolution;

        if (videoGetState(0, 0, &state) == 0 &&
            videoGetResolution(state.displayMode.resolution, &resolution) == 0)
        {
            m_width = resolution.width;
            m_height = resolution.height;
        }

        // Set video configuration
        memset(&vconfig, 0, sizeof(videoConfiguration));
        vconfig.resolution = VIDEORESOLUTION_1920x1080;
        vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
        vconfig.pitch = m_width * 4;
        vconfig.aspect = VIDEO_ASPECT_16_9;

        // Create PSGL device
        PSGLdeviceParameters params;
        params.enable = PSGL_DEVICE_PARAMETERS_COLOR_FORMAT |
                        PSGL_DEVICE_PARAMETERS_DEPTH_FORMAT |
                        PSGL_DEVICE_PARAMETERS_MULTISAMPLING_MODE;
        params.colorFormat = GL_ARGB_SCE;
        params.depthFormat = GL_DEPTH_COMPONENT24;
        params.multisamplingMode = GL_MULTISAMPLING_NONE_SCE;

        m_psglDevice = psglCreateDeviceExtended(&params);
        if (!m_psglDevice)
        {
            Logger::EngineError("Failed to create PSGL device!");
            return false;
        }

        // Create PSGL context
        m_psglContext = psglCreateContext();
        if (!m_psglContext)
        {
            Logger::EngineError("Failed to create PSGL context!");
            psglDestroyDevice(m_psglDevice);
            return false;
        }

        // Make context current
        psglMakeCurrent(m_psglContext, m_psglDevice);

        // Reset PSGL state
        psglResetCurrentContext();

        // Get actual framebuffer dimensions
        GLuint fbWidth, fbHeight;
        psglGetDeviceDimensions(m_psglDevice, &fbWidth, &fbHeight);
        m_width = fbWidth;
        m_height = fbHeight;

        // Set viewport
        glViewport(0, 0, m_width, m_height);

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // Enable culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Logger::EngineLog("PS3 Graphics initialized (%dx%d)", m_width, m_height);
        m_initialized = true;

        return true;
    }

    void PS3GraphicsContext::Shutdown()
    {
        if (m_initialized)
        {
            if (m_psglContext)
            {
                psglDestroyContext(m_psglContext);
                m_psglContext = nullptr;
            }

            if (m_psglDevice)
            {
                psglDestroyDevice(m_psglDevice);
                m_psglDevice = nullptr;
            }

            psglExit();
            m_initialized = false;
        }
    }

    void PS3GraphicsContext::BeginFrame()
    {
        // Nothing specific for PSGL
    }

    void PS3GraphicsContext::EndFrame()
    {
        // Nothing specific for PSGL
    }

    void PS3GraphicsContext::SwapBuffers()
    {
        if (m_psglDevice)
        {
            psglSwap();
        }
    }

    void PS3GraphicsContext::Clear(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void PS3GraphicsContext::ClearDepth()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    bool PS3GraphicsContext::ShouldClose() const
    {
        return m_shouldClose;
    }

    void PS3GraphicsContext::SetShouldClose(bool shouldClose)
    {
        m_shouldClose = shouldClose;
    }

    void PS3GraphicsContext::GetFramebufferSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    void PS3GraphicsContext::GetWindowSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    void PS3GraphicsContext::SetVSync(bool enabled)
    {
        m_vsyncEnabled = enabled;
        // PSGL VSync is typically controlled by psglSwap behavior
    }

    bool PS3GraphicsContext::IsVSyncEnabled() const
    {
        return m_vsyncEnabled;
    }

    float PS3GraphicsContext::GetTime() const
    {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        float currentTime = tv.tv_sec + tv.tv_usec / 1000000.0f;
        return currentTime - m_startTime;
    }

    void PS3GraphicsContext::SetBackgroundColor(float r, float g, float b, float a)
    {
        m_clearColor[0] = r;
        m_clearColor[1] = g;
        m_clearColor[2] = b;
        m_clearColor[3] = a;
    }

    void PS3GraphicsContext::SetViewport(int x, int y, int width, int height)
    {
        glViewport(x, y, width, height);
    }
}

#endif // PLATFORM_PS3
