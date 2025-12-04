#pragma once

#include <wv/platform/IPlatform.h>
#include <memory>

namespace WillowVox
{
    class XboxSeriesGraphicsContext;

    /**
     * Xbox Series X/S Platform (UWP with XInput)
     *
     * Controller: Xbox Series Controller (standard Xbox layout)
     * - Left Stick: Movement
     * - Right Stick: Look
     * - A: Action1 / Jump
     * - B: Action2 / MenuBack
     * - X: Action3
     * - Y: Action4
     * - LB/RB: Cycle items
     * - LT/RT: Crouch/Sprint
     * - Menu (≡): MenuOpen
     * - View (::): MenuBack
     *
     * Graphics: OpenGL ES via ANGLE (translates to DirectX)
     * Input: XInput API
     */
    class XboxSeriesPlatform : public IPlatform
    {
    public:
        XboxSeriesPlatform();
        ~XboxSeriesPlatform() override;

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
        std::unique_ptr<XboxSeriesGraphicsContext> m_graphicsContext;

        // XInput gamepad state
        uint16_t m_buttons = 0;
        uint16_t m_prevButtons = 0;
        float m_lstickX = 0.0f;
        float m_lstickY = 0.0f;
        float m_rstickX = 0.0f;
        float m_rstickY = 0.0f;
        float m_leftTrigger = 0.0f;
        float m_rightTrigger = 0.0f;

        void UpdateGamepadInput(InputState& outInputState);
        float NormalizeAxis(int16_t value) const;
        float NormalizeTrigger(uint8_t value) const;
    };
}
