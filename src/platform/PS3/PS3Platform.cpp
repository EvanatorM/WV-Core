#include "PS3Platform.h"
#include "PS3GraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_PS3

// PSL1GHT includes (homebrew SDK)
#include <io/pad.h>
#include <sysutil/sysutil.h>
#include <sys/time.h>

// PSL1GHT pad button constants (from io/pad.h)
#define PAD_BTN_SELECT      0x0001
#define PAD_BTN_L3          0x0002
#define PAD_BTN_R3          0x0004
#define PAD_BTN_START       0x0008
#define PAD_BTN_UP          0x0010
#define PAD_BTN_RIGHT       0x0020
#define PAD_BTN_DOWN        0x0040
#define PAD_BTN_LEFT        0x0080
#define PAD_BTN_L2          0x0100
#define PAD_BTN_R2          0x0200
#define PAD_BTN_L1          0x0400
#define PAD_BTN_R1          0x0800
#define PAD_BTN_TRIANGLE    0x1000
#define PAD_BTN_CIRCLE      0x2000
#define PAD_BTN_CROSS       0x4000
#define PAD_BTN_SQUARE      0x8000

namespace WillowVox
{
    PS3Platform::PS3Platform()
    {
        m_padState = {};
    }

    PS3Platform::~PS3Platform()
    {
        Shutdown();
    }

    bool PS3Platform::Initialize()
    {
        Logger::EngineLog("Initializing PS3 Platform (PSL1GHT Homebrew)");

        // Initialize pad library (max 7 pads)
        if (ioPadInit(7) != 0)
        {
            Logger::EngineError("Failed to initialize PS3 pad library!");
            return false;
        }

        // Initialize graphics
        m_graphicsContext = std::make_unique<PS3GraphicsContext>();

        Logger::EngineLog("PS3 Platform initialized");
        return true;
    }

    void PS3Platform::Shutdown()
    {
        m_graphicsContext.reset();

        // Shutdown pad
        ioPadEnd();

        Logger::EngineLog("PS3 Platform shutdown");
    }

    void PS3Platform::ProcessEvents()
    {
        // PS3 homebrew doesn't have a traditional event loop
        // Events are polled directly via ioPad, sysutil, etc.

        // Handle system utility callbacks (important for XMB interaction)
        sysUtilCheckCallback();
    }

    void PS3Platform::PollInput(InputState& outInputState)
    {
        UpdateGamepadInput(outInputState);
        outInputState.deviceType = InputDeviceType::PSController;
    }

    void PS3Platform::UpdateGamepadInput(InputState& outInputState)
    {
        // Store previous button state
        m_padState.prevButtons = m_padState.buttons;

        // Read pad data from PSL1GHT
        padInfo padinfo;
        ioPadGetInfo(&padinfo);

        // Reset to defaults
        m_padState.buttons = 0;
        m_padState.lstickX = 128;
        m_padState.lstickY = 128;
        m_padState.rstickX = 128;
        m_padState.rstickY = 128;

        // Check if pad 0 is connected
        if (padinfo.status[0])
        {
            padData paddata;
            if (ioPadGetData(0, &paddata) == 0 && paddata.len > 0)
            {
                // Button states (combine all buttons into one field)
                m_padState.buttons = paddata.button[2] | (paddata.button[3] << 8);

                // Analog stick values (0-255, center at 128)
                m_padState.lstickX = paddata.ANA_L_H;
                m_padState.lstickY = paddata.ANA_L_V;
                m_padState.rstickX = paddata.ANA_R_H;
                m_padState.rstickY = paddata.ANA_R_V;
            }
        }

        // Analog sticks
        outInputState.moveAxisX = NormalizeAxis(m_padState.lstickX);
        outInputState.moveAxisY = -NormalizeAxis(m_padState.lstickY);
        outInputState.lookAxisX = NormalizeAxis(m_padState.rstickX) * 100.0f;
        outInputState.lookAxisY = -NormalizeAxis(m_padState.rstickY) * 100.0f;

        // Button mapping
        bool btnCross = (m_padState.buttons & PAD_BTN_CROSS) != 0;
        bool btnCircle = (m_padState.buttons & PAD_BTN_CIRCLE) != 0;
        bool btnSquare = (m_padState.buttons & PAD_BTN_SQUARE) != 0;
        bool btnTriangle = (m_padState.buttons & PAD_BTN_TRIANGLE) != 0;
        bool btnL1 = (m_padState.buttons & PAD_BTN_L1) != 0;
        bool btnR1 = (m_padState.buttons & PAD_BTN_R1) != 0;
        bool btnL2 = (m_padState.buttons & PAD_BTN_L2) != 0;
        bool btnR2 = (m_padState.buttons & PAD_BTN_R2) != 0;
        bool btnStart = (m_padState.buttons & PAD_BTN_START) != 0;
        bool btnSelect = (m_padState.buttons & PAD_BTN_SELECT) != 0;

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
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuOpen)] = btnStart;
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuBack)] = btnSelect || btnCircle;

        // Pressed this frame
        uint16_t pressed = m_padState.buttons & ~m_padState.prevButtons;
        outInputState.actions[static_cast<int>(InputAction::Action1)] = (pressed & PAD_BTN_CROSS) != 0;
        outInputState.actions[static_cast<int>(InputAction::Action2)] = (pressed & PAD_BTN_CIRCLE) != 0;
        outInputState.actions[static_cast<int>(InputAction::MenuOpen)] = (pressed & PAD_BTN_START) != 0;

        // Analog to digital movement
        if (std::abs(outInputState.moveAxisY) > 0.3f)
        {
            if (outInputState.moveAxisY > 0.3f)
                outInputState.actionsHeld[static_cast<int>(InputAction::MoveForward)] = true;
            else if (outInputState.moveAxisY < -0.3f)
                outInputState.actionsHeld[static_cast<int>(InputAction::MoveBackward)] = true;
        }
        if (std::abs(outInputState.moveAxisX) > 0.3f)
        {
            if (outInputState.moveAxisX > 0.3f)
                outInputState.actionsHeld[static_cast<int>(InputAction::MoveRight)] = true;
            else if (outInputState.moveAxisX < -0.3f)
                outInputState.actionsHeld[static_cast<int>(InputAction::MoveLeft)] = true;
        }
    }

    float PS3Platform::NormalizeAxis(uint8_t value) const
    {
        // PS3 pad axes are 0-255, center at 128
        float normalized = (static_cast<float>(value) - 128.0f) / 128.0f;

        // Apply deadzone
        const float deadzone = 0.15f;
        if (std::abs(normalized) < deadzone)
            return 0.0f;

        return normalized;
    }

    void PS3Platform::ResetInputFrameState(InputState& inputState)
    {
        inputState.ResetFrameStates();
    }

    IGraphicsContext* PS3Platform::GetGraphicsContext()
    {
        return m_graphicsContext.get();
    }

    const char* PS3Platform::GetUserDataPath() const
    {
        return "/dev_hdd0/game/WVOX00001/USRDIR/userdata";
    }

    const char* PS3Platform::GetAssetsPath() const
    {
        return "/dev_hdd0/game/WVOX00001/USRDIR/assets";
    }

    const char* PS3Platform::GetPlatformName() const
    {
        return "PlayStation 3";
    }

    InputDeviceType PS3Platform::GetPrimaryInputDevice() const
    {
        return InputDeviceType::PSController;
    }

    bool PS3Platform::HasFeature(const char* featureName) const
    {
        if (strcmp(featureName, "gamepad") == 0) return true;
        if (strcmp(featureName, "filesystem") == 0) return true;
        return false;
    }
}

#endif // PLATFORM_PS3
