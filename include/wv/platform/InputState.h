#pragma once

#include <wv/wvpch.h>

namespace WillowVox
{
    /**
     * Platform-Agnostic Input State
     *
     * This structure represents the abstract input state that the game layer sees.
     * All platform-specific inputs (keyboard, mouse, touch, gamepads) are mapped
     * to these abstract actions by the platform layer.
     */

    // Abstract input actions that game understands
    enum class InputAction
    {
        // Movement
        MoveForward,
        MoveBackward,
        MoveLeft,
        MoveRight,
        MoveUp,
        MoveDown,

        // Camera/Look
        LookUp,
        LookDown,
        LookLeft,
        LookRight,

        // Primary actions
        Action1,        // Primary action (e.g., attack, break block, confirm)
        Action2,        // Secondary action (e.g., use item, place block, cancel)
        Action3,        // Tertiary action (e.g., pick block, special)
        Action4,        // Quaternary action

        // Menu/UI
        MenuOpen,       // Open main menu / pause
        MenuBack,       // Back / escape
        MenuConfirm,    // Confirm selection
        MenuCancel,     // Cancel selection

        // Other
        Jump,
        Crouch,
        Sprint,
        Interact,

        // Special
        CycleLeft,      // Cycle items/options left
        CycleRight,     // Cycle items/options right

        Count
    };

    // Input device types
    enum class InputDeviceType
    {
        Unknown,
        KeyboardMouse,
        Touchscreen,
        Gamepad,
        WiiRemote,
        PSController,
        XboxController,
        SwitchController
    };

    // Abstract input state
    struct InputState
    {
        // Digital buttons (pressed this frame)
        bool actions[static_cast<int>(InputAction::Count)] = {};

        // Digital buttons (held down)
        bool actionsHeld[static_cast<int>(InputAction::Count)] = {};

        // Digital buttons (released this frame)
        bool actionsReleased[static_cast<int>(InputAction::Count)] = {};

        // Analog inputs (-1.0 to 1.0)
        float moveAxisX = 0.0f;      // Left/Right movement
        float moveAxisY = 0.0f;      // Forward/Backward movement
        float lookAxisX = 0.0f;      // Horizontal look
        float lookAxisY = 0.0f;      // Vertical look

        // Mouse/Touch position (normalized 0-1 or pixels depending on platform)
        float pointerX = 0.0f;
        float pointerY = 0.0f;
        float pointerDeltaX = 0.0f;
        float pointerDeltaY = 0.0f;
        bool pointerDown = false;

        // Scroll/Zoom
        float scrollDelta = 0.0f;

        // Device info
        InputDeviceType deviceType = InputDeviceType::Unknown;

        // Helper to check if action is pressed this frame
        bool IsActionPressed(InputAction action) const
        {
            return actions[static_cast<int>(action)];
        }

        // Helper to check if action is held
        bool IsActionHeld(InputAction action) const
        {
            return actionsHeld[static_cast<int>(action)];
        }

        // Helper to check if action was released this frame
        bool IsActionReleased(InputAction action) const
        {
            return actionsReleased[static_cast<int>(action)];
        }

        // Reset per-frame input states
        void ResetFrameStates()
        {
            for (int i = 0; i < static_cast<int>(InputAction::Count); ++i)
            {
                actions[i] = false;
                actionsReleased[i] = false;
            }
            pointerDeltaX = 0.0f;
            pointerDeltaY = 0.0f;
            scrollDelta = 0.0f;
        }
    };

    /**
     * Touch Control Layout (for mobile platforms)
     *
     * Defines virtual buttons and joystick regions for touchscreen controls.
     */
    struct TouchControlRegion
    {
        float x, y, width, height;  // Normalized screen coordinates (0-1)
        InputAction mappedAction;
        bool isJoystick;            // If true, this is a virtual joystick
    };

    /**
     * Gamepad Button Mapping
     *
     * Maps physical gamepad buttons to abstract input actions.
     * Each platform implements this mapping for their specific controller.
     */
    struct GamepadMapping
    {
        // Button indices vary by platform, but we define standard mappings
        // Platform layer translates native button IDs to InputActions

        // Example for standard gamepad:
        // Button 0 (A/Cross)     -> Action1
        // Button 1 (B/Circle)    -> Action2
        // Button 2 (X/Square)    -> Action3
        // Button 3 (Y/Triangle)  -> Action4
        // Button 4 (LB/L1)       -> CycleLeft
        // Button 5 (RB/R1)       -> CycleRight
        // Button 6 (Back/Select) -> MenuBack
        // Button 7 (Start)       -> MenuOpen
        // Button 8 (L3)          -> Crouch
        // Button 9 (R3)          -> Sprint

        static constexpr int MAX_BUTTONS = 32;
        InputAction buttonMapping[MAX_BUTTONS];

        GamepadMapping()
        {
            // Initialize all to Count (invalid)
            for (int i = 0; i < MAX_BUTTONS; ++i)
                buttonMapping[i] = InputAction::Count;
        }
    };
}
