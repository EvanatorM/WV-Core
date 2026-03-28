#pragma once

/**
 * Platform Detection and Configuration
 *
 * This header centralizes all platform detection logic and defines
 * platform-specific macros used throughout the engine.
 *
 * Platform Macros Defined:
 * - Desktop: PLATFORM_WINDOWS, PLATFORM_LINUX, PLATFORM_MACOS
 * - Mobile: PLATFORM_ANDROID, PLATFORM_IOS
 * - Consoles: PLATFORM_PS3, PLATFORM_PS4, PLATFORM_WII, PLATFORM_GAMECUBE,
 *             PLATFORM_WIIU, PLATFORM_SWITCH, PLATFORM_XBOX_SERIES_DEV
 *
 * Platform Categories:
 * - PLATFORM_DESKTOP: Windows, Linux, macOS
 * - PLATFORM_MOBILE: Android, iOS
 * - PLATFORM_CONSOLE: All console platforms
 * - PLATFORM_NINTENDO: Wii, GameCube, Wii U, Switch
 * - PLATFORM_PLAYSTATION: PS3, PS4
 */

// ============================================================================
// Desktop Platforms
// ============================================================================

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__WINDOWS__)
    #ifndef PLATFORM_WINDOWS
        #define PLATFORM_WINDOWS 1
    #endif
    #define PLATFORM_DESKTOP 1
    #define PLATFORM_NAME "Windows"

#elif defined(__linux__) && !defined(__ANDROID__)
    #ifndef PLATFORM_LINUX
        #define PLATFORM_LINUX 1
    #endif
    #define PLATFORM_DESKTOP 1
    #define PLATFORM_NAME "Linux"

#elif defined(__APPLE__) && defined(__MACH__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #ifndef PLATFORM_IOS
            #define PLATFORM_IOS 1
        #endif
        #define PLATFORM_MOBILE 1
        #define PLATFORM_NAME "iOS"
    #else
        #ifndef PLATFORM_MACOS
            #define PLATFORM_MACOS 1
        #endif
        #define PLATFORM_DESKTOP 1
        #define PLATFORM_NAME "macOS"
    #endif

// ============================================================================
// Mobile Platforms
// ============================================================================

#elif defined(__ANDROID__)
    #ifndef PLATFORM_ANDROID
        #define PLATFORM_ANDROID 1
    #endif
    #define PLATFORM_MOBILE 1
    #define PLATFORM_NAME "Android"

// ============================================================================
// PlayStation Consoles (Homebrew)
// ============================================================================

#elif defined(__PPU__) || defined(__CELLOS_LV2__)
    // PS3 (PSL1GHT toolchain)
    #ifndef PLATFORM_PS3
        #define PLATFORM_PS3 1
    #endif
    #define PLATFORM_CONSOLE 1
    #define PLATFORM_PLAYSTATION 1
    #define PLATFORM_NAME "PlayStation 3"

#elif defined(__ORBIS__) || defined(__PS4__)
    // PS4 (OpenOrbis toolchain)
    #ifndef PLATFORM_PS4
        #define PLATFORM_PS4 1
    #endif
    #define PLATFORM_CONSOLE 1
    #define PLATFORM_PLAYSTATION 1
    #define PLATFORM_NAME "PlayStation 4"

// ============================================================================
// Nintendo Consoles (devkitPro Homebrew)
// ============================================================================

#elif defined(__wii__) || defined(HW_RVL)
    // Wii (devkitPPC)
    #ifndef PLATFORM_WII
        #define PLATFORM_WII 1
    #endif
    #define PLATFORM_CONSOLE 1
    #define PLATFORM_NINTENDO 1
    #define PLATFORM_NAME "Nintendo Wii"

#elif defined(__gamecube__) || defined(HW_DOL)
    // GameCube (devkitPPC)
    #ifndef PLATFORM_GAMECUBE
        #define PLATFORM_GAMECUBE 1
    #endif
    #define PLATFORM_CONSOLE 1
    #define PLATFORM_NINTENDO 1
    #define PLATFORM_NAME "Nintendo GameCube"

#elif defined(__WIIU__) || defined(__wiiu__)
    // Wii U (devkitPPC + wut)
    #ifndef PLATFORM_WIIU
        #define PLATFORM_WIIU 1
    #endif
    #define PLATFORM_CONSOLE 1
    #define PLATFORM_NINTENDO 1
    #define PLATFORM_NAME "Nintendo Wii U"

#elif defined(__SWITCH__) || defined(__switch__)
    // Nintendo Switch (devkitA64 + libnx)
    #ifndef PLATFORM_SWITCH
        #define PLATFORM_SWITCH 1
    #endif
    #define PLATFORM_CONSOLE 1
    #define PLATFORM_NINTENDO 1
    #define PLATFORM_NAME "Nintendo Switch"

// ============================================================================
// Xbox (Dev Mode / GDK)
// ============================================================================

#elif defined(_GAMING_XBOX) || defined(_GAMING_XBOX_SCARLETT)
    // Xbox Series X/S in Dev Mode or GDK
    #ifndef PLATFORM_XBOX_SERIES_DEV
        #define PLATFORM_XBOX_SERIES_DEV 1
    #endif
    #define PLATFORM_CONSOLE 1
    #define PLATFORM_XBOX 1
    #define PLATFORM_NAME "Xbox Series X/S (Dev Mode)"

#else
    #error "Unknown or unsupported platform!"
#endif

// ============================================================================
// Platform Feature Detection
// ============================================================================

// Has mouse & keyboard
#if defined(PLATFORM_DESKTOP)
    #define PLATFORM_HAS_MOUSE 1
    #define PLATFORM_HAS_KEYBOARD 1
#endif

// Has touchscreen
#if defined(PLATFORM_MOBILE)
    #define PLATFORM_HAS_TOUCHSCREEN 1
#endif

// Has gamepad/controller
#if defined(PLATFORM_CONSOLE) || defined(PLATFORM_DESKTOP)
    #define PLATFORM_HAS_GAMEPAD 1
#endif

// Has filesystem access
#if !defined(PLATFORM_WEB)
    #define PLATFORM_HAS_FILESYSTEM 1
#endif

// ============================================================================
// Graphics API Hints
// ============================================================================

#if defined(PLATFORM_DESKTOP)
    // Desktop typically uses OpenGL or Vulkan
    #define PLATFORM_GRAPHICS_OPENGL 1
#elif defined(PLATFORM_ANDROID)
    // Android uses OpenGL ES or Vulkan
    #define PLATFORM_GRAPHICS_OPENGL_ES 1
#elif defined(PLATFORM_IOS)
    // iOS uses Metal (but we can also use OpenGL ES compatibility)
    #define PLATFORM_GRAPHICS_METAL 1
    #define PLATFORM_GRAPHICS_OPENGL_ES 1
#elif defined(PLATFORM_SWITCH)
    // Switch uses OpenGL or native NVN
    #define PLATFORM_GRAPHICS_OPENGL 1
#elif defined(PLATFORM_PS3)
    // PS3 uses GCM or PSGL (OpenGL subset)
    #define PLATFORM_GRAPHICS_GCM 1
#elif defined(PLATFORM_PS4)
    // PS4 uses GNM
    #define PLATFORM_GRAPHICS_GNM 1
#elif defined(PLATFORM_WII) || defined(PLATFORM_GAMECUBE)
    // Wii/GC use GX
    #define PLATFORM_GRAPHICS_GX 1
#elif defined(PLATFORM_WIIU)
    // Wii U uses GX2
    #define PLATFORM_GRAPHICS_GX2 1
#elif defined(PLATFORM_XBOX_SERIES_DEV)
    // Xbox uses DirectX 12
    #define PLATFORM_GRAPHICS_DX12 1
#endif

// ============================================================================
// Compiler Attributes
// ============================================================================

#if defined(__GNUC__) || defined(__clang__)
    #define WV_FORCE_INLINE __attribute__((always_inline)) inline
    #define WV_NO_INLINE __attribute__((noinline))
#elif defined(_MSC_VER)
    #define WV_FORCE_INLINE __forceinline
    #define WV_NO_INLINE __declspec(noinline)
#else
    #define WV_FORCE_INLINE inline
    #define WV_NO_INLINE
#endif

// ============================================================================
// Debug/Release Configuration
// ============================================================================

#if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
    #define WV_DEBUG 1
    #define WV_RELEASE 0
#else
    #define WV_DEBUG 0
    #define WV_RELEASE 1
#endif

// ============================================================================
// Platform-Specific Includes
// ============================================================================

// Standard library availability varies by platform
#if defined(PLATFORM_PS3) || defined(PLATFORM_WII) || defined(PLATFORM_GAMECUBE)
    // Some older consoles have limited standard library support
    #define WV_LIMITED_STL 1
#endif
