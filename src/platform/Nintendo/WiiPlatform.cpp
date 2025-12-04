#include "WiiPlatform.h"
#include "WiiGraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_WII

// libogc includes
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <cmath>

namespace WillowVox
{
    WiiPlatform::WiiPlatform()
    {
    }

    WiiPlatform::~WiiPlatform()
    {
        Shutdown();
    }

    bool WiiPlatform::Initialize()
    {
        Logger::EngineLog("Initializing Wii Platform (libogc)");

        // Initialize WPAD (Wiimote)
        WPAD_Init();
        WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
        WPAD_SetVRes(WPAD_CHAN_0, 640, 480);

        // Initialize graphics
        m_graphicsContext = std::make_unique<WiiGraphicsContext>();

        Logger::EngineLog("Wii Platform initialized");
        return true;
    }

    void WiiPlatform::Shutdown()
    {
        WPAD_Shutdown();
        m_graphicsContext.reset();
        Logger::EngineLog("Wii Platform shutdown");
    }

    void WiiPlatform::ProcessEvents()
    {
        // Wii doesn't have traditional event polling
        // Input is polled directly via WPAD_ScanPads()
    }

    void WiiPlatform::PollInput(InputState& outInputState)
    {
        UpdateWiimoteInput(outInputState);
        outInputState.deviceType = InputDeviceType::WiiRemote;
    }

    void WiiPlatform::UpdateWiimoteInput(InputState& outInputState)
    {
        // Store previous button state
        m_prevButtons = m_buttons;

        // Scan for button presses
        WPAD_ScanPads();

        // Read Wiimote data
        u32 pressed = WPAD_ButtonsDown(WPAD_CHAN_0);
        u32 held = WPAD_ButtonsHeld(WPAD_CHAN_0);
        m_buttons = held;

        // Get expansion data
        WPADData* data = WPAD_Data(WPAD_CHAN_0);

        if (data && data->exp.type == WPAD_EXP_NUNCHUK)
        {
            // Nunchuk is connected - use for movement
            joystick_t nunchuk = data->exp.nunchuk.js;
            m_nunchukX = NormalizeAxis(nunchuk.pos.x, nunchuk.center.x);
            m_nunchukY = NormalizeAxis(nunchuk.pos.y, nunchuk.center.y);

            // Nunchuk buttons
            if (data->exp.nunchuk.btns & NUNCHUK_BUTTON_Z)
                m_buttons |= WPAD_BUTTON_HOME; // Map Z to crouch
            if (data->exp.nunchuk.btns & NUNCHUK_BUTTON_C)
                m_buttons |= WPAD_BUTTON_1; // Map C to sprint
        }
        else
        {
            // No Nunchuk - reset analog values
            m_nunchukX = 0.0f;
            m_nunchukY = 0.0f;
        }

        // Get IR pointer data
        if (data && data->ir.valid)
        {
            m_irValid = true;
            m_irX = static_cast<float>(data->ir.x) / 640.0f;
            m_irY = static_cast<float>(data->ir.y) / 480.0f;
        }
        else
        {
            m_irValid = false;
        }

        // Map analog sticks
        outInputState.moveAxisX = m_nunchukX;
        outInputState.moveAxisY = m_nunchukY;

        // IR pointer for look (if valid)
        if (m_irValid)
        {
            // Center IR to -1..1 range
            outInputState.lookAxisX = (m_irX - 0.5f) * 200.0f;
            outInputState.lookAxisY = (m_irY - 0.5f) * 200.0f;
        }

        // Button mapping (Wiimote)
        bool btnA = (m_buttons & WPAD_BUTTON_A) != 0;
        bool btnB = (m_buttons & WPAD_BUTTON_B) != 0;
        bool btn1 = (m_buttons & WPAD_BUTTON_1) != 0;
        bool btn2 = (m_buttons & WPAD_BUTTON_2) != 0;
        bool btnHome = (m_buttons & WPAD_BUTTON_HOME) != 0;
        bool btnPlus = (m_buttons & WPAD_BUTTON_PLUS) != 0;
        bool btnMinus = (m_buttons & WPAD_BUTTON_MINUS) != 0;
        bool btnUp = (m_buttons & WPAD_BUTTON_UP) != 0;
        bool btnDown = (m_buttons & WPAD_BUTTON_DOWN) != 0;
        bool btnLeft = (m_buttons & WPAD_BUTTON_LEFT) != 0;
        bool btnRight = (m_buttons & WPAD_BUTTON_RIGHT) != 0;

        // Map to abstract actions
        outInputState.actionsHeld[static_cast<int>(InputAction::Action1)] = btnA;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action2)] = btnB;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action3)] = btn1;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action4)] = btn2;
        outInputState.actionsHeld[static_cast<int>(InputAction::Jump)] = btnA;
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuOpen)] = btnHome;
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuBack)] = btnB;
        outInputState.actionsHeld[static_cast<int>(InputAction::CycleLeft)] = btnMinus;
        outInputState.actionsHeld[static_cast<int>(InputAction::CycleRight)] = btnPlus;

        // D-pad for movement
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveForward)] = btnUp;
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveBackward)] = btnDown;
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveLeft)] = btnLeft;
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveRight)] = btnRight;

        // Pressed this frame
        outInputState.actions[static_cast<int>(InputAction::Action1)] = (pressed & WPAD_BUTTON_A) != 0;
        outInputState.actions[static_cast<int>(InputAction::Action2)] = (pressed & WPAD_BUTTON_B) != 0;
        outInputState.actions[static_cast<int>(InputAction::MenuOpen)] = (pressed & WPAD_BUTTON_HOME) != 0;
        outInputState.actions[static_cast<int>(InputAction::Jump)] = (pressed & WPAD_BUTTON_A) != 0;

        // Analog to digital movement (Nunchuk)
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

    float WiiPlatform::NormalizeAxis(uint8_t value, uint8_t center) const
    {
        // Nunchuk stick is 0-255, center varies by device
        float normalized = (static_cast<float>(value) - static_cast<float>(center)) / 128.0f;

        // Apply deadzone
        const float deadzone = 0.15f;
        if (std::abs(normalized) < deadzone)
            return 0.0f;

        return normalized;
    }

    void WiiPlatform::ResetInputFrameState(InputState& inputState)
    {
        inputState.ResetFrameStates();
    }

    IGraphicsContext* WiiPlatform::GetGraphicsContext()
    {
        return m_graphicsContext.get();
    }

    const char* WiiPlatform::GetUserDataPath() const
    {
        return "sd:/WillowVox/userdata";
    }

    const char* WiiPlatform::GetAssetsPath() const
    {
        return "sd:/WillowVox/assets";
    }

    const char* WiiPlatform::GetPlatformName() const
    {
        return "Nintendo Wii";
    }

    InputDeviceType WiiPlatform::GetPrimaryInputDevice() const
    {
        return InputDeviceType::WiiRemote;
    }

    bool WiiPlatform::HasFeature(const char* featureName) const
    {
        if (strcmp(featureName, "gamepad") == 0) return true;
        if (strcmp(featureName, "motion") == 0) return true;
        if (strcmp(featureName, "filesystem") == 0) return true;
        return false;
    }
}

#endif // PLATFORM_WII
