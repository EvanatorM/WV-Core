#include "DesktopPlatform.h"
#include <wv/Logger.h>
#include <GLFW/glfw3.h>
#include <cstring>

namespace WillowVox
{
    // GLFW scroll callback
    static double g_scrollX = 0.0;
    static double g_scrollY = 0.0;

    static void GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        g_scrollX += xoffset;
        g_scrollY += yoffset;
    }

    DesktopPlatform::DesktopPlatform()
    {
    }

    DesktopPlatform::~DesktopPlatform()
    {
        Shutdown();
    }

    bool DesktopPlatform::Initialize()
    {
        Logger::EngineLog("Initializing Desktop Platform (%s)", PLATFORM_NAME);

        m_graphicsContext = std::make_unique<DesktopGraphicsContext>();

        // Graphics context will initialize GLFW
        return true;
    }

    void DesktopPlatform::Shutdown()
    {
        m_graphicsContext.reset();
        Logger::EngineLog("Desktop Platform shutdown");
    }

    void DesktopPlatform::ProcessEvents()
    {
        glfwPollEvents();

        // Store current scroll values
        m_scrollX = g_scrollX;
        m_scrollY = g_scrollY;

        // Reset scroll accumulator for next frame
        g_scrollX = 0.0;
        g_scrollY = 0.0;
    }

    void DesktopPlatform::PollInput(InputState& outInputState)
    {
        GLFWwindow* window = m_graphicsContext->GetGLFWWindow();
        if (!window)
            return;

        // Setup scroll callback if not already set
        static bool scrollCallbackSet = false;
        if (!scrollCallbackSet)
        {
            glfwSetScrollCallback(window, GLFWScrollCallback);
            scrollCallbackSet = true;
        }

        // Update keyboard
        UpdateKeyboardInput(outInputState);

        // Update mouse
        UpdateMouseInput(outInputState);

        // Update gamepad (if connected)
        UpdateGamepadInput(outInputState);

        // Store previous states
        std::memcpy(m_keyPrevStates, m_keyStates, sizeof(m_keyStates));
        std::memcpy(m_mouseButtonPrevStates, m_mouseButtonStates, sizeof(m_mouseButtonStates));
    }

    void DesktopPlatform::ResetInputFrameState(InputState& inputState)
    {
        inputState.ResetFrameStates();
        m_scrollX = 0.0;
        m_scrollY = 0.0;
    }

    void DesktopPlatform::UpdateKeyboardInput(InputState& outInputState)
    {
        GLFWwindow* window = m_graphicsContext->GetGLFWWindow();

        // Read key states
        m_keyStates[GLFW_KEY_W] = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
        m_keyStates[GLFW_KEY_S] = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
        m_keyStates[GLFW_KEY_A] = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
        m_keyStates[GLFW_KEY_D] = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
        m_keyStates[GLFW_KEY_E] = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
        m_keyStates[GLFW_KEY_Q] = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
        m_keyStates[GLFW_KEY_SPACE] = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        m_keyStates[GLFW_KEY_LEFT_SHIFT] = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        m_keyStates[GLFW_KEY_LEFT_CONTROL] = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
        m_keyStates[GLFW_KEY_ESCAPE] = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
        m_keyStates[GLFW_KEY_ENTER] = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;
        m_keyStates[GLFW_KEY_TAB] = glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;

        // Map to InputState actions

        // Movement (digital)
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveForward)] = m_keyStates[GLFW_KEY_W];
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveBackward)] = m_keyStates[GLFW_KEY_S];
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveLeft)] = m_keyStates[GLFW_KEY_A];
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveRight)] = m_keyStates[GLFW_KEY_D];
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveUp)] = m_keyStates[GLFW_KEY_E];
        outInputState.actionsHeld[static_cast<int>(InputAction::MoveDown)] = m_keyStates[GLFW_KEY_Q];

        // Pressed this frame
        outInputState.actions[static_cast<int>(InputAction::MoveForward)] =
            m_keyStates[GLFW_KEY_W] && !m_keyPrevStates[GLFW_KEY_W];
        outInputState.actions[static_cast<int>(InputAction::MoveBackward)] =
            m_keyStates[GLFW_KEY_S] && !m_keyPrevStates[GLFW_KEY_S];
        outInputState.actions[static_cast<int>(InputAction::MoveLeft)] =
            m_keyStates[GLFW_KEY_A] && !m_keyPrevStates[GLFW_KEY_A];
        outInputState.actions[static_cast<int>(InputAction::MoveRight)] =
            m_keyStates[GLFW_KEY_D] && !m_keyPrevStates[GLFW_KEY_D];

        // Menu
        outInputState.actions[static_cast<int>(InputAction::MenuOpen)] =
            m_keyStates[GLFW_KEY_ESCAPE] && !m_keyPrevStates[GLFW_KEY_ESCAPE];
        outInputState.actionsHeld[static_cast<int>(InputAction::MenuOpen)] = m_keyStates[GLFW_KEY_ESCAPE];

        outInputState.actions[static_cast<int>(InputAction::MenuConfirm)] =
            m_keyStates[GLFW_KEY_ENTER] && !m_keyPrevStates[GLFW_KEY_ENTER];

        // Other actions
        outInputState.actionsHeld[static_cast<int>(InputAction::Jump)] = m_keyStates[GLFW_KEY_SPACE];
        outInputState.actionsHeld[static_cast<int>(InputAction::Crouch)] = m_keyStates[GLFW_KEY_LEFT_SHIFT];
        outInputState.actionsHeld[static_cast<int>(InputAction::Sprint)] = m_keyStates[GLFW_KEY_LEFT_CONTROL];

        // Analog axes from digital input
        outInputState.moveAxisX = 0.0f;
        outInputState.moveAxisY = 0.0f;
        if (m_keyStates[GLFW_KEY_W]) outInputState.moveAxisY += 1.0f;
        if (m_keyStates[GLFW_KEY_S]) outInputState.moveAxisY -= 1.0f;
        if (m_keyStates[GLFW_KEY_D]) outInputState.moveAxisX += 1.0f;
        if (m_keyStates[GLFW_KEY_A]) outInputState.moveAxisX -= 1.0f;

        // Device type
        outInputState.deviceType = InputDeviceType::KeyboardMouse;
    }

    void DesktopPlatform::UpdateMouseInput(InputState& outInputState)
    {
        GLFWwindow* window = m_graphicsContext->GetGLFWWindow();

        // Get mouse position
        glfwGetCursorPos(window, &m_mouseX, &m_mouseY);

        // Calculate delta
        outInputState.pointerDeltaX = static_cast<float>(m_mouseX - m_prevMouseX);
        outInputState.pointerDeltaY = static_cast<float>(m_mouseY - m_prevMouseY);
        m_prevMouseX = m_mouseX;
        m_prevMouseY = m_mouseY;

        outInputState.pointerX = static_cast<float>(m_mouseX);
        outInputState.pointerY = static_cast<float>(m_mouseY);

        // Look axes (from mouse delta)
        outInputState.lookAxisX = outInputState.pointerDeltaX;
        outInputState.lookAxisY = outInputState.pointerDeltaY;

        // Mouse buttons
        m_mouseButtonStates[0] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        m_mouseButtonStates[1] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        m_mouseButtonStates[2] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;

        // Map to actions
        outInputState.actionsHeld[static_cast<int>(InputAction::Action1)] = m_mouseButtonStates[0];
        outInputState.actionsHeld[static_cast<int>(InputAction::Action2)] = m_mouseButtonStates[1];
        outInputState.actionsHeld[static_cast<int>(InputAction::Action3)] = m_mouseButtonStates[2];

        // Pressed this frame
        outInputState.actions[static_cast<int>(InputAction::Action1)] =
            m_mouseButtonStates[0] && !m_mouseButtonPrevStates[0];
        outInputState.actions[static_cast<int>(InputAction::Action2)] =
            m_mouseButtonStates[1] && !m_mouseButtonPrevStates[1];
        outInputState.actions[static_cast<int>(InputAction::Action3)] =
            m_mouseButtonStates[2] && !m_mouseButtonPrevStates[2];

        // Released this frame
        outInputState.actionsReleased[static_cast<int>(InputAction::Action1)] =
            !m_mouseButtonStates[0] && m_mouseButtonPrevStates[0];
        outInputState.actionsReleased[static_cast<int>(InputAction::Action2)] =
            !m_mouseButtonStates[1] && m_mouseButtonPrevStates[1];

        // Scroll
        outInputState.scrollDelta = static_cast<float>(m_scrollY);

        // Scroll for cycling
        if (m_scrollY > 0.1)
            outInputState.actions[static_cast<int>(InputAction::CycleRight)] = true;
        else if (m_scrollY < -0.1)
            outInputState.actions[static_cast<int>(InputAction::CycleLeft)] = true;

        // Pointer down
        outInputState.pointerDown = m_mouseButtonStates[0];
    }

    void DesktopPlatform::UpdateGamepadInput(InputState& outInputState)
    {
        // Check if gamepad is connected
        if (glfwJoystickPresent(GLFW_JOYSTICK_1))
        {
            GLFWgamepadstate state;
            if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
            {
                // Override device type
                outInputState.deviceType = InputDeviceType::Gamepad;

                // Axes
                outInputState.moveAxisX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
                outInputState.moveAxisY = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
                outInputState.lookAxisX = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
                outInputState.lookAxisY = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];

                // Buttons
                outInputState.actionsHeld[static_cast<int>(InputAction::Action1)] =
                    state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS;
                outInputState.actionsHeld[static_cast<int>(InputAction::Action2)] =
                    state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS;
                outInputState.actionsHeld[static_cast<int>(InputAction::Action3)] =
                    state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS;
                outInputState.actionsHeld[static_cast<int>(InputAction::Action4)] =
                    state.buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS;

                outInputState.actionsHeld[static_cast<int>(InputAction::Jump)] =
                    state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS;

                outInputState.actionsHeld[static_cast<int>(InputAction::MenuOpen)] =
                    state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS;
                outInputState.actionsHeld[static_cast<int>(InputAction::MenuBack)] =
                    state.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS;

                outInputState.actionsHeld[static_cast<int>(InputAction::CycleLeft)] =
                    state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS;
                outInputState.actionsHeld[static_cast<int>(InputAction::CycleRight)] =
                    state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS;
            }
        }
    }

    IGraphicsContext* DesktopPlatform::GetGraphicsContext()
    {
        return m_graphicsContext.get();
    }

    const char* DesktopPlatform::GetUserDataPath() const
    {
        // Platform-specific user data paths
#if defined(PLATFORM_WINDOWS)
        // Windows: %APPDATA%\WillowVox
        static const char* path = "%APPDATA%\\WillowVox";
#elif defined(PLATFORM_MACOS)
        // macOS: ~/Library/Application Support/WillowVox
        static const char* path = "~/Library/Application Support/WillowVox";
#else
        // Linux: ~/.local/share/WillowVox
        static const char* path = "~/.local/share/WillowVox";
#endif
        return path;
    }

    const char* DesktopPlatform::GetAssetsPath() const
    {
        return "./assets";
    }

    const char* DesktopPlatform::GetPlatformName() const
    {
        return PLATFORM_NAME;
    }

    InputDeviceType DesktopPlatform::GetPrimaryInputDevice() const
    {
        return InputDeviceType::KeyboardMouse;
    }

    bool DesktopPlatform::HasFeature(const char* featureName) const
    {
        // Desktop has mouse, keyboard, filesystem, etc.
        if (strcmp(featureName, "mouse") == 0) return true;
        if (strcmp(featureName, "keyboard") == 0) return true;
        if (strcmp(featureName, "gamepad") == 0) return true;
        if (strcmp(featureName, "filesystem") == 0) return true;
        return false;
    }

    void DesktopPlatform::SetVibration(int playerIndex, float lowFrequency, float highFrequency)
    {
#if defined(PLATFORM_WINDOWS)
        // Windows: Use XInput for gamepad vibration
        // Note: This requires linking against Xinput.lib
        // For now, GLFW doesn't expose vibration API, so we'd need platform-specific code
        // This is a placeholder - actual implementation would use XInput directly
        Logger::EngineLog("Vibration requested (Windows XInput not yet implemented)");
#else
        // Linux/macOS: GLFW doesn't support vibration
        // Would need to use platform-specific APIs (evdev on Linux, IOKit on macOS)
        Logger::EngineLog("Vibration not supported on this platform");
#endif
    }

    void DesktopPlatform::StopVibration(int playerIndex)
    {
        // Stop vibration by setting both motors to 0
        SetVibration(playerIndex, 0.0f, 0.0f);
    }
}
