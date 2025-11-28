#pragma once

#include <wv/platform/IGraphicsContext.h>

namespace WillowVox
{
    /**
     * PS3 Graphics Context (PSGL or libgcm)
     *
     * PS3 homebrew can use:
     * - PSGL: OpenGL-like API (easier, but less performant)
     * - libgcm: Low-level RSX graphics API (more complex, better performance)
     *
     * This implementation provides a template for either approach.
     */
    class PS3GraphicsContext : public IGraphicsContext
    {
    public:
        PS3GraphicsContext();
        ~PS3GraphicsContext() override;

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
        float m_clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
        int m_width = 1920;
        int m_height = 1080;
        float m_startTime = 0.0f;

        // PS3-specific handles (void* for template)
        void* m_psglDevice = nullptr;
        void* m_psglContext = nullptr;
        void* m_gcmContext = nullptr;
    };
}
