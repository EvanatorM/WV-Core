#pragma once

#include <wv/platform/IGraphicsContext.h>

namespace WillowVox
{
    /**
     * Xbox Series X/S Graphics Context (UWP with OpenGL via ANGLE)
     *
     * Uses ANGLE (Almost Native Graphics Layer Evolution) to translate
     * OpenGL ES calls to DirectX on Xbox via UWP.
     */
    class XboxSeriesGraphicsContext : public IGraphicsContext
    {
    public:
        XboxSeriesGraphicsContext();
        ~XboxSeriesGraphicsContext() override;

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
        int m_width = 1920;
        int m_height = 1080;
        float m_clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
        float m_startTime = 0.0f;

        // UWP/ANGLE handles (using void* to avoid including headers)
        void* m_coreWindow = nullptr;      // Windows::UI::Core::CoreWindow^
        void* m_eglDisplay = nullptr;      // EGLDisplay
        void* m_eglSurface = nullptr;      // EGLSurface
        void* m_eglContext = nullptr;      // EGLContext
    };
}
