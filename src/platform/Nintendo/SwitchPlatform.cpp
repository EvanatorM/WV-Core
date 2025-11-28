#include "SwitchPlatform.h"
#include "SwitchGraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_SWITCH

// libnx includes (devkitA64 homebrew SDK)
#include <switch.h>

namespace WillowVox
{
    SwitchPlatform::SwitchPlatform()
    {
    }

    SwitchPlatform::~SwitchPlatform()
    {
        Shutdown();
    }

    bool SwitchPlatform::Initialize()
    {
        Logger::EngineLog("Initializing Nintendo Switch Platform (libnx v4.0+)");

        // Configure input for single player with standard controller styles
        // HidNpadStyleSet_NpadStandard supports handheld, Joy-Con, and Pro Controller
        padConfigureInput(1, HidNpadStyleSet_NpadStandard);

        // Initialize the default gamepad (reads handheld mode and first connected controller)
        padInitializeDefault(&m_pad);

        // Initialize graphics
        m_graphicsContext = std::make_unique<SwitchGraphicsContext>();

        Logger::EngineLog("Switch Platform initialized");
        return true;
    }

    void SwitchPlatform::Shutdown()
    {
        m_graphicsContext.reset();
        Logger::EngineLog("Switch Platform shutdown");
    }

    void SwitchPlatform::ProcessEvents()
    {
        // Check if we should exit (e.g., user pressed home button)
        if (!appletMainLoop())
        {
            m_graphicsContext->SetShouldClose(true);
        }
    }

    void SwitchPlatform::PollInput(InputState& outInputState)
    {
        UpdateGamepadInput(outInputState);
        outInputState.deviceType = InputDeviceType::SwitchController;
    }

    void SwitchPlatform::UpdateGamepadInput(InputState& outInputState)
    {
        m_prevButtons = m_buttons;

        // Update pad state (must be called once per frame)
        padUpdate(&m_pad);

        // Get button states
        m_buttons = padGetButtons(&m_pad);
        u64 buttonsDown = padGetButtonsDown(&m_pad);

        // Get analog stick positions
        HidAnalogStickState lstick = padGetStickPos(&m_pad, 0);  // Left stick
        HidAnalogStickState rstick = padGetStickPos(&m_pad, 1);  // Right stick

        m_lstickX = lstick.x;
        m_lstickY = lstick.y;
        m_rstickX = rstick.x;
        m_rstickY = rstick.y;

        // Analog axes
        outInputState.moveAxisX = NormalizeAxis(m_lstickX);
        outInputState.moveAxisY = -NormalizeAxis(m_lstickY);
        outInputState.lookAxisX = NormalizeAxis(m_rstickX) * 100.0f;
        outInputState.lookAxisY = -NormalizeAxis(m_rstickY) * 100.0f;

        // Button mapping (using libnx HidNpadButton constants)
        bool btnA = (m_buttons & HidNpadButton_A) != 0;
        bool btnB = (m_buttons & HidNpadButton_B) != 0;
        bool btnX = (m_buttons & HidNpadButton_X) != 0;
        bool btnY = (m_buttons & HidNpadButton_Y) != 0;
        bool btnL = (m_buttons & HidNpadButton_L) != 0;
        bool btnR = (m_buttons & HidNpadButton_R) != 0;
        bool btnZL = (m_buttons & HidNpadButton_ZL) != 0;
        bool btnZR = (m_buttons & HidNpadButton_ZR) != 0;
        bool btnPlus = (m_buttons & HidNpadButton_Plus) != 0;
        bool btnMinus = (m_buttons & HidNpadButton_Minus) != 0;

        outInputState.actionsHeld[static_cast<int>(InputAction::Action1)] = btnA;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action2)] = btnB;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action3)] = btnX;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action4)] = btnY;
        outInputState.actionsHeld[static_cast<int>(InputAction::Jump)] = btnA;
        outInputState.actionsHeld[static_cast<int>(InputAction::Crouch)] = btnZL;
        outInputState.actionsHeld[static_cast<int>(InputAction::Sprint)] = btnZR;
        outInputState.actionsHeld[static_cast<int>(InputAction::CycleLeft)] = btnL;
        outInputState.actionsHeld[static_cast<int>(InputAction::CycleRight)] = btnR;
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuOpen)] = btnPlus;
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuBack)] = btnMinus || btnB;

        // Pressed this frame (using libnx HidNpadButton constants)
        uint64_t pressed = m_buttons & ~m_prevButtons;
        outInputState.actions[static_cast<int>(InputAction::Action1)] = (pressed & HidNpadButton_A) != 0;
        outInputState.actions[static_cast<int>(InputAction::Action2)] = (pressed & HidNpadButton_B) != 0;
        outInputState.actions[static_cast<int>(InputAction::MenuOpen)] = (pressed & HidNpadButton_Plus) != 0;

        // Analog to digital
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

    float SwitchPlatform::NormalizeAxis(int32_t value) const
    {
        // Switch analog sticks return values from -32768 to 32767
        float normalized = static_cast<float>(value) / 32768.0f;

        // Apply deadzone
        const float deadzone = 0.15f;
        if (std::abs(normalized) < deadzone)
            return 0.0f;

        return normalized;
    }

    void SwitchPlatform::ResetInputFrameState(InputState& inputState)
    {
        inputState.ResetFrameStates();
    }

    IGraphicsContext* SwitchPlatform::GetGraphicsContext()
    {
        return m_graphicsContext.get();
    }

    const char* SwitchPlatform::GetUserDataPath() const
    {
        return "/switch/WillowVox/userdata";
    }

    const char* SwitchPlatform::GetAssetsPath() const
    {
        return "/switch/WillowVox/assets";
    }

    const char* SwitchPlatform::GetPlatformName() const
    {
        return "Nintendo Switch";
    }

    InputDeviceType SwitchPlatform::GetPrimaryInputDevice() const
    {
        return InputDeviceType::SwitchController;
    }

    bool SwitchPlatform::HasFeature(const char* featureName) const
    {
        if (strcmp(featureName, "gamepad") == 0) return true;
        if (strcmp(featureName, "filesystem") == 0) return true;
        return false;
    }
}

#endif // PLATFORM_SWITCH
