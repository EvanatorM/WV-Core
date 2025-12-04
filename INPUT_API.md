# WillowVox Input API Documentation

The WillowVox engine provides a Unity-like input system that abstracts platform-specific input into a consistent API. All input is handled through three main classes:

## Core Classes

### 1. **InputManager** - Main Input API
The primary interface for all input queries. Works across all platforms (PC, mobile, consoles).

### 2. **GamepadInput** - Gamepad-Specific Features
Advanced gamepad features like individual stick values, triggers, and vibration.

### 3. **TouchInput** - Touch-Specific Features
Multi-touch support for mobile (Android, iOS) and Wii U GamePad.

---

## InputManager - Core Input API

### Button Input

```cpp
#include <wv/core.h>

using namespace WillowVox;

void Update(const InputState& input)
{
    // Check if jump button is held down
    if (InputManager::GetButton(InputAction::Jump))
    {
        player->Jump();
    }

    // Check if action button was pressed THIS FRAME
    if (InputManager::GetButtonDown(InputAction::Action1))
    {
        player->Attack();
    }

    // Check if button was released THIS FRAME
    if (InputManager::GetButtonUp(InputAction::Action2))
    {
        player->StopAiming();
    }
}
```

### Axis Input (Movement & Look)

```cpp
// Get individual axes by name
float horizontal = InputManager::GetAxis("MoveX");      // -1.0 to 1.0
float vertical = InputManager::GetAxis("MoveY");        // -1.0 to 1.0
float lookX = InputManager::GetAxis("LookX");
float lookY = InputManager::GetAxis("LookY");

// Or get as vectors (recommended)
glm::vec2 movement = InputManager::GetMovementVector();  // (x, y)
glm::vec2 look = InputManager::GetLookVector();          // (x, y)

// Apply to player
player->Move(movement.x, movement.y);
camera->Rotate(look.x, look.y);
```

**Supported Axis Names:**
- `"MoveX"` or `"Horizontal"` - Left/Right movement
- `"MoveY"` or `"Vertical"` - Forward/Backward movement
- `"LookX"` or `"Mouse X"` - Horizontal look/camera
- `"LookY"` or `"Mouse Y"` - Vertical look/camera

### Pointer/Mouse Input

```cpp
// Get pointer position (mouse or touch)
glm::vec2 pointerPos = InputManager::GetPointerPosition();

// Get pointer movement delta (useful for camera)
glm::vec2 pointerDelta = InputManager::GetPointerDelta();
camera->Rotate(pointerDelta.x * sensitivity, pointerDelta.y * sensitivity);

// Check if pointer/mouse button is down
if (InputManager::GetPointerDown())
{
    // Click or touch is active
}

// Get scroll wheel delta
float scroll = InputManager::GetScrollDelta();
camera->Zoom(scroll);
```

### Device Detection

```cpp
// Check what device is being used
InputDeviceType device = InputManager::GetDeviceType();

// Check for specific device types
if (InputManager::IsGamepad())
{
    // Show gamepad button prompts
    UI::ShowPrompt("Press A to continue");
}
else if (InputManager::IsTouchscreen())
{
    // Show touch controls
    UI::ShowVirtualButtons();
}

// Check if specific device
if (InputManager::IsDeviceType(InputDeviceType::XboxController))
{
    // Xbox-specific UI
}
```

### Convenience Methods

```cpp
// Check if player is trying to move
if (InputManager::IsMoving())
{
    player->PlayWalkAnimation();
}

// Check if player is looking around
if (InputManager::IsLooking())
{
    camera->UpdateRotation();
}
```

---

## GamepadInput - Gamepad Features

For more advanced gamepad control, use the `GamepadInput` class:

### Individual Stick Values

```cpp
#include <wv/input/GamepadInput.h>

// Get left stick (movement)
float leftX = GamepadInput::GetLeftStickX();
float leftY = GamepadInput::GetLeftStickY();
glm::vec2 leftStick = GamepadInput::GetLeftStick();

// Get right stick (camera/look)
float rightX = GamepadInput::GetRightStickX();
float rightY = GamepadInput::GetRightStickY();
glm::vec2 rightStick = GamepadInput::GetRightStick();
```

### Trigger Values

```cpp
// Get analog trigger values (0.0 to 1.0)
float leftTrigger = GamepadInput::GetTriggerLeft();
float rightTrigger = GamepadInput::GetTriggerRight();

// Use for vehicle acceleration
vehicle->Accelerate(rightTrigger);
vehicle->Brake(leftTrigger);

// Check if trigger exceeds threshold
if (GamepadInput::GetTriggerRightDown(0.5f))
{
    // Trigger pressed at least 50%
}
```

### D-Pad Input

```cpp
// Get D-Pad as vector
glm::vec2 dpad = GamepadInput::GetDPad();

// Or check individual directions
if (GamepadInput::GetDPadUp())    { menu->MoveUp(); }
if (GamepadInput::GetDPadDown())  { menu->MoveDown(); }
if (GamepadInput::GetDPadLeft())  { menu->MoveLeft(); }
if (GamepadInput::GetDPadRight()) { menu->MoveRight(); }
```

### Face Buttons (Position-Based)

```cpp
// These map to physical button locations, not labels
bool south = GamepadInput::GetButtonSouth();   // A / Cross (bottom)
bool east = GamepadInput::GetButtonEast();     // B / Circle (right)
bool west = GamepadInput::GetButtonWest();     // X / Square (left)
bool north = GamepadInput::GetButtonNorth();   // Y / Triangle (top)

// Useful for QTE sequences or when you need exact button positions
if (GamepadInput::GetButtonSouth())
{
    qte->PressCorrectButton();
}
```

### Shoulder Buttons

```cpp
bool lb = GamepadInput::GetShoulderLeft();   // LB / L1
bool rb = GamepadInput::GetShoulderRight();  // RB / R1
```

### Connection Status

```cpp
// Check if gamepad is connected
if (GamepadInput::IsConnected(0))
{
    // Player 1 gamepad connected
}

// Get gamepad type
InputDeviceType type = GamepadInput::GetGamepadType();
if (type == InputDeviceType::XboxController)
{
    UI::ShowXboxButtons();
}
else if (type == InputDeviceType::PSController)
{
    UI::ShowPlayStationButtons();
}
```

### Vibration/Rumble

```cpp
// Set vibration (platform-dependent)
GamepadInput::SetVibration(0.5f, 0.8f, 1.0f);  // (low motor, high motor, duration)

// Stop vibration
GamepadInput::StopVibration();
```

---

## TouchInput - Mobile & Touch Features

For touchscreen platforms (Android, iOS, Wii U GamePad):

### Basic Touch Input

```cpp
#include <wv/input/TouchInput.h>

// Check if touchscreen is supported
if (TouchInput::IsSupported())
{
    // Get number of active touches
    int touchCount = TouchInput::GetTouchCount();

    // Check if any finger is touching
    if (TouchInput::IsTouching())
    {
        // Handle touch
    }
}
```

### Primary Touch

```cpp
// Get the first/primary touch
Touch touch = TouchInput::GetPrimaryTouch();

if (touch.isActive)
{
    glm::vec2 position = touch.position;
    glm::vec2 delta = touch.deltaPosition;

    // Check touch phase
    if (touch.phase == Touch::Phase::Began)
    {
        // Touch just started
    }
    else if (touch.phase == Touch::Phase::Moved)
    {
        // Touch is moving
        camera->Pan(delta.x, delta.y);
    }
}
```

### Multi-Touch Support

```cpp
// Get all active touches
std::vector<Touch> touches = TouchInput::GetTouches();

for (const Touch& touch : touches)
{
    // Process each touch point
    UI::DrawTouchIndicator(touch.position);
}

// Or access by index
Touch touch0 = TouchInput::GetTouch(0);  // First touch
Touch touch1 = TouchInput::GetTouch(1);  // Second touch

// Pinch-to-zoom example
if (TouchInput::GetTouchCount() == 2)
{
    Touch t0 = TouchInput::GetTouch(0);
    Touch t1 = TouchInput::GetTouch(1);

    float distance = glm::distance(t0.position, t1.position);
    camera->Zoom(distance);
}
```

### Touch Phase Detection

```cpp
// Check for specific touch events
if (TouchInput::TouchBegan())
{
    // New touch detected this frame
}

if (TouchInput::TouchMoved())
{
    // Active touch moved this frame
}

if (TouchInput::TouchEnded())
{
    // Touch was released this frame
}
```

### Platform Features

```cpp
// Check capabilities
bool multiTouch = TouchInput::SupportsMultiTouch();
int maxTouches = TouchInput::GetMaxTouchCount();

// Wii U GamePad specific
if (TouchInput::IsWiiUGamePad())
{
    // Use GamePad-specific features
}
```

---

## Platform-Specific Notes

### Desktop (Windows/Linux/macOS)
- **Movement**: WASD or arrow keys → `MoveX/MoveY`
- **Look**: Mouse movement → `LookX/LookY`
- **Actions**: Mouse buttons, Space, etc. → `Action1/2/3, Jump`
- **Gamepad**: XInput controllers supported

### Mobile (Android/iOS)
- **Movement**: Virtual joystick (left side) → `MoveX/MoveY`
- **Look**: Touch drag (right side) → `LookX/LookY`
- **Actions**: Virtual buttons → `Action1/2, Jump`
- **Multi-touch**: Supported via `TouchInput`

### PlayStation (PS3/PS4)
- **Movement**: Left analog stick → `MoveX/MoveY`
- **Look**: Right analog stick → `LookX/LookY`
- **Actions**: DualShock buttons → `Action1/2/3/4`
- **Triggers**: L2/R2 → `Crouch/Sprint`

### Xbox Series X/S
- **Movement**: Left analog stick → `MoveX/MoveY`
- **Look**: Right analog stick → `LookX/LookY`
- **Actions**: ABXY buttons → `Action1/2/3/4`
- **Triggers**: LT/RT → `Crouch/Sprint`

### Nintendo Switch
- **Movement**: Left analog stick → `MoveX/MoveY`
- **Look**: Right analog stick → `LookX/LookY`
- **Actions**: ABXY buttons → `Action1/2/3/4`
- **Triggers**: ZL/ZR → `Crouch/Sprint`

### Nintendo Wii
- **Movement**: Nunchuk joystick or D-pad → `MoveX/MoveY`
- **Look**: Wiimote IR pointer → `LookX/LookY`
- **Actions**: A/B/1/2 buttons → `Action1/2/3/4`

### Nintendo GameCube
- **Movement**: Control stick → `MoveX/MoveY`
- **Look**: C-stick → `LookX/LookY`
- **Actions**: ABXY buttons → `Action1/2/3/4`
- **Triggers**: L/R analog triggers supported

### Nintendo Wii U
- **Movement**: Left analog stick → `MoveX/MoveY`
- **Look**: Right analog stick → `LookX/LookY`
- **Actions**: ABXY buttons → `Action1/2/3/4`
- **Touchscreen**: GamePad touchscreen via `TouchInput`

---

## Complete Usage Example

```cpp
#include <wv/core.h>

using namespace WillowVox;

class MyGame : public App
{
public:
    void Start() override
    {
        Logger::Log("Game Started!");
    }

    void Update(const InputState& input) override
    {
        // === Movement ===
        glm::vec2 movement = InputManager::GetMovementVector();
        player->Move(movement * speed * App::m_deltaTime);

        // === Camera/Look ===
        glm::vec2 look = InputManager::GetLookVector();
        camera->Rotate(look * sensitivity);

        // === Actions ===
        if (InputManager::GetButtonDown(InputAction::Jump))
        {
            player->Jump();
        }

        if (InputManager::GetButton(InputAction::Action1))
        {
            player->PrimaryAction();
        }

        if (InputManager::GetButtonDown(InputAction::MenuOpen))
        {
            menu->Toggle();
        }

        // === Platform-Specific ===
        if (InputManager::IsGamepad())
        {
            // Advanced gamepad features
            float rightTrigger = GamepadInput::GetTriggerRight();
            if (rightTrigger > 0.5f)
            {
                player->Sprint();
            }
        }
        else if (InputManager::IsTouchscreen())
        {
            // Touch-specific features
            int touchCount = TouchInput::GetTouchCount();
            if (touchCount == 2)
            {
                // Pinch to zoom
                Touch t0 = TouchInput::GetTouch(0);
                Touch t1 = TouchInput::GetTouch(1);
                float dist = glm::distance(t0.position, t1.position);
                camera->Zoom(dist);
            }
        }
    }

    void Render() override
    {
        // Your rendering code
    }

private:
    Player* player;
    Camera* camera;
    Menu* menu;
    float speed = 5.0f;
    float sensitivity = 2.0f;
};
```

---

## InputAction Reference

All abstract actions available in `InputAction` enum:

| Action | Description | Common Mapping |
|--------|-------------|----------------|
| `MoveForward` | Move forward | W, Up, D-pad Up, Left Stick Up |
| `MoveBackward` | Move backward | S, Down, D-pad Down, Left Stick Down |
| `MoveLeft` | Move left | A, Left, D-pad Left, Left Stick Left |
| `MoveRight` | Move right | D, Right, D-pad Right, Left Stick Right |
| `Action1` | Primary action | Mouse Left, A/Cross, Touch Button |
| `Action2` | Secondary action | Mouse Right, B/Circle |
| `Action3` | Tertiary action | X/Square |
| `Action4` | Quaternary action | Y/Triangle |
| `Jump` | Jump | Space, A/Cross |
| `Crouch` | Crouch | Ctrl, L2/LT, Z |
| `Sprint` | Sprint | Shift, R2/RT |
| `MenuOpen` | Open menu | Esc, Start, Plus |
| `MenuBack` | Back/Cancel | Esc, B/Circle, Minus |
| `CycleLeft` | Cycle items left | Q, LB/L1 |
| `CycleRight` | Cycle items right | E, RB/R1 |

---

## Best Practices

1. **Always use InputManager for basic input** - It works across all platforms
2. **Use GamepadInput only when you need advanced gamepad features** (triggers, vibration)
3. **Use TouchInput only for multi-touch or touch-specific features**
4. **Detect device type to show appropriate UI prompts** (keyboard vs gamepad vs touch)
5. **Test on all target platforms** - Input behavior may vary slightly
6. **Use GetButtonDown for single-press actions** (menus, attacks)
7. **Use GetButton for continuous actions** (movement, aiming)
8. **Normalize and multiply by deltaTime for frame-independent movement**

---

## Migration from Old Input System

If you're using the old `Input` class (keyboard/mouse only):

```cpp
// OLD (Desktop only)
if (Input::GetKey(Key::W))
    player->Move(0, 1);

// NEW (All platforms)
glm::vec2 movement = InputManager::GetMovementVector();
player->Move(movement.x, movement.y);
```

```cpp
// OLD (Desktop only)
glm::vec2 mouseDelta = Input::GetMouseDelta();

// NEW (All platforms)
glm::vec2 lookDelta = InputManager::GetLookVector();
```

The new API is **backward compatible** - you can still use the old `Input` class for desktop-specific features, but `InputManager` is recommended for cross-platform projects.
