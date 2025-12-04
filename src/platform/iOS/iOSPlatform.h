#pragma once

#include <wv/platform/IPlatform.h>
#include <memory>
#include <vector>

namespace WillowVox
{
    class iOSGraphicsContext;

    /**
     * iOS Platform Implementation
     *
     * Virtual Touch Controls (similar to Android):
     * - Left virtual joystick: Movement
     * - Right side: Look control (drag)
     * - Bottom right buttons: A (Action1), B (Action2), Jump
     * - Top left: Menu button
     */
    class iOSPlatform : public IPlatform
    {
    public:
        iOSPlatform();
        ~iOSPlatform() override;

        bool Initialize() override;
        void Shutdown() override;

        void PollInput(InputState& outInputState) override;
        void ResetInputFrameState(InputState& inputState) override;
        void ProcessEvents() override;

        IGraphicsContext* GetGraphicsContext() override;

        const char* GetUserDataPath() const override;
        const char* GetAssetsPath() const override;
        const char* GetPlatformName() const override;
        InputDeviceType GetPrimaryInputDevice() const override;
        bool HasFeature(const char* featureName) const override;

    private:
        struct TouchPoint
        {
            int32_t id;
            float x, y;
            float startX, startY;
            bool active;
        };

        std::unique_ptr<iOSGraphicsContext> m_graphicsContext;

        // Touch tracking
        static constexpr int MAX_TOUCH_POINTS = 10;
        TouchPoint m_touchPoints[MAX_TOUCH_POINTS] = {};

        // Virtual control regions (normalized 0-1)
        struct VirtualControl
        {
            float x, y, radius;
            InputAction action;
            bool isJoystick;
        };

        std::vector<VirtualControl> m_virtualControls;

        // Input state
        float m_joystickX = 0.0f;
        float m_joystickY = 0.0f;
        int m_joystickTouchId = -1;

        float m_lookDeltaX = 0.0f;
        float m_lookDeltaY = 0.0f;
        int m_lookTouchId = -1;

        bool m_buttonStates[static_cast<int>(InputAction::Count)] = {};
        bool m_buttonPrevStates[static_cast<int>(InputAction::Count)] = {};

        int m_screenWidth = 1920;
        int m_screenHeight = 1080;

        // Helper methods
        void SetupVirtualControls();
        void ProcessTouchInput(InputState& outInputState);
        TouchPoint* GetTouchPoint(int32_t id);
        TouchPoint* AllocateTouchPoint(int32_t id);
        void ReleaseTouchPoint(int32_t id);
        bool IsInsideCircle(float x, float y, float cx, float cy, float radius) const;
    };
}
