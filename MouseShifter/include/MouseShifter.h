#ifndef MOUSE_SHIFTER_H
#define MOUSE_SHIFTER_H

#include <windows.h>
#include <string>
#include <vector>
#include <functional>

// Structure for defining gear zone
struct GearZone {
    int id;                 // Gear ID (0 - neutral, 1-6 - gears, -1 - reverse)
    std::wstring name;      // Gear name
    RECT rect;              // Rectangular zone on screen
    bool active;            // Whether zone is active
    
    GearZone() : id(0), active(false) {
        rect = {0, 0, 0, 0};
    }
    
    GearZone(int gearId, const std::wstring& gearName, int x, int y, int width, int height)
        : id(gearId), name(gearName), active(false) {
        rect.left = x;
        rect.top = y;
        rect.right = x + width;
        rect.bottom = y + height;
    }
    
    // Check if point is inside zone
    bool contains(int x, int y) const {
        return x >= rect.left && x <= rect.right && 
               y >= rect.top && y <= rect.bottom;
    }
};

// Class for program control
class MouseShifter {
public:
    MouseShifter();
    ~MouseShifter();
    
    // Initialize gear zones
    void initGearZones();
    
    // Main processing loop
    void run();
    
    // Stop the program
    void stop();
    
    // Set zone for specific gear
    void setGearZone(int gearId, int x, int y, int width, int height);
    
    // Get current gear
    int getCurrentGear() const { return currentGear; }
    
    // Configure hotkeys
    void setToggleKey(int vkCode) { toggleKey = vkCode; }
    void setExitKey(int vkCode) { exitKey = vkCode; }

private:
    std::vector<GearZone> gearZones;
    int currentGear;
    bool running;
    int toggleKey;      // Toggle key (default F12)
    int exitKey;        // Exit key (default ESC)
    bool enabled;       // Whether shifter is enabled
    
    // Handle mouse movement
    void processMouseMove();
    
    // Shift gear
    void shiftGear(int newGear);
    
    // Draw overlay
    void drawOverlay();
    
    // Get mouse position
    POINT getMousePosition();
    
    // Virtual key press (for shift simulation)
    void simulateKeyPress(int keyCode);
};

#endif // MOUSE_SHIFTER_H
