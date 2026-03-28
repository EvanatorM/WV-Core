#include <wv/input/GamepadInput.h>
#include <wv/platform/IPlatform.h>
#include <cmath>

namespace WillowVox
{
    const InputState* GamepadInput::s_currentState = nullptr;
    IPlatform* GamepadInput::s_platform = nullptr;
    float GamepadInput::s_leftTrigger = 0.0f;
    float GamepadInput::s_rightTrigger = 0.0f;

    void GamepadInput::SetInputState(const InputState* state)
    {
        s_currentState = state;

        // Update simulated trigger values based on button state
        if (s_currentState)
        {
            s_leftTrigger = s_currentState->IsActionHeld(InputAction::Crouch) ? 1.0f : 0.0f;
            s_rightTrigger = s_currentState->IsActionHeld(InputAction::Sprint) ? 1.0f : 0.0f;
        }
    }

    float GamepadInput::GetLeftStickX()
    {
        if (!s_currentState) return 0.0f;
        return s_currentState->moveAxisX;
    }

    float GamepadInput::GetLeftStickY()
    {
        if (!s_currentState) return 0.0f;
        return s_currentState->moveAxisY;
    }

    glm::vec2 GamepadInput::GetLeftStick()
    {
        if (!s_currentState) return glm::vec2(0.0f);
        return glm::vec2(s_currentState->moveAxisX, s_currentState->moveAxisY);
    }

    float GamepadInput::GetRightStickX()
    {
        if (!s_currentState) return 0.0f;
        // Look axis from platforms is typically scaled up (e.g., 100x for sensitivity)
        // Normalize it back to -1..1 range
        float value = s_currentState->lookAxisX;
        return glm::clamp(value / 100.0f, -1.0f, 1.0f);
    }

    float GamepadInput::GetRightStickY()
    {
        if (!s_currentState) return 0.0f;
        float value = s_currentState->lookAxisY;
        return glm::clamp(value / 100.0f, -1.0f, 1.0f);
    }

    glm::vec2 GamepadInput::GetRightStick()
    {
        return glm::vec2(GetRightStickX(), GetRightStickY());
    }

    float GamepadInput::GetTriggerLeft()
    {
        return s_leftTrigger;
    }

    float GamepadInput::GetTriggerRight()
    {
        return s_rightTrigger;
    }

    bool GamepadInput::GetTriggerLeftDown(float threshold)
    {
        return s_leftTrigger > threshold;
    }

    bool GamepadInput::GetTriggerRightDown(float threshold)
    {
        return s_rightTrigger > threshold;
    }

    glm::vec2 GamepadInput::GetDPad()
    {
        if (!s_currentState) return glm::vec2(0.0f);

        float x = 0.0f;
        float y = 0.0f;

        if (s_currentState->IsActionHeld(InputAction::MoveLeft)) x = -1.0f;
        if (s_currentState->IsActionHeld(InputAction::MoveRight)) x = 1.0f;
        if (s_currentState->IsActionHeld(InputAction::MoveForward)) y = 1.0f;
        if (s_currentState->IsActionHeld(InputAction::MoveBackward)) y = -1.0f;

        return glm::vec2(x, y);
    }

    bool GamepadInput::GetDPadUp()
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionHeld(InputAction::MoveForward);
    }

    bool GamepadInput::GetDPadDown()
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionHeld(InputAction::MoveBackward);
    }

    bool GamepadInput::GetDPadLeft()
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionHeld(InputAction::MoveLeft);
    }

    bool GamepadInput::GetDPadRight()
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionHeld(InputAction::MoveRight);
    }

    bool GamepadInput::GetButtonSouth()
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionHeld(InputAction::Action1);
    }

    bool GamepadInput::GetButtonEast()
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionHeld(InputAction::Action2);
    }

    bool GamepadInput::GetButtonWest()
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionHeld(InputAction::Action3);
    }

    bool GamepadInput::GetButtonNorth()
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionHeld(InputAction::Action4);
    }

    bool GamepadInput::GetShoulderLeft()
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionHeld(InputAction::CycleLeft);
    }

    bool GamepadInput::GetShoulderRight()
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionHeld(InputAction::CycleRight);
    }

    bool GamepadInput::GetButtonStart()
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionHeld(InputAction::MenuOpen);
    }

    bool GamepadInput::GetButtonSelect()
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionHeld(InputAction::MenuBack);
    }

    bool GamepadInput::IsConnected(int index)
    {
        if (index != 0) return false; // Only support player 0 for now
        if (!s_currentState) return false;

        // Consider connected if device type is a gamepad
        InputDeviceType device = s_currentState->deviceType;
        return device == InputDeviceType::Gamepad ||
               device == InputDeviceType::XboxController ||
               device == InputDeviceType::PSController ||
               device == InputDeviceType::SwitchController ||
               device == InputDeviceType::WiiRemote ||
               device == InputDeviceType::GameCubeController ||
               device == InputDeviceType::WiiUGamePad;
    }

    InputDeviceType GamepadInput::GetGamepadType()
    {
        if (!s_currentState) return InputDeviceType::Unknown;
        return s_currentState->deviceType;
    }

    void GamepadInput::SetVibration(float leftMotor, float rightMotor, float duration)
    {
        if (!s_platform)
            return;

        // Call platform's vibration implementation
        // Duration is not yet implemented (would need a timer system)
        s_platform->SetVibration(0, leftMotor, rightMotor);
    }

    void GamepadInput::StopVibration()
    {
        if (!s_platform)
            return;

        s_platform->StopVibration(0);
    }
}
