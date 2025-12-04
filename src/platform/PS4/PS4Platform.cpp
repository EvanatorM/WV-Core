#include "PS4Platform.h"
#include "PS4GraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_PS4

// OpenOrbis includes
#include <orbis/Pad.h>
#include <orbis/UserService.h>
#include <orbis/libkernel.h>

namespace WillowVox
{
    PS4Platform::PS4Platform()
    {
    }

    PS4Platform::~PS4Platform()
    {
        Shutdown();
    }

    bool PS4Platform::Initialize()
    {
        Logger::EngineLog("Initializing PS4 Platform (OpenOrbis)");

        // Initialize user service
        int ret = sceUserServiceInitialize(NULL);
        if (ret != 0)
        {
            Logger::EngineError("Failed to initialize user service: 0x%08X", ret);
            return false;
        }

        // Get initial user ID
        ret = sceUserServiceGetInitialUser(&m_userId);
        if (ret != 0)
        {
            Logger::EngineError("Failed to get initial user: 0x%08X", ret);
            return false;
        }

        // Initialize pad library
        ret = scePadInit();
        if (ret != 0)
        {
            Logger::EngineError("Failed to initialize pad: 0x%08X", ret);
            return false;
        }

        // Open pad for user
        m_padHandle = scePadOpen(m_userId, 0, 0, NULL);
        if (m_padHandle < 0)
        {
            Logger::EngineError("Failed to open pad: 0x%08X", m_padHandle);
            return false;
        }

        // Initialize graphics
        m_graphicsContext = std::make_unique<PS4GraphicsContext>();

        Logger::EngineLog("PS4 Platform initialized (User ID: %d, Pad: %d)", m_userId, m_padHandle);
        return true;
    }

    void PS4Platform::Shutdown()
    {
        if (m_padHandle >= 0)
        {
            scePadClose(m_padHandle);
            m_padHandle = -1;
        }

        m_graphicsContext.reset();

        Logger::EngineLog("PS4 Platform shutdown");
    }

    void PS4Platform::ProcessEvents()
    {
        // PS4 doesn't have traditional event polling like desktop
        // Events are handled through system callbacks
    }

    void PS4Platform::PollInput(InputState& outInputState)
    {
        UpdateGamepadInput(outInputState);
        outInputState.deviceType = InputDeviceType::PSController;
    }

    void PS4Platform::UpdateGamepadInput(InputState& outInputState)
    {
        if (m_padHandle < 0)
            return;

        // Store previous button state
        m_prevButtons = m_buttons;

        // Read pad data
        ScePadData padData;
        int ret = scePadReadState(m_padHandle, &padData);

        if (ret == 0 && padData.connected)
        {
            // Get button states
            m_buttons = padData.buttons;

            // Get analog stick values
            m_lstickX = padData.leftStick.x;
            m_lstickY = padData.leftStick.y;
            m_rstickX = padData.rightStick.x;
            m_rstickY = padData.rightStick.y;

            // Trigger values
            m_l2Value = padData.analogButtons.l2;
            m_r2Value = padData.analogButtons.r2;
        }
        else
        {
            // Controller disconnected, reset to defaults
            m_buttons = 0;
            m_lstickX = 128;
            m_lstickY = 128;
            m_rstickX = 128;
            m_rstickY = 128;
            m_l2Value = 0;
            m_r2Value = 0;
        }

        // Analog sticks
        outInputState.moveAxisX = NormalizeAxis(m_lstickX);
        outInputState.moveAxisY = -NormalizeAxis(m_lstickY);
        outInputState.lookAxisX = NormalizeAxis(m_rstickX) * 100.0f;
        outInputState.lookAxisY = -NormalizeAxis(m_rstickY) * 100.0f;

        // Button mapping (DualShock 4)
        bool btnCross = (m_buttons & SCE_PAD_BUTTON_CROSS) != 0;
        bool btnCircle = (m_buttons & SCE_PAD_BUTTON_CIRCLE) != 0;
        bool btnSquare = (m_buttons & SCE_PAD_BUTTON_SQUARE) != 0;
        bool btnTriangle = (m_buttons & SCE_PAD_BUTTON_TRIANGLE) != 0;
        bool btnL1 = (m_buttons & SCE_PAD_BUTTON_L1) != 0;
        bool btnR1 = (m_buttons & SCE_PAD_BUTTON_R1) != 0;
        bool btnL2 = m_l2Value > 64;  // Digital threshold for analog trigger
        bool btnR2 = m_r2Value > 64;
        bool btnOptions = (m_buttons & SCE_PAD_BUTTON_OPTIONS) != 0;
        bool btnTouchPad = (m_buttons & SCE_PAD_BUTTON_TOUCH_PAD) != 0;

        // Map to abstract actions
        outInputState.actionsHeld[static_cast<int>(InputAction::Action1)] = btnCross;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action2)] = btnCircle;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action3)] = btnSquare;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action4)] = btnTriangle;
        outInputState.actionsHeld[static_cast<int>(InputAction::Jump)] = btnCross;
        outInputState.actionsHeld[static_cast<int>(InputAction::Crouch)] = btnL2;
        outInputState.actionsHeld[static_cast<int>(InputAction::Sprint)] = btnR2;
        outInputState.actionsHeld[static_cast<int>(InputAction::CycleLeft)] = btnL1;
        outInputState.actionsHeld[static_cast<int>(InputAction::CycleRight)] = btnR1;
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuOpen)] = btnOptions;
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuBack)] = btnCircle;

        // Pressed this frame
        uint32_t pressed = m_buttons & ~m_prevButtons;
        outInputState.actions[static_cast<int>(InputAction::Action1)] = (pressed & SCE_PAD_BUTTON_CROSS) != 0;
        outInputState.actions[static_cast<int>(InputAction::Action2)] = (pressed & SCE_PAD_BUTTON_CIRCLE) != 0;
        outInputState.actions[static_cast<int>(InputAction::MenuOpen)] = (pressed & SCE_PAD_BUTTON_OPTIONS) != 0;

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

    float PS4Platform::NormalizeAxis(uint8_t value) const
    {
        // PS4 pad axes are 0-255, center at 128
        float normalized = (static_cast<float>(value) - 128.0f) / 128.0f;

        // Apply deadzone
        const float deadzone = 0.15f;
        if (std::abs(normalized) < deadzone)
            return 0.0f;

        return normalized;
    }

    void PS4Platform::ResetInputFrameState(InputState& inputState)
    {
        inputState.ResetFrameStates();
    }

    IGraphicsContext* PS4Platform::GetGraphicsContext()
    {
        return m_graphicsContext.get();
    }

    const char* PS4Platform::GetUserDataPath() const
    {
        return "/data/WillowVox/userdata";
    }

    const char* PS4Platform::GetAssetsPath() const
    {
        return "/app0/assets";
    }

    const char* PS4Platform::GetPlatformName() const
    {
        return "PlayStation 4";
    }

    InputDeviceType PS4Platform::GetPrimaryInputDevice() const
    {
        return InputDeviceType::PSController;
    }

    bool PS4Platform::HasFeature(const char* featureName) const
    {
        if (strcmp(featureName, "gamepad") == 0) return true;
        if (strcmp(featureName, "filesystem") == 0) return true;
        return false;
    }

    void PS4Platform::SetVibration(int playerIndex, float lowFrequency, float highFrequency)
    {
        if (m_padHandle < 0)
            return;

        // PS4 uses scePadSetVibration
        // Values are 0-255
        ScePadVibrationParam vibParam;
        vibParam.largeMotor = static_cast<uint8_t>(lowFrequency * 255.0f);
        vibParam.smallMotor = static_cast<uint8_t>(highFrequency * 255.0f);

        scePadSetVibration(m_padHandle, &vibParam);
    }

    void PS4Platform::StopVibration(int playerIndex)
    {
        SetVibration(playerIndex, 0.0f, 0.0f);
    }
}

#endif // PLATFORM_PS4
