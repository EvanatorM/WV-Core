#pragma once

#include <wv/platform/IPlatform.h>
#include <memory>

namespace WillowVox
{
    class SwitchGraphicsContext;

    /**
     * Nintendo Switch Platform Implementation (libnx Homebrew)
     *
     * Controller Mapping (Joy-Con / Pro Controller):
     * - Left Stick: Movement (MoveAxisX/Y)
     * - Right Stick: Look (LookAxisX/Y)
     * - A: Action1
     * - B: Action2 / MenuBack
     * - X: Action3
     * - Y: Action4
     * - L: CycleLeft
     * - R: CycleRight
     * - ZL: Crouch
     * - ZR: Sprint
     * - Plus (+): MenuOpen
     * - Minus (-): MenuBack
     *
     * Uses: libnx (devkitA64), OpenGL for graphics, hidpad for input
     */
    class SwitchPlatform : public IPlatform
    {
    public:
        SwitchPlatform();
        ~SwitchPlatform() override;

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
        std::unique_ptr<SwitchGraphicsContext> m_graphicsContext;

        // Pad state - using char array to avoid including switch.h in header
        // Real type is PadState from libnx
        alignas(8) char m_padStorage[0x400];  // Storage for PadState

        // HID pad state (libnx)
        uint64_t m_buttons = 0;
        uint64_t m_prevButtons = 0;
        int32_t m_lstickX = 0;
        int32_t m_lstickY = 0;
        int32_t m_rstickX = 0;
        int32_t m_rstickY = 0;

        void UpdateGamepadInput(InputState& outInputState);
        float NormalizeAxis(int32_t value) const;
    };
}
