# Mouse Shifter for Sim Racing

A program that creates a virtual gearbox controlled by mouse movement.

## 🎮 Features

- **Mouse-controlled gear shifting** - move cursor over gear zone to shift
- **Visual Overlay** - displays current gear, status, and zones in bottom-right corner
- **Hotkeys**:
  - `F12` - toggle shifter on/off
  - `ESC` - exit program
- **Automatic key simulation** - program sends key presses (1-6, R, N) to the game
- **H-pattern gearbox layout** - classic gear arrangement

## 📋 Requirements

- Windows 7/8/10/11
- CMake 3.15 or higher
- C++17 compatible compiler (MSVC, MinGW, etc.)
- CLion (recommended)

## 🚀 Installation and Build in CLion

### Option 1: Open as CMake Project

1. Launch CLion
2. Select `File` → `Open`
3. Specify the `MouseShifter` folder
4. CLion will automatically detect CMakeLists.txt
5. Wait for CMake indexing to complete
6. Press `Build` → `Build Project` (or Ctrl+F9)
7. Run via `Run` → `Run` (or Shift+F10)

### Option 2: Manual Console Build

```bash
cd MouseShifter
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

The compiled file will be in `build/bin/MouseShifter.exe`

## 🎯 How to Use

1. **Launch the program** - a console window will open
2. **Press F12** - to enable the shifter (you'll see "Shifter ENABLED" message)
3. **Move mouse to bottom-right corner** - gear zones will be there
4. **Hover cursor over zones** - gear will shift automatically
5. **Press ESC** - to exit the program

## 📐 Zone Layout

Gear zones are located in the bottom-right corner of the screen:

```
R   1   3   5   (top row)
    2   4   6   (bottom row)
        N       (neutral - center)
```

Each zone is highlighted:
- **Green** - inactive zone
- **Yellow** - active zone (cursor inside)

## ⚙️ Configuration

### Changing Gear Zones

In `src/MouseShifter.cpp`, function `initGearZones()`, you can modify:
- Zone sizes (`zoneWidth`, `zoneHeight`)
- Zone positions
- Number of gears

Example of changing 1st gear zone position:
```cpp
// Before
gearZones.emplace_back(1, L"1", startX, startY + zoneHeight * 1, zoneWidth, zoneHeight);

// After (new position x=100, y=200, width=100, height=60)
setGearZone(1, 100, 200, 100, 60);
```

### Changing Hotkeys

In the `MouseShifter` class constructor:
```cpp
MouseShifter::MouseShifter() 
    : currentGear(0), running(false), 
      toggleKey(VK_F12),  // Change to another key
      exitKey(VK_ESCAPE), // Change to another key
      enabled(false) {
    initGearZones();
}
```

Virtual key codes reference: [VK Codes](https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes)

### Configuring Key Simulation

In the `shiftGear()` function, you can change which keys are sent to the game:

```cpp
// Example for different key layout
if (newGear > 0 && newGear <= 6) {
    int keyCodes[] = {'Q', 'W', 'E', 'A', 'S', 'D'}; // Q,W,E,A,S,D keys
    simulateKeyPress(keyCodes[newGear - 1]);
}
```

## 🔧 Technical Details

### Project Structure

```
MouseShifter/
├── CMakeLists.txt          # CMake configuration
├── include/
│   └── MouseShifter.h      # Class header file
├── src/
│   ├── main.cpp            # Entry point
│   └── MouseShifter.cpp    # Class implementation
└── README.md               # Documentation
```

### Windows API Used

- `GetCursorPos()` - get mouse position
- `GetDC()` - get device context for drawing
- `TextOutW()` - output text to screen
- `Rectangle()` - draw rectangles
- `SendInput()` - simulate key presses
- `GetAsyncKeyState()` - check key states

## ⚠️ Important Notes

1. **Antivirus** - the program uses low-level Windows calls, which may trigger false positives. Add the project to exceptions.

2. **Run as Administrator** - overlay may require administrator privileges to work correctly.

3. **Fullscreen Mode** - in some games, overlay may not display in exclusive fullscreen mode. Try Borderless Window mode.

4. **Performance** - program updates every 10ms. If experiencing performance issues, you can increase the delay in the `run()` function.

## 🛠️ Troubleshooting

### Overlay Not Displaying
- Run the program as administrator
- Check that the game is not in exclusive fullscreen mode

### Gears Not Shifting
- Make sure shifter is enabled (press F12)
- Check that cursor is inside the gear zone

### Game Not Responding to Shifts
- Check game control settings
- Change key codes in `shiftGear()` function
- Ensure the game is actively accepting keyboard input

## 📝 License

This project was created for educational purposes to enhance Sim Racing experience.

## 🤝 Contributing

Feel free to improve the code for your needs! Main ideas for enhancement:
- Add configuration file (JSON/XML)
- GUI for zone configuration
- Support for different gearbox layouts (sequential, DSG, etc.)
- Integration with specific games (Assetto Corsa, iRacing, etc.)

---

**Happy Racing! 🏁**
