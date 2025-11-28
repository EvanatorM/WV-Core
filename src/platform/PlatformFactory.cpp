#include <wv/platform/IPlatform.h>
#include <wv/platform/IGraphicsContext.h>
#include <wv/platform/PlatformConfig.h>

// Include platform-specific implementations
#if defined(PLATFORM_DESKTOP)
    #include "Desktop/DesktopPlatform.h"
    #include "Desktop/DesktopGraphicsContext.h"
#elif defined(PLATFORM_ANDROID)
    #include "Android/AndroidPlatform.h"
    #include "Android/AndroidGraphicsContext.h"
#elif defined(PLATFORM_IOS)
    #include "iOS/iOSPlatform.h"
    #include "iOS/iOSGraphicsContext.h"
#elif defined(PLATFORM_PS3)
    #include "PS3/PS3Platform.h"
    #include "PS3/PS3GraphicsContext.h"
#elif defined(PLATFORM_PS4)
    #include "PS4/PS4Platform.h"
    #include "PS4/PS4GraphicsContext.h"
#elif defined(PLATFORM_WII)
    #include "Nintendo/WiiPlatform.h"
    #include "Nintendo/WiiGraphicsContext.h"
#elif defined(PLATFORM_GAMECUBE)
    #include "Nintendo/GameCubePlatform.h"
    #include "Nintendo/GameCubeGraphicsContext.h"
#elif defined(PLATFORM_WIIU)
    #include "Nintendo/WiiUPlatform.h"
    #include "Nintendo/WiiUGraphicsContext.h"
#elif defined(PLATFORM_SWITCH)
    #include "Nintendo/SwitchPlatform.h"
    #include "Nintendo/SwitchGraphicsContext.h"
#elif defined(PLATFORM_XBOX_SERIES_DEV)
    #include "Xbox/XboxSeriesPlatform.h"
    #include "Xbox/XboxSeriesGraphicsContext.h"
#endif

namespace WillowVox
{
    IPlatform* PlatformFactory::CreatePlatform()
    {
#if defined(PLATFORM_DESKTOP)
        return new DesktopPlatform();
#elif defined(PLATFORM_ANDROID)
        return new AndroidPlatform();
#elif defined(PLATFORM_IOS)
        return new iOSPlatform();
#elif defined(PLATFORM_PS3)
        return new PS3Platform();
#elif defined(PLATFORM_PS4)
        return new PS4Platform();
#elif defined(PLATFORM_WII)
        return new WiiPlatform();
#elif defined(PLATFORM_GAMECUBE)
        return new GameCubePlatform();
#elif defined(PLATFORM_WIIU)
        return new WiiUPlatform();
#elif defined(PLATFORM_SWITCH)
        return new SwitchPlatform();
#elif defined(PLATFORM_XBOX_SERIES_DEV)
        return new XboxSeriesPlatform();
#else
        #error "Unsupported platform!"
        return nullptr;
#endif
    }

    IGraphicsContext* GraphicsContextFactory::CreateGraphicsContext()
    {
#if defined(PLATFORM_DESKTOP)
        return new DesktopGraphicsContext();
#elif defined(PLATFORM_ANDROID)
        return new AndroidGraphicsContext();
#elif defined(PLATFORM_IOS)
        return new iOSGraphicsContext();
#elif defined(PLATFORM_PS3)
        return new PS3GraphicsContext();
#elif defined(PLATFORM_PS4)
        return new PS4GraphicsContext();
#elif defined(PLATFORM_WII)
        return new WiiGraphicsContext();
#elif defined(PLATFORM_GAMECUBE)
        return new GameCubeGraphicsContext();
#elif defined(PLATFORM_WIIU)
        return new WiiUGraphicsContext();
#elif defined(PLATFORM_SWITCH)
        return new SwitchGraphicsContext();
#elif defined(PLATFORM_XBOX_SERIES_DEV)
        return new XboxSeriesGraphicsContext();
#else
        #error "Unsupported platform!"
        return nullptr;
#endif
    }
}
