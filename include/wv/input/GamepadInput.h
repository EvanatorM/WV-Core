#pragma once

#include <wv/platform/InputState.h>
#include <wv/wvpch.h>

namespace WillowVox
{
    /**
     * Gamepad-Specific Input API
     *
     * Provides access to gamepad-specific features like:
     * - Individual stick values
     * - Trigger values
     * - Vibration/haptics (future)
     * - Multiple gamepad support (future)
     *
     * Usage:
     *   float leftX = GamepadInput::GetLeftStickX();
     *   float rightTrigger = GamepadInput::GetTriggerRight();
     *   bool connected = GamepadInput::IsConnected(0);
     */
    class GamepadInput
    {
    public:
        /**
         * Initialize gamepad input with current state
         * Called automatically by InputManager
         */
        static void SetInputState(const InputState* state);

        // ===== Stick Input =====

        /**
         * Get left stick X axis (-1.0 to 1.0)
         * @return Left stick horizontal value
         */
        static float GetLeftStickX();

        /**
         * Get left stick Y axis (-1.0 to 1.0)
         * @return Left stick vertical value
         */
        static float GetLeftStickY();

        /**
         * Get left stick as a vector
         * @return glm::vec2 (x, y) of left stick
         */
        static glm::vec2 GetLeftStick();

        /**
         * Get right stick X axis (-1.0 to 1.0)
         * @return Right stick horizontal value
         */
        static float GetRightStickX();

        /**
         * Get right stick Y axis (-1.0 to 1.0)
         * @return Right stick vertical value
         */
        static float GetRightStickY();

        /**
         * Get right stick as a vector
         * @return glm::vec2 (x, y) of right stick
         */
        static glm::vec2 GetRightStick();

        // ===== Trigger Input =====

        /**
         * Get left trigger value (0.0 to 1.0)
         * Simulated on platforms without analog triggers
         * @return Left trigger pressure
         */
        static float GetTriggerLeft();

        /**
         * Get right trigger value (0.0 to 1.0)
         * Simulated on platforms without analog triggers
         * @return Right trigger pressure
         */
        static float GetTriggerRight();

        /**
         * Check if left trigger is pressed past threshold
         * @param threshold Threshold value (default 0.5)
         * @return True if trigger exceeds threshold
         */
        static bool GetTriggerLeftDown(float threshold = 0.5f);

        /**
         * Check if right trigger is pressed past threshold
         * @param threshold Threshold value (default 0.5)
         * @return True if trigger exceeds threshold
         */
        static bool GetTriggerRightDown(float threshold = 0.5f);

        // ===== D-Pad Input =====

        /**
         * Get D-Pad as a vector
         * @return glm::vec2 (-1/0/1 for each axis)
         */
        static glm::vec2 GetDPad();

        /**
         * Check if D-Pad direction is pressed
         */
        static bool GetDPadUp();
        static bool GetDPadDown();
        static bool GetDPadLeft();
        static bool GetDPadRight();

        // ===== Face Buttons =====

        /**
         * Get face button state (A/B/X/Y or Cross/Circle/Square/Triangle)
         * These map to the physical location, not semantic meaning
         */
        static bool GetButtonSouth();    // A / Cross
        static bool GetButtonEast();     // B / Circle
        static bool GetButtonWest();     // X / Square
        static bool GetButtonNorth();    // Y / Triangle

        // ===== Shoulder Buttons =====

        static bool GetShoulderLeft();   // LB / L1
        static bool GetShoulderRight();  // RB / R1

        // ===== Special Buttons =====

        static bool GetButtonStart();    // Start / Options / Plus
        static bool GetButtonSelect();   // Back / Share / Minus

        // ===== Connection Status =====

        /**
         * Check if a gamepad is connected
         * @param index Gamepad index (0-3), default 0
         * @return True if gamepad is connected
         */
        static bool IsConnected(int index = 0);

        /**
         * Get the gamepad type
         * @return InputDeviceType of the connected gamepad
         */
        static InputDeviceType GetGamepadType();

        // ===== Vibration (Platform-Specific) =====

        /**
         * Set vibration/rumble on the gamepad
         * Not all platforms support this
         * @param leftMotor Low frequency motor (0.0-1.0)
         * @param rightMotor High frequency motor (0.0-1.0)
         * @param duration Duration in seconds (0 = continuous)
         */
        static void SetVibration(float leftMotor, float rightMotor, float duration = 0.0f);

        /**
         * Stop all vibration
         */
        static void StopVibration();

    private:
        static const InputState* s_currentState;
        static class IPlatform* s_platform;

        // Trigger values (simulated on platforms without analog triggers)
        static float s_leftTrigger;
        static float s_rightTrigger;

        friend class InputManager;  // Allow InputManager to set platform pointer
    };
}
