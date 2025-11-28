#pragma once

#include <wv/platform/IPlatform.h>
#include <memory>

namespace WillowVox
{
    class WiiGraphicsContext;

    /**
     * Nintendo Wii Platform (devkitPPC Homebrew)
     *
     * Controller Mapping (Wiimote + Nunchuk or Classic Controller):
     * - Nunchuk Stick / Classic Left Stick: Movement
     * - Wiimote IR / Classic Right Stick: Look
     * - A / Classic A: Action1
     * - B / Classic B: Action2
     * - 1 / Classic X: Action3
     * - 2 / Classic Y: Action4
     * - Home: MenuOpen
     * - Plus/Minus: Cycle items
     *
     * Graphics: GX (libogc)
     * Input: WPAD (Wiimote) / PAD (GameCube controller)
     */
    class WiiPlatform : public IPlatform
    {
    public:
        WiiPlatform();
        ~WiiPlatform() override;

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
        std::unique_ptr<WiiGraphicsContext> m_graphicsContext;
        // Wii-specific input handling
    };
}
