#include "GameCubeGraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_GAMECUBE

// libogc includes
#include <gccore.h>
#include <ogc/system.h>
#include <ogc/video.h>
#include <ogc/gx.h>
#include <malloc.h>
#include <cstring>

#define DEFAULT_FIFO_SIZE (256 * 1024)

namespace WillowVox
{
    GameCubeGraphicsContext::GameCubeGraphicsContext()
    {
        m_startTime = static_cast<float>(SYS_Time()) / TB_TIMER_CLOCK;
    }

    GameCubeGraphicsContext::~GameCubeGraphicsContext()
    {
        Shutdown();
    }

    bool GameCubeGraphicsContext::Initialize(int width, int height, const char* title)
    {
        Logger::EngineLog("Initializing GameCube Graphics (GX)");

        // Initialize VIDEO subsystem
        VIDEO_Init();

        // Get preferred video mode
        GXRModeObj* rmode = VIDEO_GetPreferredMode(NULL);
        m_renderMode = rmode;

        m_width = rmode->fbWidth;
        m_height = rmode->efbHeight;

        Logger::EngineLog("Video mode: %dx%d", m_width, m_height);

        // Allocate framebuffers (double buffering)
        m_frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
        m_frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

        if (!m_frameBuffer[0] || !m_frameBuffer[1])
        {
            Logger::EngineError("Failed to allocate framebuffers");
            return false;
        }

        // Configure VIDEO
        VIDEO_Configure(rmode);
        VIDEO_SetNextFramebuffer(m_frameBuffer[0]);
        VIDEO_SetBlack(FALSE);
        VIDEO_Flush();
        VIDEO_WaitVSync();
        if (rmode->viTVMode & VI_NON_INTERLACE)
            VIDEO_WaitVSync();

        // Allocate GX FIFO buffer
        m_fifoBuffer = memalign(32, DEFAULT_FIFO_SIZE);
        if (!m_fifoBuffer)
        {
            Logger::EngineError("Failed to allocate GX FIFO buffer");
            return false;
        }
        memset(m_fifoBuffer, 0, DEFAULT_FIFO_SIZE);

        // Initialize GX
        GX_Init(m_fifoBuffer, DEFAULT_FIFO_SIZE);

        // Set up GX for 3D rendering
        GX_SetCopyClear((GXColor){26, 26, 26, 255}, 0x00FFFFFF);

        // Set viewport
        GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
        f32 yscale = GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight);
        u32 xfbHeight = GX_SetDispCopyYScale(yscale);
        GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
        GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
        GX_SetDispCopyDst(rmode->fbWidth, xfbHeight);
        GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
        GX_SetFieldMode(rmode->field_rendering, ((rmode->viHeight == 2 * rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));

        // Set pixel format
        if (rmode->aa)
            GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
        else
            GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

        // Enable depth testing
        GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

        // Enable backface culling
        GX_SetCullMode(GX_CULL_BACK);

        // Enable blending
        GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

        // Set color update
        GX_SetColorUpdate(GX_TRUE);

        Logger::EngineLog("GameCube Graphics initialized (GX, %dx%d)", m_width, m_height);
        m_initialized = true;

        return true;
    }

    void GameCubeGraphicsContext::Shutdown()
    {
        if (m_initialized)
        {
            if (m_fifoBuffer)
            {
                free(m_fifoBuffer);
                m_fifoBuffer = nullptr;
            }

            // Framebuffers are managed by libogc, no need to free

            m_initialized = false;
            Logger::EngineLog("GameCube Graphics shutdown");
        }
    }

    void GameCubeGraphicsContext::BeginFrame()
    {
        // Nothing specific needed
    }

    void GameCubeGraphicsContext::EndFrame()
    {
        // Execute all pending GX commands
        GX_DrawDone();
    }

    void GameCubeGraphicsContext::SwapBuffers()
    {
        // Copy EFB to XFB
        GX_CopyDisp(m_frameBuffer[m_currentFB], GX_TRUE);
        GX_Flush();

        // Swap framebuffers
        VIDEO_SetNextFramebuffer(m_frameBuffer[m_currentFB]);
        VIDEO_Flush();

        if (m_vsyncEnabled)
            VIDEO_WaitVSync();

        m_currentFB ^= 1;  // Toggle between 0 and 1
    }

    void GameCubeGraphicsContext::Clear(float r, float g, float b, float a)
    {
        GXColor color = {
            static_cast<u8>(r * 255),
            static_cast<u8>(g * 255),
            static_cast<u8>(b * 255),
            static_cast<u8>(a * 255)
        };
        GX_SetCopyClear(color, 0x00FFFFFF);
    }

    void GameCubeGraphicsContext::ClearDepth()
    {
        // GX clears depth automatically with GX_CopyDisp
    }

    bool GameCubeGraphicsContext::ShouldClose() const
    {
        return m_shouldClose;
    }

    void GameCubeGraphicsContext::SetShouldClose(bool shouldClose)
    {
        m_shouldClose = shouldClose;
    }

    void GameCubeGraphicsContext::GetFramebufferSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    void GameCubeGraphicsContext::GetWindowSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    void GameCubeGraphicsContext::SetVSync(bool enabled)
    {
        m_vsyncEnabled = enabled;
    }

    bool GameCubeGraphicsContext::IsVSyncEnabled() const
    {
        return m_vsyncEnabled;
    }

    float GameCubeGraphicsContext::GetTime() const
    {
        float currentTime = static_cast<float>(SYS_Time()) / TB_TIMER_CLOCK;
        return currentTime - m_startTime;
    }

    void GameCubeGraphicsContext::SetBackgroundColor(float r, float g, float b, float a)
    {
        m_clearColor[0] = r;
        m_clearColor[1] = g;
        m_clearColor[2] = b;
        m_clearColor[3] = a;
    }

    void GameCubeGraphicsContext::SetViewport(int x, int y, int width, int height)
    {
        GX_SetViewport(x, y, width, height, 0, 1);
        GX_SetScissor(x, y, width, height);
    }
}

#endif // PLATFORM_GAMECUBE
