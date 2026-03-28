#pragma once

#include <wv/platform/IPlatform.h>
#include <memory>

namespace WillowVox
{
    class PS4GraphicsContext;

    /**
     * PlayStation 4 Platform (OpenOrbis Homebrew)
     *
     * Controller: DualShock 4
     * - Cross (X): Action1 / Jump
     * - Circle: Action2 / MenuBack
     * - Square: Action3
     * - Triangle: Action4
     * - L1/R1: Cycle items
     * - L2/R2: Crouch/Sprint
     * - Options: MenuOpen
     * - TouchPad: Special action
     *
     * Graphics: OpenGL via piglet (easier than GNM)
     * Input: libSceUserService + libScePad (OpenOrbis)
     */
    class PS4Platform : public IPlatform
    {
    public:
        PS4Platform();
        ~PS4Platform() override;

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

        void SetVibration(int playerIndex, float lowFrequency, float highFrequency) override;
        void StopVibration(int playerIndex) override;

    private:
        std::unique_ptr<PS4GraphicsContext> m_graphicsContext;

        // PS4-specific handles
        int m_userId = 0;
        int m_padHandle = -1;

        // Pad state
        uint32_t m_buttons = 0;
        uint32_t m_prevButtons = 0;
        uint8_t m_lstickX = 128;
        uint8_t m_lstickY = 128;
        uint8_t m_rstickX = 128;
        uint8_t m_rstickY = 128;
        uint8_t m_l2Value = 0;
        uint8_t m_r2Value = 0;

        void UpdateGamepadInput(InputState& outInputState);
        float NormalizeAxis(uint8_t value) const;
    };
}
