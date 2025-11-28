#pragma once

#include <wv/platform/PlatformConfig.h>
#include <wv/platform/InputState.h>
#include <wv/platform/IGraphicsContext.h>

namespace WillowVox
{
    /**
     * Abstract Platform Interface
     *
     * This interface defines the contract that each platform must implement.
     * It handles platform-specific initialization, input polling, events,
     * and any OS-level operations.
     *
     * The platform layer is responsible for:
     * - Creating and managing the graphics context
     * - Polling and translating input to InputState
     * - File system access
     * - Platform-specific main loop integration
     */
    class IPlatform
    {
    public:
        virtual ~IPlatform() = default;

        // Platform lifecycle
        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;

        // Input
        virtual void PollInput(InputState& outInputState) = 0;
        virtual void ResetInputFrameState(InputState& inputState) = 0;

        // Events (platform may have specific event handling)
        virtual void ProcessEvents() = 0;

        // Graphics context access
        virtual IGraphicsContext* GetGraphicsContext() = 0;

        // File system
        virtual const char* GetUserDataPath() const = 0;
        virtual const char* GetAssetsPath() const = 0;

        // Platform info
        virtual const char* GetPlatformName() const = 0;
        virtual InputDeviceType GetPrimaryInputDevice() const = 0;

        // Special platform features
        virtual bool HasFeature(const char* featureName) const = 0;

        // Optional: Some platforms may need special per-frame updates
        virtual void Update(float deltaTime) {}
    };

    /**
     * Platform Factory
     *
     * Creates the appropriate platform implementation for the current platform.
     */
    class PlatformFactory
    {
    public:
        static IPlatform* CreatePlatform();
    };

    /**
     * Platform Entry Point Helpers
     *
     * Different platforms have different entry points and main loop requirements.
     * These macros help define the correct entry point for each platform.
     */

    // Forward declare App
    class App;
    App* CreateApp();

    // Desktop platforms (Windows, Linux, macOS)
    #if defined(PLATFORM_DESKTOP)
        #define WV_PLATFORM_MAIN() \
            int main(int argc, char** argv)

    // Android
    #elif defined(PLATFORM_ANDROID)
        #define WV_PLATFORM_MAIN() \
            void android_main(struct android_app* state)

    // iOS
    #elif defined(PLATFORM_IOS)
        #define WV_PLATFORM_MAIN() \
            int main(int argc, char** argv)

    // PS3
    #elif defined(PLATFORM_PS3)
        #define WV_PLATFORM_MAIN() \
            int main(int argc, char** argv)

    // PS4
    #elif defined(PLATFORM_PS4)
        #define WV_PLATFORM_MAIN() \
            int main(int argc, char** argv)

    // Wii / GameCube
    #elif defined(PLATFORM_WII) || defined(PLATFORM_GAMECUBE)
        #define WV_PLATFORM_MAIN() \
            int main(int argc, char** argv)

    // Wii U
    #elif defined(PLATFORM_WIIU)
        #define WV_PLATFORM_MAIN() \
            int main(int argc, char** argv)

    // Nintendo Switch
    #elif defined(PLATFORM_SWITCH)
        #define WV_PLATFORM_MAIN() \
            int main(int argc, char** argv)

    // Xbox Series X/S Dev Mode
    #elif defined(PLATFORM_XBOX_SERIES_DEV)
        #define WV_PLATFORM_MAIN() \
            int __cdecl main(Platform::Array<Platform::String^>^ args)

    #else
        #error "Unsupported platform for entry point!"
    #endif
}
