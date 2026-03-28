#pragma once

#include <wv/platform/IGraphicsContext.h>

// Forward declare GLFW types to avoid including GLFW in header
struct GLFWwindow;

namespace WillowVox
{
    /**
     * Desktop Graphics Context (OpenGL + GLFW)
     *
     * Implements IGraphicsContext for desktop platforms (Windows, Linux, macOS)
     * using GLFW for windowing and OpenGL for rendering.
     */
    class DesktopGraphicsContext : public IGraphicsContext
    {
    public:
        DesktopGraphicsContext();
        ~DesktopGraphicsContext() override;

        // IGraphicsContext implementation
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

        void* GetNativeWindowHandle() const override;
        void* GetNativeGraphicsHandle() const override;

        void SetViewport(int x, int y, int width, int height) override;

        // Desktop-specific
        GLFWwindow* GetGLFWWindow() const { return m_window; }

    private:
        GLFWwindow* m_window = nullptr;
        bool m_vsyncEnabled = true;
        float m_clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
        int m_width = 0;
        int m_height = 0;
    };
}
