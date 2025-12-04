#pragma once

#include <wv/platform/InputState.h>
#include <wv/wvpch.h>

namespace WillowVox
{
    /**
     * Centralized Input Manager (Unity-like API)
     *
     * Provides a static interface for querying input state across all platforms.
     * Automatically adapts to the current platform's input device.
     *
     * Usage:
     *   if (InputManager::GetButton(InputAction::Jump))
     *   if (InputManager::GetButtonDown(InputAction::Action1))
     *   float moveX = InputManager::GetAxis("MoveX");
     *   float lookY = InputManager::GetAxis("LookY");
     */
    class InputManager
    {
    public:
        /**
         * Initialize the input manager with the current input state
         * Called automatically by the engine each frame
         */
        static void SetInputState(const InputState* state);

        // ===== Button Input =====

        /**
         * Check if a button is currently held down
         * @param action The abstract input action to check
         * @return True if the button is held down
         */
        static bool GetButton(InputAction action);

        /**
         * Check if a button was pressed this frame
         * @param action The abstract input action to check
         * @return True if the button was pressed this frame
         */
        static bool GetButtonDown(InputAction action);

        /**
         * Check if a button was released this frame
         * @param action The abstract input action to check
         * @return True if the button was released this frame
         */
        static bool GetButtonUp(InputAction action);

        // ===== Axis Input =====

        /**
         * Get the value of a virtual axis (-1.0 to 1.0)
         * @param axisName Name of the axis ("MoveX", "MoveY", "LookX", "LookY")
         * @return The axis value normalized to -1.0 to 1.0
         */
        static float GetAxis(const char* axisName);

        /**
         * Get raw axis value without smoothing
         * Same as GetAxis() for now, but provided for Unity compatibility
         */
        static float GetAxisRaw(const char* axisName);

        // ===== Movement =====

        /**
         * Get movement vector (X, Y) from analog stick or keyboard
         * @return glm::vec2 where x = left/right, y = forward/backward
         */
        static glm::vec2 GetMovementVector();

        /**
         * Get look vector (X, Y) from analog stick or mouse
         * @return glm::vec2 where x = horizontal, y = vertical
         */
        static glm::vec2 GetLookVector();

        // ===== Pointer/Touch =====

        /**
         * Get pointer position (mouse or primary touch)
         * @return glm::vec2 screen position
         */
        static glm::vec2 GetPointerPosition();

        /**
         * Get pointer delta (mouse movement or touch drag)
         * @return glm::vec2 delta since last frame
         */
        static glm::vec2 GetPointerDelta();

        /**
         * Check if pointer is down (mouse button or touch)
         * @return True if pointer is currently down
         */
        static bool GetPointerDown();

        // ===== Device Info =====

        /**
         * Get the current input device type
         * @return InputDeviceType enum value
         */
        static InputDeviceType GetDeviceType();

        /**
         * Check if a specific device type is active
         * @param deviceType Device type to check
         * @return True if the device type matches
         */
        static bool IsDeviceType(InputDeviceType deviceType);

        /**
         * Check if the current device is a gamepad
         * @return True if using any gamepad type
         */
        static bool IsGamepad();

        /**
         * Check if the current device has a touchscreen
         * @return True if using touchscreen
         */
        static bool IsTouchscreen();

        // ===== Scroll =====

        /**
         * Get scroll wheel delta
         * @return Scroll delta this frame
         */
        static float GetScrollDelta();

        // ===== Convenience Methods =====

        /**
         * Check if any movement input is active
         * @return True if player is trying to move
         */
        static bool IsMoving();

        /**
         * Check if any look input is active
         * @return True if player is trying to look around
         */
        static bool IsLooking();

    private:
        static const InputState* s_currentState;

        // Helper to get axis by name
        static float GetAxisInternal(const char* axisName);
    };
}
