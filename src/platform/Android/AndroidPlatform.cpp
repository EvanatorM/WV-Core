#include "AndroidPlatform.h"
#include <wv/Logger.h>

#ifdef PLATFORM_ANDROID

#include <android/input.h>
#include <android_native_app_glue.h>
#include <android/native_activity.h>
#include <cmath>
#include <cstring>

namespace WillowVox
{
    AndroidPlatform::AndroidPlatform()
    {
    }

    AndroidPlatform::~AndroidPlatform()
    {
        Shutdown();
    }

    void AndroidPlatform::SetAndroidApp(android_app* app)
    {
        m_androidApp = app;
    }

    bool AndroidPlatform::Initialize()
    {
        Logger::EngineLog("Initializing Android Platform");

        if (!m_androidApp)
        {
            Logger::EngineError("Android app not set!");
            return false;
        }

        m_graphicsContext = std::make_unique<AndroidGraphicsContext>();
        m_graphicsContext->SetNativeWindow(m_androidApp->window);
        m_graphicsContext->SetAssetManager(m_androidApp->activity->assetManager);

        // Get screen dimensions
        if (m_androidApp->window)
        {
            m_screenWidth = ANativeWindow_getWidth(m_androidApp->window);
            m_screenHeight = ANativeWindow_getHeight(m_androidApp->window);
            Logger::EngineLog("Screen size: %dx%d", m_screenWidth, m_screenHeight);
        }

        // Setup virtual controls
        SetupVirtualControls();

        return true;
    }

    void AndroidPlatform::SetupVirtualControls()
    {
        m_virtualControls.clear();

        // Left virtual joystick (bottom left corner)
        // Position at (0.15, 0.85) with radius 0.12
        m_virtualControls.push_back({0.15f, 0.85f, 0.12f, InputAction::Count, true});

        // Action buttons (bottom right)
        // A button (primary action)
        m_virtualControls.push_back({0.85f, 0.85f, 0.06f, InputAction::Action1, false});

        // B button (secondary action)
        m_virtualControls.push_back({0.75f, 0.80f, 0.06f, InputAction::Action2, false});

        // Jump button (X)
        m_virtualControls.push_back({0.85f, 0.75f, 0.06f, InputAction::Jump, false});

        // Menu button (top left)
        m_virtualControls.push_back({0.10f, 0.10f, 0.05f, InputAction::MenuOpen, false});

        Logger::EngineLog("Virtual controls setup complete");
    }

    void AndroidPlatform::Shutdown()
    {
        m_graphicsContext.reset();
        Logger::EngineLog("Android Platform shutdown");
    }

    void AndroidPlatform::ProcessEvents()
    {
        if (!m_androidApp)
            return;

        // Process pending events
        int events;
        struct android_poll_source* source;

        // Non-blocking poll
        while (ALooper_pollAll(0, nullptr, &events, (void**)&source) >= 0)
        {
            if (source != nullptr)
            {
                source->process(m_androidApp, source);
            }

            // Check if we're exiting
            if (m_androidApp->destroyRequested != 0)
            {
                m_graphicsContext->SetShouldClose(true);
            }
        }
    }

    int32_t AndroidPlatform::HandleInputEvent(AInputEvent* event)
    {
        int32_t eventType = AInputEvent_getType(event);

        if (eventType == AINPUT_EVENT_TYPE_MOTION)
        {
            int32_t action = AMotionEvent_getAction(event);
            int32_t actionMasked = action & AMOTION_EVENT_ACTION_MASK;
            size_t pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                                  >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            int32_t pointerId = AMotionEvent_getPointerId(event, pointerIndex);

            float x = AMotionEvent_getX(event, pointerIndex);
            float y = AMotionEvent_getY(event, pointerIndex);

            // Normalize to 0-1
            float normX = x / m_screenWidth;
            float normY = y / m_screenHeight;

            switch (actionMasked)
            {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
            {
                TouchPoint* touch = AllocateTouchPoint(pointerId);
                if (touch)
                {
                    touch->x = normX;
                    touch->y = normY;
                    touch->startX = normX;
                    touch->startY = normY;
                }
                break;
            }

            case AMOTION_EVENT_ACTION_MOVE:
            {
                // Update all active touch points
                size_t pointerCount = AMotionEvent_getPointerCount(event);
                for (size_t i = 0; i < pointerCount; ++i)
                {
                    int32_t id = AMotionEvent_getPointerId(event, i);
                    TouchPoint* touch = GetTouchPoint(id);
                    if (touch)
                    {
                        touch->x = AMotionEvent_getX(event, i) / m_screenWidth;
                        touch->y = AMotionEvent_getY(event, i) / m_screenHeight;
                    }
                }
                break;
            }

            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
            case AMOTION_EVENT_ACTION_CANCEL:
            {
                ReleaseTouchPoint(pointerId);
                break;
            }
            }

            return 1; // Event handled
        }

        return 0; // Event not handled
    }

    void AndroidPlatform::PollInput(InputState& outInputState)
    {
        ProcessTouchInput(outInputState);

        // Copy button states
        std::memcpy(m_buttonPrevStates, m_buttonStates, sizeof(m_buttonStates));

        outInputState.deviceType = InputDeviceType::Touchscreen;
    }

    void AndroidPlatform::ProcessTouchInput(InputState& outInputState)
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

    void AndroidPlatform::ResetInputFrameState(InputState& inputState)
    {
        inputState.ResetFrameStates();
    }

    TouchPoint* AndroidPlatform::GetTouchPoint(int32_t id)
    {
        for (int i = 0; i < MAX_TOUCH_POINTS; ++i)
        {
            if (m_touchPoints[i].active && m_touchPoints[i].id == id)
                return &m_touchPoints[i];
        }
        return nullptr;
    }

    TouchPoint* AndroidPlatform::AllocateTouchPoint(int32_t id)
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

    void AndroidPlatform::ReleaseTouchPoint(int32_t id)
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

    bool AndroidPlatform::IsInsideCircle(float x, float y, float cx, float cy, float radius) const
    {
        float dx = x - cx;
        float dy = y - cy;
        return (dx * dx + dy * dy) <= (radius * radius);
    }

    IGraphicsContext* AndroidPlatform::GetGraphicsContext()
    {
        return m_graphicsContext.get();
    }

    const char* AndroidPlatform::GetUserDataPath() const
    {
        if (m_androidApp && m_androidApp->activity)
        {
            return m_androidApp->activity->internalDataPath;
        }
        return "/sdcard/WillowVox";
    }

    const char* AndroidPlatform::GetAssetsPath() const
    {
        return ""; // Android uses AssetManager, not file paths
    }

    const char* AndroidPlatform::GetPlatformName() const
    {
        return "Android";
    }

    InputDeviceType AndroidPlatform::GetPrimaryInputDevice() const
    {
        return InputDeviceType::Touchscreen;
    }

    bool AndroidPlatform::HasFeature(const char* featureName) const
    {
        if (strcmp(featureName, "touchscreen") == 0) return true;
        if (strcmp(featureName, "filesystem") == 0) return true;
        return false;
    }
}

#endif // PLATFORM_ANDROID
