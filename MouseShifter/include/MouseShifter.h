#ifndef MOUSE_SHIFTER_H
#define MOUSE_SHIFTER_H

#include <windows.h>
#include <string>
#include <vector>
#include <map>

enum Gear {
    GEAR_NONE = -2,
    GEAR_INVALID = -1,
    GEAR_N = 0,
    GEAR_1 = 1,
    GEAR_2 = 2,
    GEAR_3 = 3,
    GEAR_4 = 4,
    GEAR_5 = 5,
    GEAR_6 = 6,
    GEAR_R = 7
};

struct GearSlot {
    float x;
    float y;
};

class MouseShifter {
public:
    MouseShifter();
    ~MouseShifter() = default;
    
    void initOverlay();
    void run();
    int getCurrentGear() const { return currentGear; }
    void toggleEnabled() { isEnabled = !isEnabled; }
    bool isEnabledState() const { return isEnabled; }

private:
    Gear currentGear;
    Gear currentHoveredGear;
    bool isEnabled;
    bool isGrabbing;
    HWND hwndOverlay;
    bool needsRedraw;
    float virtualStickX;
    float virtualStickY;
    std::map<Gear, GearSlot> gearSlots;
    DWORD lastGearSendTime;
    
    void handleInput();
    void updateShifterLogic();
    Gear detectHoveredGear();
    void engageGear();
    void renderOverlay();
    void sendKeyPress(Gear gear);
    const wchar_t* getGearName(Gear gear);
    static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif
