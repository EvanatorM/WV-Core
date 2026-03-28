#include "WiiUPlatform.h"
#include "WiiUGraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_WIIU

// wut library includes (Wii U homebrew)
#include <vpad/input.h>
#include <padscore/kpad.h>
#include <coreinit/time.h>
#include <cmath>

namespace WillowVox
{
    WiiUPlatform::WiiUPlatform()
    {
    }

    WiiUPlatform::~WiiUPlatform()
    {
        Shutdown();
    }

    bool WiiUPlatform::Initialize()
    {
        Logger::EngineLog("Initializing Wii U Platform (wut)");

        // Initialize VPAD (GamePad)
        VPADInit();

        // Initialize KPAD (Pro Controller / Wiimote)
        KPADInit();

        // Initialize graphics
        m_graphicsContext = std::make_unique<WiiUGraphicsContext>();

        Logger::EngineLog("Wii U Platform initialized");
        return true;
    }

    void WiiUPlatform::Shutdown()
    {
        KPADShutdown();
        VPADShutdown();
        m_graphicsContext.reset();
        Logger::EngineLog("Wii U Platform shutdown");
    }

    void WiiUPlatform::ProcessEvents()
    {
        // Wii U doesn't have traditional event polling
        // Input is polled directly via VPADRead()
    }

    void WiiUPlatform::PollInput(InputState& outInputState)
    {
        UpdateGamepadInput(outInputState);
        outInputState.deviceType = InputDeviceType::WiiUGamePad;
    }

    void WiiUPlatform::UpdateGamepadInput(InputState& outInputState)
    {
        // Store previous button state
        m_prevButtons = m_buttons;

        // Read GamePad data (VPAD)
        VPADStatus vpadStatus;
        VPADReadError error;
        VPADRead(VPAD_CHAN_0, &vpadStatus, 1, &error);

        if (error == VPAD_READ_SUCCESS)
        {
            // Store button state
            m_buttons = vpadStatus.hold;

            // Read analog sticks
            m_lstickX = NormalizeAxis(vpadStatus.leftStick.x, -1.0f, 1.0f);
            m_lstickY = NormalizeAxis(vpadStatus.leftStick.y, -1.0f, 1.0f);
            m_rstickX = NormalizeAxis(vpadStatus.rightStick.x, -1.0f, 1.0f);
            m_rstickY = NormalizeAxis(vpadStatus.rightStick.y, -1.0f, 1.0f);
        }
        else
        {
            // No input, reset to defaults
            m_buttons = 0;
            m_lstickX = 0.0f;
            m_lstickY = 0.0f;
            m_rstickX = 0.0f;
            m_rstickY = 0.0f;
        }

        // Map analog sticks
        outInputState.moveAxisX = m_lstickX;
        outInputState.moveAxisY = m_lstickY;
        outInputState.lookAxisX = m_rstickX * 100.0f;
        outInputState.lookAxisY = -m_rstickY * 100.0f; // Invert Y for camera

        // Button mapping (Wii U GamePad - similar to Pro Controller)
        bool btnA = (m_buttons & VPAD_BUTTON_A) != 0;
        bool btnB = (m_buttons & VPAD_BUTTON_B) != 0;
        bool btnX = (m_buttons & VPAD_BUTTON_X) != 0;
        bool btnY = (m_buttons & VPAD_BUTTON_Y) != 0;
        bool btnL = (m_buttons & VPAD_BUTTON_L) != 0;
        bool btnR = (m_buttons & VPAD_BUTTON_R) != 0;
        bool btnZL = (m_buttons & VPAD_BUTTON_ZL) != 0;
        bool btnZR = (m_buttons & VPAD_BUTTON_ZR) != 0;
        bool btnPlus = (m_buttons & VPAD_BUTTON_PLUS) != 0;
        bool btnMinus = (m_buttons & VPAD_BUTTON_MINUS) != 0;
        bool btnHome = (m_buttons & VPAD_BUTTON_HOME) != 0;
        bool btnDpadUp = (m_buttons & VPAD_BUTTON_UP) != 0;
        bool btnDpadDown = (m_buttons & VPAD_BUTTON_DOWN) != 0;
        bool btnDpadLeft = (m_buttons & VPAD_BUTTON_LEFT) != 0;
        bool btnDpadRight = (m_buttons & VPAD_BUTTON_RIGHT) != 0;

        // Map to abstract actions
        outInputState.actionsHeld[static_cast<int>(InputAction::Action1)] = btnA;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action2)] = btnB;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action3)] = btnX;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action4)] = btnY;
        outInputState.actionsHeld[static_cast<int>(InputAction::Jump)] = btnA;
        outInputState.actionsHeld[static_cast<int>(InputAction::Crouch)] = btnZL;
        outInputState.actionsHeld[static_cast<int>(InputAction::Sprint)] = btnZR;
        outInputState.actionsHeld[static_cast<int>(InputAction::CycleLeft)] = btnL;
        outInputState.actionsHeld[static_cast<int>(InputAction::CycleRight)] = btnR;
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuOpen)] = btnPlus || btnHome;
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuBack)] = btnB || btnMinus;

        // D-pad for movement
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveForward)] = btnDpadUp;
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveBackward)] = btnDpadDown;
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveLeft)] = btnDpadLeft;
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveRight)] = btnDpadRight;

        // Pressed this frame
        uint32_t pressed = m_buttons & ~m_prevButtons;
        outInputState.actions[static_cast<int>(InputAction::Action1)] = (pressed & VPAD_BUTTON_A) != 0;
        outInputState.actions[static_cast<int>(InputAction::Action2)] = (pressed & VPAD_BUTTON_B) != 0;
        outInputState.actions[static_cast<int>(InputAction::MenuOpen)] = (pressed & VPAD_BUTTON_PLUS) != 0;
        outInputState.actions[static_cast<int>(InputAction::Jump)] = (pressed & VPAD_BUTTON_A) != 0;

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

    float WiiUPlatform::NormalizeAxis(float value, float min, float max) const
    {
        // VPAD sticks are already -1.0 to 1.0
        // Apply deadzone
        const float deadzone = 0.15f;
        if (std::abs(value) < deadzone)
            return 0.0f;

        // Rescale to account for deadzone
        float sign = (value > 0.0f) ? 1.0f : -1.0f;
        float absValue = std::abs(value);
        return sign * ((absValue - deadzone) / (1.0f - deadzone));
    }

    void WiiUPlatform::ResetInputFrameState(InputState& inputState)
    {
        inputState.ResetFrameStates();
    }

    IGraphicsContext* WiiUPlatform::GetGraphicsContext()
    {
        return m_graphicsContext.get();
    }

    const char* WiiUPlatform::GetUserDataPath() const
    {
        return "fs:/vol/external01/WillowVox/userdata";
    }

    const char* WiiUPlatform::GetAssetsPath() const
    {
        return "fs:/vol/content/WillowVox/assets";
    }

    const char* WiiUPlatform::GetPlatformName() const
    {
        return "Nintendo Wii U";
    }

    InputDeviceType WiiUPlatform::GetPrimaryInputDevice() const
    {
        return InputDeviceType::WiiUGamePad;
    }

    bool WiiUPlatform::HasFeature(const char* featureName) const
    {
        if (strcmp(featureName, "gamepad") == 0) return true;
        if (strcmp(featureName, "touchscreen") == 0) return true; // GamePad has touchscreen
        if (strcmp(featureName, "filesystem") == 0) return true;
        return false;
    }
}

#endif // PLATFORM_WIIU
