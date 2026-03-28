#include <wv/input/TouchInput.h>

namespace WillowVox
{
    const InputState* TouchInput::s_currentState = nullptr;
    std::vector<Touch> TouchInput::s_touches;
    std::vector<Touch> TouchInput::s_prevTouches;

    void TouchInput::SetInputState(const InputState* state)
    {
        s_currentState = state;

        // Store previous frame's touches
        s_prevTouches = s_touches;

        UpdateTouches();
    }

    void TouchInput::UpdateTouches()
    {
        s_touches.clear();

        if (!s_currentState) return;
        if (!IsSupported()) return;

        // Create touch from pointer state
        if (s_currentState->pointerDown)
        {
            Touch touch;
            touch.fingerId = 0;
            touch.position = glm::vec2(s_currentState->pointerX, s_currentState->pointerY);
            touch.deltaPosition = glm::vec2(s_currentState->pointerDeltaX, s_currentState->pointerDeltaY);
            touch.pressure = 1.0f;
            touch.isActive = true;

            // Determine phase based on previous frame
            Touch* prevTouch = FindPreviousTouch(touch.fingerId);

            if (!prevTouch)
            {
                // New touch - just began
                touch.phase = Touch::Phase::Began;
            }
            else
            {
                // Existing touch - check if moved
                float deltaX = touch.position.x - prevTouch->position.x;
                float deltaY = touch.position.y - prevTouch->position.y;
                float moveDist = std::sqrt(deltaX * deltaX + deltaY * deltaY);

                if (moveDist > 0.01f)
                    touch.phase = Touch::Phase::Moved;
                else
                    touch.phase = Touch::Phase::Stationary;
            }

            s_touches.push_back(touch);
        }
    }

    Touch* TouchInput::FindPreviousTouch(int fingerId)
    {
        for (auto& touch : s_prevTouches)
        {
            if (touch.fingerId == fingerId && touch.isActive)
                return &touch;
        }
        return nullptr;
    }

    int TouchInput::GetTouchCount()
    {
        return static_cast<int>(s_touches.size());
    }

    bool TouchInput::IsTouching()
    {
        return GetTouchCount() > 0;
    }

    Touch TouchInput::GetTouch(int index)
    {
        if (index < 0 || index >= static_cast<int>(s_touches.size()))
            return Touch(); // Return invalid touch

        return s_touches[index];
    }

    Touch TouchInput::GetPrimaryTouch()
    {
        return GetTouch(0);
    }

    std::vector<Touch> TouchInput::GetTouches()
    {
        return s_touches;
    }

    glm::vec2 TouchInput::GetTouchPosition()
    {
        if (!s_currentState) return glm::vec2(0.0f);
        return glm::vec2(s_currentState->pointerX, s_currentState->pointerY);
    }

    glm::vec2 TouchInput::GetTouchDelta()
    {
        if (!s_currentState) return glm::vec2(0.0f);
        return glm::vec2(s_currentState->pointerDeltaX, s_currentState->pointerDeltaY);
    }

    bool TouchInput::TouchBegan()
    {
        if (s_touches.empty()) return false;

        for (const auto& touch : s_touches)
        {
            if (touch.phase == Touch::Phase::Began)
                return true;
        }
        return false;
    }

    bool TouchInput::TouchMoved()
    {
        if (s_touches.empty()) return false;

        for (const auto& touch : s_touches)
        {
            if (touch.phase == Touch::Phase::Moved)
                return true;
        }
        return false;
    }

    bool TouchInput::TouchEnded()
    {
        // Check if any touch from previous frame is no longer active
        for (const auto& prevTouch : s_prevTouches)
        {
            if (!prevTouch.isActive)
                continue;

            // Check if this touch still exists in current frame
            bool stillActive = false;
            for (const auto& currentTouch : s_touches)
            {
                if (currentTouch.fingerId == prevTouch.fingerId)
                {
                    stillActive = true;
                    break;
                }
            }

            if (!stillActive)
                return true;  // Touch ended this frame
        }

        return false;
    }

    bool TouchInput::IsSupported()
    {
        if (!s_currentState) return false;

        return s_currentState->deviceType == InputDeviceType::Touchscreen ||
               s_currentState->deviceType == InputDeviceType::WiiUGamePad;
    }

    bool TouchInput::SupportsMultiTouch()
    {
        // Most modern touchscreens support multi-touch
        return IsSupported();
    }

    int TouchInput::GetMaxTouchCount()
    {
        if (!IsSupported()) return 0;
        return MAX_TOUCHES;
    }

    bool TouchInput::IsWiiUGamePad()
    {
        if (!s_currentState) return false;
        return s_currentState->deviceType == InputDeviceType::WiiUGamePad;
    }
}
