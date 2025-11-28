#pragma once

#include <wv/platform/IPlatform.h>
#include <memory>

namespace WillowVox
{
    class PS3GraphicsContext;

    /**
     * PlayStation 3 Platform Implementation (PSL1GHT Homebrew)
     *
     * Controller Mapping (DualShock 3):
     * - Left Stick: Movement (MoveAxisX/Y)
     * - Right Stick: Look (LookAxisX/Y)
     * - Cross (X): Action1 / Jump
     * - Circle (O): Action2 / MenuBack
     * - Square: Action3
     * - Triangle: Action4
     * - L1: CycleLeft
     * - R1: CycleRight
     * - L2: Crouch
     * - R2: Sprint
     * - Start: MenuOpen
     * - Select: MenuBack
     *
     * Uses: libgcm or PSGL for graphics, libpad for input
     */
    class PS3Platform : public IPlatform
    {
    public:
        PS3Platform();
        ~PS3Platform() override;

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
        std::unique_ptr<PS3GraphicsContext> m_graphicsContext;

        // PS3 pad state
        struct PadState
        {
            uint16_t buttons;
            uint16_t prevButtons;
            uint8_t lstickX, lstickY;
            uint8_t rstickX, rstickY;
        } m_padState;

        void UpdateGamepadInput(InputState& outInputState);
        float NormalizeAxis(uint8_t value) const;
    };
}
