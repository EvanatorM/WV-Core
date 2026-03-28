#include <wv/input/InputManager.h>
#include <cstring>
#include <cmath>

namespace WillowVox
{
    const InputState* InputManager::s_currentState = nullptr;

    void InputManager::SetInputState(const InputState* state)
    {
        s_currentState = state;
    }

    bool InputManager::GetButton(InputAction action)
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionHeld(action);
    }

    bool InputManager::GetButtonDown(InputAction action)
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionPressed(action);
    }

    bool InputManager::GetButtonUp(InputAction action)
    {
        if (!s_currentState) return false;
        return s_currentState->IsActionReleased(action);
    }

    float InputManager::GetAxis(const char* axisName)
    {
        return GetAxisInternal(axisName);
    }

    float InputManager::GetAxisRaw(const char* axisName)
    {
        // For now, same as GetAxis - could add smoothing to GetAxis() later
        return GetAxisInternal(axisName);
    }

    float InputManager::GetAxisInternal(const char* axisName)
    {
        if (!s_currentState) return 0.0f;

        // Map axis names to input state values
        if (strcmp(axisName, "MoveX") == 0 || strcmp(axisName, "Horizontal") == 0)
            return s_currentState->moveAxisX;
        else if (strcmp(axisName, "MoveY") == 0 || strcmp(axisName, "Vertical") == 0)
            return s_currentState->moveAxisY;
        else if (strcmp(axisName, "LookX") == 0 || strcmp(axisName, "Mouse X") == 0)
            return s_currentState->lookAxisX;
        else if (strcmp(axisName, "LookY") == 0 || strcmp(axisName, "Mouse Y") == 0)
            return s_currentState->lookAxisY;

        return 0.0f;
    }

    glm::vec2 InputManager::GetMovementVector()
    {
        if (!s_currentState) return glm::vec2(0.0f);
        return glm::vec2(s_currentState->moveAxisX, s_currentState->moveAxisY);
    }

    glm::vec2 InputManager::GetLookVector()
    {
        if (!s_currentState) return glm::vec2(0.0f);
        return glm::vec2(s_currentState->lookAxisX, s_currentState->lookAxisY);
    }

    glm::vec2 InputManager::GetPointerPosition()
    {
        if (!s_currentState) return glm::vec2(0.0f);
        return glm::vec2(s_currentState->pointerX, s_currentState->pointerY);
    }

    glm::vec2 InputManager::GetPointerDelta()
    {
        if (!s_currentState) return glm::vec2(0.0f);
        return glm::vec2(s_currentState->pointerDeltaX, s_currentState->pointerDeltaY);
    }

    bool InputManager::GetPointerDown()
    {
        if (!s_currentState) return false;
        return s_currentState->pointerDown;
    }

    InputDeviceType InputManager::GetDeviceType()
    {
        if (!s_currentState) return InputDeviceType::Unknown;
        return s_currentState->deviceType;
    }

    bool InputManager::IsDeviceType(InputDeviceType deviceType)
    {
        return GetDeviceType() == deviceType;
    }

    bool InputManager::IsGamepad()
    {
        InputDeviceType device = GetDeviceType();
        return device == InputDeviceType::Gamepad ||
               device == InputDeviceType::XboxController ||
               device == InputDeviceType::PSController ||
               device == InputDeviceType::SwitchController ||
               device == InputDeviceType::WiiRemote;
    }

    bool InputManager::IsTouchscreen()
    {
        return GetDeviceType() == InputDeviceType::Touchscreen;
    }

    float InputManager::GetScrollDelta()
    {
        if (!s_currentState) return 0.0f;
        return s_currentState->scrollDelta;
    }

    bool InputManager::IsMoving()
    {
        if (!s_currentState) return false;
        float threshold = 0.1f;
        return std::abs(s_currentState->moveAxisX) > threshold ||
               std::abs(s_currentState->moveAxisY) > threshold ||
               GetButton(InputAction::MoveForward) ||
               GetButton(InputAction::MoveBackward) ||
               GetButton(InputAction::MoveLeft) ||
               GetButton(InputAction::MoveRight);
    }

    bool InputManager::IsLooking()
    {
        if (!s_currentState) return false;
        float threshold = 0.1f;
        return std::abs(s_currentState->lookAxisX) > threshold ||
               std::abs(s_currentState->lookAxisY) > threshold;
    }
}
