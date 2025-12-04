#pragma once

#include <wv/platform/IPlatform.h>
#include "DesktopGraphicsContext.h"
#include <memory>

namespace WillowVox
{
    /**
     * Desktop Platform Implementation
     *
     * Implements IPlatform for Windows, Linux, and macOS.
     * Uses GLFW for windowing and input.
     *
     * Input Mapping (Keyboard & Mouse):
     * - WASD: Movement (Forward/Back/Left/Right)
     * - Space/Shift: Up/Down
     * - Mouse: Look
     * - Left Click: Action1 (break block)
     * - Right Click: Action2 (place block)
     * - Middle Click: Action3 (pick block)
     * - ESC: MenuOpen
     * - E/Q: MoveUp/MoveDown
     * - Mouse Scroll: CycleLeft/CycleRight
     */
    class DesktopPlatform : public IPlatform
    {
    public:
        DesktopPlatform();
        ~DesktopPlatform() override;

        // IPlatform implementation
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
        std::unique_ptr<DesktopGraphicsContext> m_graphicsContext;

        // Input tracking
        bool m_keyStates[512] = {};
        bool m_keyPrevStates[512] = {};
        bool m_mouseButtonStates[8] = {};
        bool m_mouseButtonPrevStates[8] = {};
        double m_mouseX = 0.0;
        double m_mouseY = 0.0;
        double m_prevMouseX = 0.0;
        double m_prevMouseY = 0.0;
        double m_scrollX = 0.0;
        double m_scrollY = 0.0;

        // Helper methods
        void UpdateKeyboardInput(InputState& outInputState);
        void UpdateMouseInput(InputState& outInputState);
        void UpdateGamepadInput(InputState& outInputState);
    };
}
