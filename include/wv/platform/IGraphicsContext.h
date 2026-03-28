#pragma once

#include <wv/platform/PlatformConfig.h>
#include <wv/wvpch.h>

namespace WillowVox
{
    /**
     * Abstract Graphics Context Interface
     *
     * This interface abstracts the platform-specific graphics initialization,
     * window/framebuffer management, and basic rendering operations.
     *
     * Each platform implements this interface using its native graphics API:
     * - Desktop: OpenGL + GLFW
     * - Android: OpenGL ES + EGL/ANativeWindow
     * - iOS: Metal or OpenGL ES + UIKit
     * - PS3: PSGL or GCM
     * - PS4: GNM
     * - Wii/GC: GX
     * - Wii U: GX2
     * - Switch: OpenGL or NVN
     * - Xbox: DirectX 12
     */
    class IGraphicsContext
    {
    public:
        virtual ~IGraphicsContext() = default;

        // Initialization and shutdown
        virtual bool Initialize(int width, int height, const char* title) = 0;
        virtual void Shutdown() = 0;

        // Frame management
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void SwapBuffers() = 0;

        // Clear operations
        virtual void Clear(float r, float g, float b, float a) = 0;
        virtual void ClearDepth() = 0;

        // Window management
        virtual bool ShouldClose() const = 0;
        virtual void SetShouldClose(bool shouldClose) = 0;
        virtual void GetFramebufferSize(int& width, int& height) const = 0;
        virtual void GetWindowSize(int& width, int& height) const = 0;

        // VSync
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSyncEnabled() const = 0;

        // Time
        virtual float GetTime() const = 0;

        // Background color
        virtual void SetBackgroundColor(float r, float g, float b, float a) = 0;

        // Platform-specific native handles (optional, for advanced use)
        virtual void* GetNativeWindowHandle() const { return nullptr; }
        virtual void* GetNativeGraphicsHandle() const { return nullptr; }

        // Viewport
        virtual void SetViewport(int x, int y, int width, int height) = 0;
    };

    /**
     * Graphics Context Factory
     *
     * Creates the appropriate graphics context for the current platform.
     */
    class GraphicsContextFactory
    {
    public:
        static IGraphicsContext* CreateGraphicsContext();
    };
}
