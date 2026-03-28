#include "iOSPlatform.h"
#include "iOSGraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_IOS

// iOS SDK includes
#include <UIKit/UIKit.h>
#include <cmath>
#include <cstring>

namespace WillowVox
{
    iOSPlatform::iOSPlatform()
    {
    }

    iOSPlatform::~iOSPlatform()
    {
        Shutdown();
    }

    bool iOSPlatform::Initialize()
    {
        Logger::EngineLog("Initializing iOS Platform");

        // Graphics context will handle UIKit/OpenGL ES setup
        m_graphicsContext = std::make_unique<iOSGraphicsContext>();

        // Get screen dimensions from UIScreen
        UIScreen* screen = [UIScreen mainScreen];
        CGRect bounds = [screen bounds];
        CGFloat scale = [screen scale];
        m_screenWidth = static_cast<int>(bounds.size.width * scale);
        m_screenHeight = static_cast<int>(bounds.size.height * scale);
        Logger::EngineLog("Screen size: %dx%d (scale: %.1f)", m_screenWidth, m_screenHeight, scale);

        // Setup virtual touch controls (similar to Android)
        SetupVirtualControls();

        Logger::EngineLog("iOS Platform initialized");
        return true;
    }

    void iOSPlatform::Shutdown()
    {
        m_graphicsContext.reset();
        Logger::EngineLog("iOS Platform shutdown");
    }

    void iOSPlatform::ProcessEvents()
    {
        // iOS event processing handled by UIKit run loop
        // This is called from the main game loop
    }

    void iOSPlatform::PollInput(InputState& outInputState)
    {
        ProcessTouchInput(outInputState);

        // Copy button states
        std::memcpy(m_buttonPrevStates, m_buttonStates, sizeof(m_buttonStates));

        outInputState.deviceType = InputDeviceType::Touchscreen;
    }

    void iOSPlatform::SetupVirtualControls()
    {
        // Virtual joystick (left side, bottom)
        m_virtualControls.push_back({0.15f, 0.85f, 0.12f, InputAction::Count, true});

        // Action buttons (right side, bottom)
        m_virtualControls.push_back({0.85f, 0.85f, 0.06f, InputAction::Action1, false});
        m_virtualControls.push_back({0.75f, 0.80f, 0.06f, InputAction::Action2, false});
        m_virtualControls.push_back({0.85f, 0.75f, 0.06f, InputAction::Jump, false});

        // Menu button (top left)
        m_virtualControls.push_back({0.10f, 0.10f, 0.05f, InputAction::MenuOpen, false});
    }

    void iOSPlatform::ProcessTouchInput(InputState& outInputState)
    {
        // Reset per-frame state
        m_lookDeltaX = 0.0f;
        m_lookDeltaY = 0.0f;
        m_joystickX = 0.0f;
        m_joystickY = 0.0f;

        // Reset button states
        for (int i = 0; i < static_cast<int>(InputAction::Count); ++i)
        {
            m_buttonStates[i] = false;
        }

        // Process each active touch point
        // Note: Touch points are populated by UIKit touch event handlers
        // This would be integrated with UIViewController's touchesBegan/Moved/Ended methods
        for (int i = 0; i < MAX_TOUCH_POINTS; ++i)
        {
            TouchPoint& touch = m_touchPoints[i];
            if (!touch.active)
                continue;

            // Check virtual controls
            bool handled = false;

            // Virtual joystick (left side)
            if (touch.startX < 0.5f && touch.id == m_joystickTouchId)
            {
                // Calculate joystick displacement from start position
                float dx = touch.x - touch.startX;
                float dy = touch.y - touch.startY;

                // Normalize joystick input (clamp to radius)
                float dist = std::sqrt(dx * dx + dy * dy);
                float maxDist = 0.12f; // Same as virtual control radius

                if (dist > maxDist)
                {
                    dx = (dx / dist) * maxDist;
                    dy = (dy / dist) * maxDist;
                }

                m_joystickX = dx / maxDist;
                m_joystickY = dy / maxDist;

                handled = true;
            }
            else if (touch.startX < 0.5f && m_joystickTouchId == -1)
            {
                // New touch on left side - assign to joystick
                m_joystickTouchId = touch.id;
                handled = true;
            }

            // Look control (right side)
            if (touch.startX >= 0.5f && touch.id == m_lookTouchId)
            {
                // Calculate look delta
                static float prevLookX = touch.startX;
                static float prevLookY = touch.startY;

                m_lookDeltaX = (touch.x - prevLookX) * m_screenWidth;
                m_lookDeltaY = (touch.y - prevLookY) * m_screenHeight;

                prevLookX = touch.x;
                prevLookY = touch.y;

                handled = true;
            }
            else if (touch.startX >= 0.5f && m_lookTouchId == -1)
            {
                // Check if touching a button first
                bool isTouchingButton = false;
                for (const auto& control : m_virtualControls)
                {
                    if (!control.isJoystick &&
                        IsInsideCircle(touch.x, touch.y, control.x, control.y, control.radius))
                    {
                        m_buttonStates[static_cast<int>(control.action)] = true;
                        isTouchingButton = true;
                        handled = true;
                        break;
                    }
                }

                // If not touching a button, use for look control
                if (!isTouchingButton)
                {
                    m_lookTouchId = touch.id;
                    handled = true;
                }
            }

            // Check button touches
            if (!handled)
            {
                for (const auto& control : m_virtualControls)
                {
                    if (!control.isJoystick &&
                        IsInsideCircle(touch.x, touch.y, control.x, control.y, control.radius))
                    {
                        m_buttonStates[static_cast<int>(control.action)] = true;
                        break;
                    }
                }
            }
        }

        // Check for released touch IDs
        bool joystickActive = false;
        bool lookActive = false;
        for (int i = 0; i < MAX_TOUCH_POINTS; ++i)
        {
            if (m_touchPoints[i].active)
            {
                if (m_touchPoints[i].id == m_joystickTouchId)
                    joystickActive = true;
                if (m_touchPoints[i].id == m_lookTouchId)
                    lookActive = true;
            }
        }

        if (!joystickActive)
            m_joystickTouchId = -1;
        if (!lookActive)
            m_lookTouchId = -1;

        // Map to InputState
        outInputState.moveAxisX = m_joystickX;
        outInputState.moveAxisY = -m_joystickY; // Invert Y

        outInputState.lookAxisX = m_lookDeltaX;
        outInputState.lookAxisY = m_lookDeltaY;

        outInputState.pointerDeltaX = m_lookDeltaX;
        outInputState.pointerDeltaY = m_lookDeltaY;

        // Map analog to digital actions
        if (std::abs(m_joystickX) > 0.5f || std::abs(m_joystickY) > 0.5f)
        {
            if (m_joystickY < -0.5f)
                outInputState.actionsHeld[static_cast<int>(InputAction::MoveForward)] = true;
            if (m_joystickY > 0.5f)
                outInputState.actionsHeld[static_cast<int>(InputAction::MoveBackward)] = true;
            if (m_joystickX < -0.5f)
                outInputState.actionsHeld[static_cast<int>(InputAction::MoveLeft)] = true;
            if (m_joystickX > 0.5f)
                outInputState.actionsHeld[static_cast<int>(InputAction::MoveRight)] = true;
        }

        // Copy button states
        for (int i = 0; i < static_cast<int>(InputAction::Count); ++i)
        {
            outInputState.actionsHeld[i] = m_buttonStates[i];
            outInputState.actions[i] = m_buttonStates[i] && !m_buttonPrevStates[i];
            outInputState.actionsReleased[i] = !m_buttonStates[i] && m_buttonPrevStates[i];
        }
    }

    void iOSPlatform::ResetInputFrameState(InputState& inputState)
    {
        inputState.ResetFrameStates();
    }

    IGraphicsContext* iOSPlatform::GetGraphicsContext()
    {
        return m_graphicsContext.get();
    }

    const char* iOSPlatform::GetUserDataPath() const
    {
        // iOS uses app-specific directories
        return "~/Documents/WillowVox";
    }

    const char* iOSPlatform::GetAssetsPath() const
    {
        // iOS bundles assets in the app bundle
        return "";  // Use NSBundle for assets
    }

    const char* iOSPlatform::GetPlatformName() const
    {
        return "iOS";
    }

    InputDeviceType iOSPlatform::GetPrimaryInputDevice() const
    {
        return InputDeviceType::Touchscreen;
    }

    bool iOSPlatform::HasFeature(const char* featureName) const
    {
        if (strcmp(featureName, "touchscreen") == 0) return true;
        if (strcmp(featureName, "filesystem") == 0) return true;
        return false;
    }

    TouchPoint* iOSPlatform::GetTouchPoint(int32_t id)
    {
        for (int i = 0; i < MAX_TOUCH_POINTS; ++i)
        {
            if (m_touchPoints[i].active && m_touchPoints[i].id == id)
                return &m_touchPoints[i];
        }
        return nullptr;
    }

    TouchPoint* iOSPlatform::AllocateTouchPoint(int32_t id)
    {
        for (int i = 0; i < MAX_TOUCH_POINTS; ++i)
        {
            if (!m_touchPoints[i].active)
            {
                m_touchPoints[i].active = true;
                m_touchPoints[i].id = id;
                return &m_touchPoints[i];
            }
        }
        return nullptr;
    }

    void iOSPlatform::ReleaseTouchPoint(int32_t id)
    {
        for (int i = 0; i < MAX_TOUCH_POINTS; ++i)
        {
            if (m_touchPoints[i].active && m_touchPoints[i].id == id)
            {
                m_touchPoints[i].active = false;
                m_touchPoints[i].id = -1;
                return;
            }
        }
    }

    bool iOSPlatform::IsInsideCircle(float x, float y, float cx, float cy, float radius) const
    {
        float dx = x - cx;
        float dy = y - cy;
        return (dx * dx + dy * dy) <= (radius * radius);
    }
}

#endif // PLATFORM_IOS
