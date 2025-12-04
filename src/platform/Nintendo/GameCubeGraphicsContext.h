#pragma once

#include <wv/platform/IGraphicsContext.h>

namespace WillowVox
{
    /**
     * Nintendo GameCube Graphics Context (GX)
     *
     * GameCube uses the GX graphics API (libogc).
     * This is a template implementation.
     */
    class GameCubeGraphicsContext : public IGraphicsContext
    {
    public:
        GameCubeGraphicsContext();
        ~GameCubeGraphicsContext() override;

        bool Initialize(int width, int height, const char* title) override;
        void Shutdown() override;
        void BeginFrame() override;
        void EndFrame() override;
        void SwapBuffers() override;
        void Clear(float r, float g, float b, float a) override;
        void ClearDepth() override;
        bool ShouldClose() const override;
        void SetShouldClose(bool shouldClose) override;
        void GetFramebufferSize(int& width, int& height) const override;
        void GetWindowSize(int& width, int& height) const override;
        void SetVSync(bool enabled) override;
        bool IsVSyncEnabled() const override;
        float GetTime() const override;
        void SetBackgroundColor(float r, float g, float b, float a) override;
        void SetViewport(int x, int y, int width, int height) override;

    private:
        bool m_initialized = false;
        bool m_shouldClose = false;
        bool m_vsyncEnabled = true;
        int m_width = 640;
        int m_height = 480;
        float m_clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
        float m_startTime = 0.0f;

        // GX-specific framebuffer (using void* to avoid including libogc headers)
        void* m_fifoBuffer = nullptr;        // GX FIFO buffer
        void* m_frameBuffer[2] = {nullptr, nullptr};  // Double buffering
        int m_currentFB = 0;
        void* m_renderMode = nullptr;        // GXRModeObj*
    };
}
