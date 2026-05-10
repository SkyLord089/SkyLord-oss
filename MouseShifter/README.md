# Mouse Shifter for Sim Racing

A C++ application that turns your mouse into a virtual H-pattern gear shifter for sim racing games.

## Features

- **Virtual H-Pattern Shifter**: Hold Left Mouse Button to "grab" the stick, move your mouse to shift, release to engage gear
- **Always-On-Top Overlay**: Semi-transparent overlay in bottom-right corner (320px from right edge) showing:
  - H-pattern diagram with gear positions (R, 1-6, N)
  - Virtual stick position (red line and head)
  - Current hovered gear (yellow highlight)
  - Current engaged gear
  - Status indicator
- **Keyboard Simulation**: Automatically sends key presses (1-6, R, N) to your game
- **Hotkeys**:
  - `F12` - Toggle overlay on/off
  - `ESC` - Exit program

## How It Works

1. **Hold Left Mouse Button** - This "grabs" the virtual shifter stick
2. **Move your mouse** - The virtual stick follows your mouse movement (relative motion)
3. **Guide the stick** into the desired gear slot (shown on overlay)
4. **Release Left Mouse Button** - The gear engages and stick snaps to position
5. If released between slots, it defaults to **Neutral**

This mimics a real manual transmission shifter!

## Building in CLion

1. Open CLion
2. `File` → `Open` → Select the `MouseShifter` folder
3. Wait for CMake to configure
4. `Build` → `Build Project` (Ctrl+F9)
5. `Run` → `Run` (Shift+F10)

**Note**: This project uses Windows API and only compiles on Windows.

## Configuration

You can adjust these values in `src/MouseShifter.cpp`:

```cpp
const int MARGIN_RIGHT = 320;     // Distance from right screen edge
const int MARGIN_BOTTOM = 100;    // Distance from bottom screen edge
const float DEADZONE_RADIUS = 30.0f;  // Gear slot detection radius
const float CENTER_DEADZONE = 40.0f;  // Neutral zone radius
```

## Gear Layout

```
    R       1       3       5
            |       |       |
    --------N-------|-------|
            |       |       |
                    2       4       6
```

## Troubleshooting

### Overlay not visible in game
- The overlay uses `WS_EX_TOPMOST` to stay above all windows including borderless games
- Press F12 to toggle visibility
- Make sure the game is not running in exclusive fullscreen mode (use borderless windowed)

### Gears not shifting in game
- Check your game's key bindings - default keys are: 1-6, R, N
- Run the program as Administrator if needed
- Some anti-cheat systems may block input simulation

### Stick movement feels off
- Mouse movement is relative - small movements move the stick slightly
- Practice the H-pattern motions
- The stick snaps to gear centers when engaged for visual clarity

## Requirements

- Windows 10/11
- Visual Studio with C++ support (for building)
- CLion (optional, but recommended)
- CMake 3.15+

## License

Free to use for personal sim racing projects.
