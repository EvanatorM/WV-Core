#pragma once

#include <wv/platform/IGraphicsContext.h>

namespace WillowVox
{
    /**
     * PS4 Graphics Context (GNM)
     *
     * PS4 uses GNM (low-level graphics API) via OpenOrbis SDK.
     * This is a template implementation.
     */
    class PS4GraphicsContext : public IGraphicsContext
    {
    public:
        PS4GraphicsContext();
        ~PS4GraphicsContext() override;

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
        int m_width = 1920;
        int m_height = 1080;
        float m_clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    };
}
