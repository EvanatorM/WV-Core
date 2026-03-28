#pragma once

#include <wv/platform/IGraphicsContext.h>

namespace WillowVox
{
    /**
     * Nintendo Switch Graphics Context (OpenGL)
     *
     * Switch homebrew supports OpenGL via mesa/deko3d or native NVN.
     * This template uses OpenGL for simplicity.
     */
    class SwitchGraphicsContext : public IGraphicsContext
    {
    public:
        SwitchGraphicsContext();
        ~SwitchGraphicsContext() override;

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
        int m_width = 1280;
        int m_height = 720;
        float m_clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
        float m_startTime = 0.0f;
    };
}
