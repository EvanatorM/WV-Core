#pragma once

#include <wv/platform/InputState.h>
#include <wv/wvpch.h>
#include <vector>

namespace WillowVox
{
    /**
     * Touch Point Information
     *
     * Represents a single touch point on the screen
     */
    struct Touch
    {
        int fingerId;           // Unique identifier for this touch
        glm::vec2 position;     // Current position (screen coordinates)
        glm::vec2 deltaPosition; // Movement since last frame
        float pressure;         // Touch pressure (0.0-1.0), if supported
        bool isActive;          // Is this touch currently active

        // Touch phase
        enum class Phase
        {
            Began,      // Touch just started
            Moved,      // Touch moved
            Stationary, // Touch is down but hasn't moved
            Ended,      // Touch just ended
            Canceled    // Touch was canceled
        };
        Phase phase;

        Touch() : fingerId(-1), position(0.0f), deltaPosition(0.0f),
                  pressure(1.0f), isActive(false), phase(Phase::Ended) {}
    };

    /**
     * Touch-Specific Input API
     *
     * Provides access to touchscreen features for mobile and Wii U GamePad:
     * - Multi-touch support
     * - Touch position and delta
     * - Touch phase tracking
     * - Gesture recognition (future)
     *
     * Usage:
     *   int touchCount = TouchInput::GetTouchCount();
     *   Touch touch = TouchInput::GetTouch(0);
     *   if (touch.phase == Touch::Phase::Began) { ... }
     */
    class TouchInput
    {
    public:
        /**
         * Initialize touch input with current state
         * Called automatically by InputManager
         */
        static void SetInputState(const InputState* state);

        // ===== Touch Count =====

        /**
         * Get the number of active touches
         * @return Number of fingers currently touching the screen
         */
        static int GetTouchCount();

        /**
         * Check if any touch is active
         * @return True if at least one finger is touching
         */
        static bool IsTouching();

        // ===== Touch Access =====

        /**
         * Get touch information by index
         * @param index Touch index (0 to GetTouchCount()-1)
         * @return Touch information, or invalid touch if index out of range
         */
        static Touch GetTouch(int index);

        /**
         * Get the primary touch (first touch point)
         * @return Primary touch information
         */
        static Touch GetPrimaryTouch();

        /**
         * Get all active touches
         * @return Vector of all active touch points
         */
        static std::vector<Touch> GetTouches();

        // ===== Touch Position =====

        /**
         * Get primary touch position
         * @return Screen position of first touch
         */
        static glm::vec2 GetTouchPosition();

        /**
         * Get primary touch delta
         * @return Movement delta of first touch
         */
        static glm::vec2 GetTouchDelta();

        // ===== Touch Phase Queries =====

        /**
         * Check if any touch just began this frame
         * @return True if new touch detected
         */
        static bool TouchBegan();

        /**
         * Check if any touch moved this frame
         * @return True if active touch moved
         */
        static bool TouchMoved();

        /**
         * Check if any touch ended this frame
         * @return True if touch was released
         */
        static bool TouchEnded();

        // ===== Platform Features =====

        /**
         * Check if the current platform supports touch input
         * @return True if touchscreen is available
         */
        static bool IsSupported();

        /**
         * Check if the platform supports multi-touch
         * @return True if multiple simultaneous touches are supported
         */
        static bool SupportsMultiTouch();

        /**
         * Get maximum number of simultaneous touches supported
         * @return Max touch points (typically 5-10)
         */
        static int GetMaxTouchCount();

        // ===== Wii U GamePad Specific =====

        /**
         * Check if touch input is from Wii U GamePad
         * @return True if device is Wii U GamePad
         */
        static bool IsWiiUGamePad();

    private:
        static const InputState* s_currentState;
        static std::vector<Touch> s_touches;
        static std::vector<Touch> s_prevTouches;
        static constexpr int MAX_TOUCHES = 10;

        // Update touch list from input state
        static void UpdateTouches();

        // Helper to find touch in previous frame
        static Touch* FindPreviousTouch(int fingerId);
    };
}
