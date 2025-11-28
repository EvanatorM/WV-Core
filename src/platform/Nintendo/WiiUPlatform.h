#pragma once

#include <wv/platform/IPlatform.h>
#include <memory>

namespace WillowVox
{
    class WiiUGraphicsContext;

    /**
     * Nintendo Wii U Platform (devkitPPC + wut Homebrew)
     *
     * Controller: Wii U Pro Controller / GamePad (similar to modern gamepad)
     * Graphics: GX2
     * Input: VPAD (GamePad) / WPAD (Wiimote) / KPAD (Pro Controller)
     *
     * Similar button mapping to Switch/modern controllers
     */
    class WiiUPlatform : public IPlatform
    {
    public:
        WiiUPlatform();
        ~WiiUPlatform() override;

        bool Initialize() override;
        void Shutdown() override;
        void PollInput(InputState& outInputState) override;
        void ResetInputFrameState(InputState& inputState) override;
        void ProcessEvents() override;
        IGraphicsContext* GetGraphicsContext() override;
        const char* GetUserDataPath() const override;
        const char* GetAssetsPath() const override;
        const char* GetPlatformName() const override;
        InputDeviceType GetPrimaryInputDevice() const override;
        bool HasFeature(const char* featureName) const override;

    private:
        std::unique_ptr<WiiUGraphicsContext> m_graphicsContext;
    };
}
