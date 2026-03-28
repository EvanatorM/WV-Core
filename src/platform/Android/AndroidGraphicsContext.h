#pragma once

#include <wv/platform/IGraphicsContext.h>

// Forward declarations (avoid including Android headers in header file)
struct ANativeWindow;
struct AAssetManager;

namespace WillowVox
{
    /**
     * Android Graphics Context (OpenGL ES + EGL)
     *
     * Implements IGraphicsContext for Android using OpenGL ES 3.0+ and EGL
     * for context creation. Uses ANativeWindow for the rendering surface.
     */
    class AndroidGraphicsContext : public IGraphicsContext
    {
    public:
        AndroidGraphicsContext();
        ~AndroidGraphicsContext() override;

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

        // Android-specific
        void SetNativeWindow(ANativeWindow* window);
        void SetAssetManager(AAssetManager* assetManager);

    private:
        bool InitializeEGL();
        void ShutdownEGL();

        ANativeWindow* m_nativeWindow = nullptr;
        AAssetManager* m_assetManager = nullptr;

        // EGL handles (using void* to avoid including EGL headers)
        void* m_eglDisplay = nullptr;
        void* m_eglSurface = nullptr;
        void* m_eglContext = nullptr;

        bool m_initialized = false;
        bool m_shouldClose = false;
        bool m_vsyncEnabled = true;

        float m_clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
        int m_width = 0;
        int m_height = 0;

        float m_startTime = 0.0f;
    };
}
