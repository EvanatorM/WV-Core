#include "WiiUGraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_WIIU

// wut library includes (Wii U homebrew)
#include <coreinit/time.h>
#include <coreinit/cache.h>
#include <coreinit/memheap.h>
#include <coreinit/screen.h>
#include <gx2/display.h>
#include <gx2/event.h>
#include <gx2/state.h>
#include <gx2/clear.h>
#include <gx2/context.h>
#include <gx2/swap.h>
#include <gx2/enum.h>
#include <malloc.h>
#include <cstring>

namespace WillowVox
{
    WiiUGraphicsContext::WiiUGraphicsContext()
    {
        m_startTime = static_cast<float>(OSGetTime()) / static_cast<float>(OSTimerClockSpeed);
    }

    WiiUGraphicsContext::~WiiUGraphicsContext()
    {
        Shutdown();
    }

    bool WiiUGraphicsContext::Initialize(int width, int height, const char* title)
    {
        Logger::EngineLog("Initializing Wii U Graphics (GX2)");

        // Get TV scan buffer dimensions
        m_width = 1280;   // Wii U TV default
        m_height = 720;

        // Initialize GX2
        GX2Init(NULL);

        // Allocate color buffer
        GX2ColorBuffer* colorBuffer = (GX2ColorBuffer*)memalign(0x100, sizeof(GX2ColorBuffer));
        if (!colorBuffer)
        {
            Logger::EngineError("Failed to allocate color buffer");
            return false;
        }
        memset(colorBuffer, 0, sizeof(GX2ColorBuffer));
        m_colorBuffer = colorBuffer;

        // Setup color buffer
        colorBuffer->surface.dim = GX2_SURFACE_DIM_TEXTURE_2D;
        colorBuffer->surface.width = m_width;
        colorBuffer->surface.height = m_height;
        colorBuffer->surface.depth = 1;
        colorBuffer->surface.mipLevels = 1;
        colorBuffer->surface.format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
        colorBuffer->surface.use = GX2_SURFACE_USE_TEXTURE_COLOR_BUFFER_TV;
        colorBuffer->viewNumSlices = 1;

        // Calculate surface size
        GX2CalcSurfaceSizeAndAlignment(&colorBuffer->surface);
        GX2InitColorBufferRegs(colorBuffer);

        // Allocate color buffer surface
        colorBuffer->surface.image = memalign(colorBuffer->surface.alignment, colorBuffer->surface.imageSize);
        if (!colorBuffer->surface.image)
        {
            Logger::EngineError("Failed to allocate color buffer surface");
            return false;
        }

        // Allocate depth buffer
        GX2DepthBuffer* depthBuffer = (GX2DepthBuffer*)memalign(0x100, sizeof(GX2DepthBuffer));
        if (!depthBuffer)
        {
            Logger::EngineError("Failed to allocate depth buffer");
            return false;
        }
        memset(depthBuffer, 0, sizeof(GX2DepthBuffer));
        m_depthBuffer = depthBuffer;

        // Setup depth buffer
        depthBuffer->surface.dim = GX2_SURFACE_DIM_TEXTURE_2D;
        depthBuffer->surface.width = m_width;
        depthBuffer->surface.height = m_height;
        depthBuffer->surface.depth = 1;
        depthBuffer->surface.mipLevels = 1;
        depthBuffer->surface.format = GX2_SURFACE_FORMAT_FLOAT_D24_S8;
        depthBuffer->surface.use = GX2_SURFACE_USE_DEPTH_BUFFER;
        depthBuffer->viewNumSlices = 1;

        // Calculate depth surface size
        GX2CalcSurfaceSizeAndAlignment(&depthBuffer->surface);
        GX2InitDepthBufferRegs(depthBuffer);

        // Allocate depth buffer surface
        depthBuffer->surface.image = memalign(depthBuffer->surface.alignment, depthBuffer->surface.imageSize);
        if (!depthBuffer->surface.image)
        {
            Logger::EngineError("Failed to allocate depth buffer surface");
            return false;
        }

        // Allocate context state
        GX2ContextState* contextState = (GX2ContextState*)memalign(GX2_CONTEXT_STATE_ALIGNMENT, sizeof(GX2ContextState));
        if (!contextState)
        {
            Logger::EngineError("Failed to allocate context state");
            return false;
        }
        GX2SetupContextStateEx(contextState, GX2_TRUE);
        m_contextState = contextState;

        // Set context state
        GX2SetContextState(contextState);

        // Set color and depth buffers
        GX2SetColorBuffer(colorBuffer, GX2_RENDER_TARGET_0);
        GX2SetDepthBuffer(depthBuffer);

        // Set viewport
        GX2SetViewport(0, 0, m_width, m_height, 0.0f, 1.0f);
        GX2SetScissor(0, 0, m_width, m_height);

        // Enable depth test
        GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_LEQUAL);

        // Enable backface culling
        GX2SetCullOnlyControl(GX2_FRONT_FACE_CCW, GX2_ENABLE, GX2_DISABLE);

        // Enable alpha blending
        GX2SetColorControl(GX2_LOGIC_OP_COPY, GX2_ENABLE, GX2_DISABLE, GX2_ENABLE);
        GX2SetBlendControl(GX2_RENDER_TARGET_0, GX2_BLEND_MODE_SRC_ALPHA, GX2_BLEND_MODE_INV_SRC_ALPHA,
                           GX2_BLEND_COMBINE_MODE_ADD, GX2_ENABLE,
                           GX2_BLEND_MODE_SRC_ALPHA, GX2_BLEND_MODE_INV_SRC_ALPHA,
                           GX2_BLEND_COMBINE_MODE_ADD);

        Logger::EngineLog("Wii U Graphics initialized (GX2, %dx%d)", m_width, m_height);
        m_initialized = true;

        return true;
    }

    void WiiUGraphicsContext::Shutdown()
    {
        if (m_initialized)
        {
            if (m_colorBuffer)
            {
                GX2ColorBuffer* cb = (GX2ColorBuffer*)m_colorBuffer;
                if (cb->surface.image)
                    free(cb->surface.image);
                free(cb);
                m_colorBuffer = nullptr;
            }

            if (m_depthBuffer)
            {
                GX2DepthBuffer* db = (GX2DepthBuffer*)m_depthBuffer;
                if (db->surface.image)
                    free(db->surface.image);
                free(db);
                m_depthBuffer = nullptr;
            }

            if (m_contextState)
            {
                free(m_contextState);
                m_contextState = nullptr;
            }

            GX2Shutdown();
            m_initialized = false;
            Logger::EngineLog("Wii U Graphics shutdown");
        }
    }

    void WiiUGraphicsContext::BeginFrame()
    {
        // Nothing specific needed
    }

    void WiiUGraphicsContext::EndFrame()
    {
        // Flush GPU
        GX2Flush();
    }

    void WiiUGraphicsContext::SwapBuffers()
    {
        // Copy color buffer to TV scan buffer
        GX2CopyColorBufferToScanBuffer((GX2ColorBuffer*)m_colorBuffer, GX2_SCAN_TARGET_TV);

        // Swap scan buffers
        GX2SwapScanBuffers();

        if (m_vsyncEnabled)
        {
            GX2WaitForVsync();
        }

        // Flush and wait
        GX2Flush();
        GX2DrawDone();
    }

    void WiiUGraphicsContext::Clear(float r, float g, float b, float a)
    {
        GX2ClearColor((GX2ColorBuffer*)m_colorBuffer, r, g, b, a);
        GX2ClearDepthStencilEx((GX2DepthBuffer*)m_depthBuffer, 1.0f, 0, GX2_CLEAR_FLAGS_BOTH);
    }

    void WiiUGraphicsContext::ClearDepth()
    {
        GX2ClearDepthStencilEx((GX2DepthBuffer*)m_depthBuffer, 1.0f, 0, GX2_CLEAR_FLAGS_DEPTH);
    }

    bool WiiUGraphicsContext::ShouldClose() const
    {
        return m_shouldClose;
    }

    void WiiUGraphicsContext::SetShouldClose(bool shouldClose)
    {
        m_shouldClose = shouldClose;
    }

    void WiiUGraphicsContext::GetFramebufferSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    void WiiUGraphicsContext::GetWindowSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    void WiiUGraphicsContext::SetVSync(bool enabled)
    {
        m_vsyncEnabled = enabled;
    }

    bool WiiUGraphicsContext::IsVSyncEnabled() const
    {
        return m_vsyncEnabled;
    }

    float WiiUGraphicsContext::GetTime() const
    {
        float currentTime = static_cast<float>(OSGetTime()) / static_cast<float>(OSTimerClockSpeed);
        return currentTime - m_startTime;
    }

    void WiiUGraphicsContext::SetBackgroundColor(float r, float g, float b, float a)
    {
        m_clearColor[0] = r;
        m_clearColor[1] = g;
        m_clearColor[2] = b;
        m_clearColor[3] = a;
    }

    void WiiUGraphicsContext::SetViewport(int x, int y, int width, int height)
    {
        GX2SetViewport(x, y, width, height, 0.0f, 1.0f);
        GX2SetScissor(x, y, width, height);
    }
}

#endif // PLATFORM_WIIU
