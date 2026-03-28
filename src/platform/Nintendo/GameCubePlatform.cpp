#include "GameCubePlatform.h"
#include "GameCubeGraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_GAMECUBE

// libogc includes
#include <gccore.h>
#include <ogc/pad.h>
#include <cmath>

namespace WillowVox
{
    GameCubePlatform::GameCubePlatform()
    {
    }

    GameCubePlatform::~GameCubePlatform()
    {
        Shutdown();
    }

    bool GameCubePlatform::Initialize()
    {
        Logger::EngineLog("Initializing GameCube Platform (libogc)");

        // Initialize PAD (GameCube controller)
        PAD_Init();

        // Initialize graphics
        m_graphicsContext = std::make_unique<GameCubeGraphicsContext>();

        Logger::EngineLog("GameCube Platform initialized");
        return true;
    }

    void GameCubePlatform::Shutdown()
    {
        m_graphicsContext.reset();
        Logger::EngineLog("GameCube Platform shutdown");
    }

    void GameCubePlatform::ProcessEvents()
    {
        // GameCube doesn't have traditional event polling
        // Input is polled directly via PAD_ScanPads()
    }

    void GameCubePlatform::PollInput(InputState& outInputState)
    {
        UpdateGamepadInput(outInputState);
        outInputState.deviceType = InputDeviceType::GameCubeController;
    }

    void GameCubePlatform::UpdateGamepadInput(InputState& outInputState)
    {
        // Store previous button state
        m_prevButtons = m_buttons;

        // Scan for button presses
        PAD_ScanPads();

        // Read controller 0 state
        u16 buttonsDown = PAD_ButtonsDown(0);
        u16 buttonsHeld = PAD_ButtonsHeld(0);
        m_buttons = buttonsHeld;

        // Read analog sticks
        s8 mainStickX = PAD_StickX(0);
        s8 mainStickY = PAD_StickY(0);
        s8 cStickX = PAD_SubStickX(0);
        s8 cStickY = PAD_SubStickY(0);

        m_mainStickX = NormalizeAxis(mainStickX);
        m_mainStickY = NormalizeAxis(mainStickY);
        m_cStickX = NormalizeAxis(cStickX);
        m_cStickY = NormalizeAxis(cStickY);

        // Read triggers
        u8 triggerL = PAD_TriggerL(0);
        u8 triggerR = PAD_TriggerR(0);

        m_leftTrigger = NormalizeTrigger(triggerL);
        m_rightTrigger = NormalizeTrigger(triggerR);

        // Map analog sticks
        outInputState.moveAxisX = m_mainStickX;
        outInputState.moveAxisY = m_mainStickY;
        outInputState.lookAxisX = m_cStickX * 100.0f;
        outInputState.lookAxisY = -m_cStickY * 100.0f; // Invert Y for camera

        // Button mapping (GameCube controller)
        bool btnA = (m_buttons & PAD_BUTTON_A) != 0;
        bool btnB = (m_buttons & PAD_BUTTON_B) != 0;
        bool btnX = (m_buttons & PAD_BUTTON_X) != 0;
        bool btnY = (m_buttons & PAD_BUTTON_Y) != 0;
        bool btnZ = (m_buttons & PAD_TRIGGER_Z) != 0;
        bool btnL = (m_buttons & PAD_TRIGGER_L) != 0;
        bool btnR = (m_buttons & PAD_TRIGGER_R) != 0;
        bool btnStart = (m_buttons & PAD_BUTTON_START) != 0;
        bool btnDpadUp = (m_buttons & PAD_BUTTON_UP) != 0;
        bool btnDpadDown = (m_buttons & PAD_BUTTON_DOWN) != 0;
        bool btnDpadLeft = (m_buttons & PAD_BUTTON_LEFT) != 0;
        bool btnDpadRight = (m_buttons & PAD_BUTTON_RIGHT) != 0;

        // Map to abstract actions
        outInputState.actionsHeld[static_cast<int>(InputAction::Action1)] = btnA;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action2)] = btnB;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action3)] = btnX;
        outInputState.actionsHeld[static_cast<int>(InputAction::Action4)] = btnY;
        outInputState.actionsHeld[static_cast<int>(InputAction::Jump)] = btnA;
        outInputState.actionsHeld[static_cast<int>(InputAction::Crouch)] = btnZ;
        outInputState.actionsHeld[static_cast<int>(InputAction::CycleLeft)] = btnL;
        outInputState.actionsHeld[static_cast<int>(InputAction::CycleRight)] = btnR;
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuOpen)] = btnStart;
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuBack)] = btnB;

        // Analog triggers for sprint/crouch
        if (m_leftTrigger > 0.5f)
            outInputState.actionsHeld[static_cast<int>(InputAction::CycleLeft)] = true;
        if (m_rightTrigger > 0.5f)
            outInputState.actionsHeld[static_cast<int>(InputAction::Sprint)] = true;

        // D-pad for movement
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveForward)] = btnDpadUp;
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveBackward)] = btnDpadDown;
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveLeft)] = btnDpadLeft;
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveRight)] = btnDpadRight;

        // Pressed this frame
        outInputState.actions[static_cast<int>(InputAction::Action1)] = (buttonsDown & PAD_BUTTON_A) != 0;
        outInputState.actions[static_cast<int>(InputAction::Action2)] = (buttonsDown & PAD_BUTTON_B) != 0;
        outInputState.actions[static_cast<int>(InputAction::MenuOpen)] = (buttonsDown & PAD_BUTTON_START) != 0;
        outInputState.actions[static_cast<int>(InputAction::Jump)] = (buttonsDown & PAD_BUTTON_A) != 0;

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

    float GameCubePlatform::NormalizeAxis(int8_t value) const
    {
        // GameCube sticks are -128 to 127
        float normalized = static_cast<float>(value) / 127.0f;

        // Apply deadzone
        const float deadzone = 0.15f;
        if (std::abs(normalized) < deadzone)
            return 0.0f;

        // Rescale to account for deadzone
        float sign = (normalized > 0.0f) ? 1.0f : -1.0f;
        float absValue = std::abs(normalized);
        return sign * ((absValue - deadzone) / (1.0f - deadzone));
    }

    float GameCubePlatform::NormalizeTrigger(uint8_t value) const
    {
        // GameCube triggers are 0-255
        float normalized = static_cast<float>(value) / 255.0f;

        // Apply trigger deadzone
        const float deadzone = 0.1f;
        if (normalized < deadzone)
            return 0.0f;

        // Rescale to account for deadzone
        return (normalized - deadzone) / (1.0f - deadzone);
    }

    void GameCubePlatform::ResetInputFrameState(InputState& inputState)
    {
        inputState.ResetFrameStates();
    }

    IGraphicsContext* GameCubePlatform::GetGraphicsContext()
    {
        return m_graphicsContext.get();
    }

    const char* GameCubePlatform::GetUserDataPath() const
    {
        return "sd:/WillowVox/userdata";
    }

    const char* GameCubePlatform::GetAssetsPath() const
    {
        return "sd:/WillowVox/assets";
    }

    const char* GameCubePlatform::GetPlatformName() const
    {
        return "Nintendo GameCube";
    }

    InputDeviceType GameCubePlatform::GetPrimaryInputDevice() const
    {
        return InputDeviceType::GameCubeController;
    }

    bool GameCubePlatform::HasFeature(const char* featureName) const
    {
        if (strcmp(featureName, "gamepad") == 0) return true;
        if (strcmp(featureName, "filesystem") == 0) return true;
        return false;
    }
}

#endif // PLATFORM_GAMECUBE
