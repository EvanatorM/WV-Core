#pragma once

#include <wv/platform/IGraphicsContext.h>

namespace WillowVox
{
    /**
     * iOS Graphics Context (OpenGL ES)
     *
     * Uses OpenGL ES 3.0 with EAGLContext and CAEAGLLayer.
     * Provides compatibility for cross-platform OpenGL code.
     */
    class iOSGraphicsContext : public IGraphicsContext
    {
    public:
        iOSGraphicsContext();
        ~iOSGraphicsContext() override;

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
        int m_width = 0;
        int m_height = 0;
        float m_clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
        float m_startTime = 0.0f;

        // iOS-specific OpenGL ES handles (using void* to avoid including Objective-C headers)
        void* m_eaglContext = nullptr;       // EAGLContext*
        void* m_eaglLayer = nullptr;         // CAEAGLLayer*
        unsigned int m_colorRenderbuffer = 0;
        unsigned int m_depthRenderbuffer = 0;
        unsigned int m_framebuffer = 0;
    };
}
