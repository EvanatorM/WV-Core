#pragma once

/**
 * Platform-Agnostic Entry Point
 *
 * This header provides the correct entry point for each platform.
 * Game code should include this instead of defining main() manually.
 *
 * Usage:
 *   #include <wv/app/PlatformEntryPoint.h>
 *
 *   WillowVox::App* WillowVox::CreateApp()
 *   {
 *       return new MyGameApp();
 *   }
 */

#include <wv/app/App.h>
#include <wv/platform/PlatformConfig.h>

// Forward declare CreateApp
extern WillowVox::App* WillowVox::CreateApp();

// ============================================================================
// Desktop Platforms (Windows, Linux, macOS)
// ============================================================================

#if defined(PLATFORM_DESKTOP)

int main(int argc, char** argv)
{
    auto app = WillowVox::CreateApp();
    app->Run();
    delete app;
    return 0;
}

// ============================================================================
// Android
// ============================================================================

#elif defined(PLATFORM_ANDROID)

#include <android_native_app_glue.h>

void android_main(struct android_app* state)
{
    // Set up the android app state
    // The platform implementation will handle this

    auto app = WillowVox::CreateApp();

    // Android apps need to be tied to the android_app state
    // This is handled internally by AndroidPlatform

    app->Run();
    delete app;
}

// ============================================================================
// iOS
// ============================================================================

#elif defined(PLATFORM_IOS)

// iOS uses UIApplicationMain, but we can provide a standard main()
// The actual setup happens in the platform layer

int main(int argc, char** argv)
{
    // iOS-specific initialization happens in the platform layer
    auto app = WillowVox::CreateApp();
    app->Run();
    delete app;
    return 0;
}

// ============================================================================
// PlayStation 3
// ============================================================================

#elif defined(PLATFORM_PS3)

// PS3 homebrew standard main entry point
int main(int argc, char** argv)
{
    auto app = WillowVox::CreateApp();
    app->Run();
    delete app;
    return 0;
}

// ============================================================================
// PlayStation 4
// ============================================================================

#elif defined(PLATFORM_PS4)

// PS4 homebrew main entry point
int main(int argc, char** argv)
{
    auto app = WillowVox::CreateApp();
    app->Run();
    delete app;
    return 0;
}

// ============================================================================
// Nintendo Wii
// ============================================================================

#elif defined(PLATFORM_WII)

// Wii homebrew main entry point (devkitPPC)
int main(int argc, char** argv)
{
    auto app = WillowVox::CreateApp();
    app->Run();
    delete app;
    return 0;
}

// ============================================================================
// Nintendo GameCube
// ============================================================================

#elif defined(PLATFORM_GAMECUBE)

// GameCube homebrew main entry point (devkitPPC)
int main(int argc, char** argv)
{
    auto app = WillowVox::CreateApp();
    app->Run();
    delete app;
    return 0;
}

// ============================================================================
// Nintendo Wii U
// ============================================================================

#elif defined(PLATFORM_WIIU)

// Wii U homebrew main entry point (devkitPPC + wut)
int main(int argc, char** argv)
{
    auto app = WillowVox::CreateApp();
    app->Run();
    delete app;
    return 0;
}

// ============================================================================
// Nintendo Switch
// ============================================================================

#elif defined(PLATFORM_SWITCH)

// Switch homebrew main entry point (devkitA64 + libnx)
int main(int argc, char** argv)
{
    auto app = WillowVox::CreateApp();
    app->Run();
    delete app;
    return 0;
}

// ============================================================================
// Xbox Series X/S (Dev Mode / GDK)
// ============================================================================

#elif defined(PLATFORM_XBOX_SERIES_DEV)

// Xbox uses a different entry point for UWP/GDK
// This may need adjustment based on the specific Xbox SDK being used

int main(int argc, char** argv)
{
    auto app = WillowVox::CreateApp();
    app->Run();
    delete app;
    return 0;
}

// For full GDK, might need:
// int __cdecl main(Platform::Array<Platform::String^>^ args)
// {
//     auto app = WillowVox::CreateApp();
//     app->Run();
//     delete app;
//     return 0;
// }

#else
    #error "Unsupported platform for entry point!"
#endif
