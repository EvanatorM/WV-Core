#include "XboxSeriesPlatform.h"
#include "XboxSeriesGraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_XBOX

// Windows/Xbox includes
#include <Windows.h>
#include <Xinput.h>
#include <cmath>

// Link XInput library
#pragma comment(lib, "xinput.lib")

namespace WillowVox
{
    XboxSeriesPlatform::XboxSeriesPlatform()
    {
    }

    XboxSeriesPlatform::~XboxSeriesPlatform()
    {
        Shutdown();
    }

    bool XboxSeriesPlatform::Initialize()
    {
        Logger::EngineLog("Initializing Xbox Series Platform (UWP)");

        // Initialize graphics context
        m_graphicsContext = std::make_unique<XboxSeriesGraphicsContext>();

        Logger::EngineLog("Xbox Series Platform initialized");
        return true;
    }

    void XboxSeriesPlatform::Shutdown()
    {
        m_graphicsContext.reset();
        Logger::EngineLog("Xbox Series Platform shutdown");
    }

    void XboxSeriesPlatform::ProcessEvents()
    {
        // UWP event processing would be handled by CoreWindow dispatcher
        // This is called from the main game loop
    }

    void XboxSeriesPlatform::PollInput(InputState& outInputState)
    {
        UpdateGamepadInput(outInputState);
        outInputState.deviceType = InputDeviceType::XboxController;
    }

    void XboxSeriesPlatform::UpdateGamepadInput(InputState& outInputState)
    {
        // Store previous button state
        m_prevButtons = m_buttons;

        // Read XInput state (controller 0)
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));

        DWORD result = XInputGetState(0, &state);

        if (result == ERROR_SUCCESS)
        {
            // Controller is connected
            XINPUT_GAMEPAD& pad = state.Gamepad;

            // Store button state
            m_buttons = pad.wButtons;

            // Store analog sticks
            m_lstickX = NormalizeAxis(pad.sThumbLX);
            m_lstickY = NormalizeAxis(pad.sThumbLY);
            m_rstickX = NormalizeAxis(pad.sThumbRX);
            m_rstickY = NormalizeAxis(pad.sThumbRY);

            // Store triggers
            m_leftTrigger = NormalizeTrigger(pad.bLeftTrigger);
            m_rightTrigger = NormalizeTrigger(pad.bRightTrigger);
        }
        else
        {
            // Controller is not connected, reset to defaults
            m_buttons = 0;
            m_lstickX = 0.0f;
            m_lstickY = 0.0f;
            m_rstickX = 0.0f;
            m_rstickY = 0.0f;
            m_leftTrigger = 0.0f;
            m_rightTrigger = 0.0f;
        }

        // Map analog sticks
        outInputState.moveAxisX = m_lstickX;
        outInputState.moveAxisY = m_lstickY;
        outInputState.lookAxisX = m_rstickX * 100.0f;
        outInputState.lookAxisY = -m_rstickY * 100.0f; // Invert Y for camera

        // Button mapping (Xbox controller)
        bool btnA = (m_buttons & XINPUT_GAMEPAD_A) != 0;
        bool btnB = (m_buttons & XINPUT_GAMEPAD_B) != 0;
        bool btnX = (m_buttons & XINPUT_GAMEPAD_X) != 0;
        bool btnY = (m_buttons & XINPUT_GAMEPAD_Y) != 0;
        bool btnLB = (m_buttons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
        bool btnRB = (m_buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
        bool btnLT = m_leftTrigger > 0.5f;  // Digital threshold for analog trigger
        bool btnRT = m_rightTrigger > 0.5f;
        bool btnMenu = (m_buttons & XINPUT_GAMEPAD_START) != 0;
        bool btnView = (m_buttons & XINPUT_GAMEPAD_BACK) != 0;
        bool btnDpadUp = (m_buttons & XINPUT_GAMEPAD_DPAD_UP) != 0;
        bool btnDpadDown = (m_buttons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
        bool btnDpadLeft = (m_buttons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
        bool btnDpadRight = (m_buttons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;

        // Map to abstract actions
        outInputState.actionsHeld[static_cast<int>(InputAction::Action1)] = btnA;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action2)] = btnB;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action3)] = btnX;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action4)] = btnY;
        outInputState.actionsHeld[static_cast<int>(InputAction::Jump)] = btnA;
        outInputState.actionsHeld[static_cast<int>(InputAction::Crouch)] = btnLT;
        outInputState.actionsHeld[static_cast<int>(InputAction::Sprint)] = btnRT;
        outInputState.actionsHeld[static_cast<int>(InputAction::CycleLeft)] = btnLB;
        outInputState.actionsHeld[static_cast<int>(InputAction::CycleRight)] = btnRB;
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuOpen)] = btnMenu;
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuBack)] = btnB || btnView;

        // D-pad for movement
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveForward)] = btnDpadUp;
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveBackward)] = btnDpadDown;
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveLeft)] = btnDpadLeft;
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveRight)] = btnDpadRight;

        // Pressed this frame
        uint16_t pressed = m_buttons & ~m_prevButtons;
        outInputState.actions[static_cast<int>(InputAction::Action1)] = (pressed & XINPUT_GAMEPAD_A) != 0;
        outInputState.actions[static_cast<int>(InputAction::Action2)] = (pressed & XINPUT_GAMEPAD_B) != 0;
        outInputState.actions[static_cast<int>(InputAction::MenuOpen)] = (pressed & XINPUT_GAMEPAD_START) != 0;
        outInputState.actions[static_cast<int>(InputAction::Jump)] = (pressed & XINPUT_GAMEPAD_A) != 0;

        // Analog to digital movement
        if (std::abs(outInputState.moveAxisY) > 0.3f)
        {
            if (outInputState.moveAxisY > 0.3f)
                outInputState.actionsHeld[static_cast<int>(InputAction::MoveForward)] = true;
            else
                outInputState.actionsHeld[static_cast<int>(InputAction::MoveBackward)] = true;
        }
        if (std::abs(outInputState.moveAxisX) > 0.3f)
        {
            if (outInputState.moveAxisX > 0.3f)
                outInputState.actionsHeld[static_cast<int>(InputAction::MoveRight)] = true;
            else
                outInputState.actionsHeld[static_cast<int>(InputAction::MoveLeft)] = true;
        }
    }

    float XboxSeriesPlatform::NormalizeAxis(int16_t value) const
    {
        // XInput thumbsticks are -32768 to 32767
        float normalized = static_cast<float>(value) / 32767.0f;

        // Apply deadzone (XInput default is ~7849 / 32767 = 0.24)
        const float deadzone = 0.24f;
        if (std::abs(normalized) < deadzone)
            return 0.0f;

        // Rescale to account for deadzone
        float sign = (normalized > 0.0f) ? 1.0f : -1.0f;
        float absValue = std::abs(normalized);
        return sign * ((absValue - deadzone) / (1.0f - deadzone));
    }

    float XboxSeriesPlatform::NormalizeTrigger(uint8_t value) const
    {
        // XInput triggers are 0-255
        float normalized = static_cast<float>(value) / 255.0f;

        // Apply trigger deadzone (XINPUT_GAMEPAD_TRIGGER_THRESHOLD = 30)
        const float deadzone = 30.0f / 255.0f;
        if (normalized < deadzone)
            return 0.0f;

        // Rescale to account for deadzone
        return (normalized - deadzone) / (1.0f - deadzone);
    }

    void XboxSeriesPlatform::ResetInputFrameState(InputState& inputState)
    {
        inputState.ResetFrameStates();
    }

    IGraphicsContext* XboxSeriesPlatform::GetGraphicsContext()
    {
        return m_graphicsContext.get();
    }

    const char* XboxSeriesPlatform::GetUserDataPath() const
    {
        // UWP apps have local application data folder
        return "LocalState/WillowVox";
    }

    const char* XboxSeriesPlatform::GetAssetsPath() const
    {
        // UWP assets are in the app package
        return "Assets";
    }

    const char* XboxSeriesPlatform::GetPlatformName() const
    {
        return "Xbox Series X|S";
    }

    InputDeviceType XboxSeriesPlatform::GetPrimaryInputDevice() const
    {
        return InputDeviceType::XboxController;
    }

    bool XboxSeriesPlatform::HasFeature(const char* featureName) const
    {
        if (strcmp(featureName, "gamepad") == 0) return true;
        if (strcmp(featureName, "filesystem") == 0) return true;
        return false;
    }

    void XboxSeriesPlatform::SetVibration(int playerIndex, float lowFrequency, float highFrequency)
    {
        if (playerIndex < 0 || playerIndex >= XUSER_MAX_COUNT)
            return;

        // XInput vibration values are 0-65535
        XINPUT_VIBRATION vibration;
        vibration.wLeftMotorSpeed = static_cast<WORD>(lowFrequency * 65535.0f);
        vibration.wRightMotorSpeed = static_cast<WORD>(highFrequency * 65535.0f);

        XInputSetState(playerIndex, &vibration);
    }

    void XboxSeriesPlatform::StopVibration(int playerIndex)
    {
        SetVibration(playerIndex, 0.0f, 0.0f);
    }
}

#endif // PLATFORM_XBOX
