#include "MouseShifter.h"
#include <iostream>
#include <thread>
#include <chrono>

MouseShifter::MouseShifter() 
    : currentGear(0), running(false), toggleKey(VK_F12), exitKey(VK_ESCAPE), enabled(false) {
    initGearZones();
}

MouseShifter::~MouseShifter() {
    stop();
}

void MouseShifter::initGearZones() {
    // Get screen dimensions for automatic zone setup
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    // Zones will be located in the bottom right corner, but offset from the edge
    int zoneWidth = 80;
    int zoneHeight = 50;
    int marginX = 120;  // Distance from right edge (increased for better positioning)
    int marginY = 120;  // Distance from bottom edge (increased for better positioning)
    int startX = screenWidth - zoneWidth - marginX;
    int startY = screenHeight - zoneHeight * 3 - marginY;
    
    // H-pattern gearbox layout (compact 3x3):
    // R   1   3
    //     N   4
    //     2   5   6
    
    // Reverse gear (top left corner)
    gearZones.emplace_back(-1, L"R", startX, startY, zoneWidth, zoneHeight);
    
    // 1st gear (right of R)
    gearZones.emplace_back(1, L"1", startX + zoneWidth + 10, startY, zoneWidth, zoneHeight);
    
    // 3rd gear (right of 1)
    gearZones.emplace_back(3, L"3", startX + (zoneWidth + 10) * 2, startY, zoneWidth, zoneHeight);
    
    // Neutral (center, under 1)
    gearZones.emplace_back(0, L"N", startX + zoneWidth + 10, startY + zoneHeight + 10, zoneWidth, zoneHeight);
    
    // 2nd gear (under 1)
    gearZones.emplace_back(2, L"2", startX + zoneWidth + 10, startY + (zoneHeight + 10) * 2, zoneWidth, zoneHeight);
    
    // 4th gear (right of N)
    gearZones.emplace_back(4, L"4", startX + (zoneWidth + 10) * 2, startY + zoneHeight + 10, zoneWidth, zoneHeight);
    
    // 5th gear (under 3)
    gearZones.emplace_back(5, L"5", startX + (zoneWidth + 10) * 2, startY + (zoneHeight + 10) * 2, zoneWidth, zoneHeight);
    
    // 6th gear (right of 5)
    gearZones.emplace_back(6, L"6", startX + (zoneWidth + 10) * 3, startY + (zoneHeight + 10) * 2, zoneWidth, zoneHeight);
}

void MouseShifter::setGearZone(int gearId, int x, int y, int width, int height) {
    for (auto& zone : gearZones) {
        if (zone.id == gearId) {
            zone.rect.left = x;
            zone.rect.top = y;
            zone.rect.right = x + width;
            zone.rect.bottom = y + height;
            break;
        }
    }
}

POINT MouseShifter::getMousePosition() {
    POINT point;
    GetCursorPos(&point);
    return point;
}

void MouseShifter::simulateKeyPress(int keyCode) {
    // Simulate key press (can be used to send commands to the game)
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = keyCode;
    
    // Key down
    SendInput(1, &input, sizeof(INPUT));
    
    // Small delay
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // Key up
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

void MouseShifter::shiftGear(int newGear) {
    if (newGear == currentGear) {
        return;
    }
    
    currentGear = newGear;
    
    // Output to console
    std::wcout << L"Gear: ";
    if (newGear == 0) {
        std::wcout << L"Neutral";
    } else if (newGear == -1) {
        std::wcout << L"Reverse";
    } else {
        std::wcout << newGear;
    }
    std::wcout << std::endl;
    
    // Simulate key press for the game
    // For example, if the game uses numbers 1-6 for gears:
    if (newGear > 0 && newGear <= 6) {
        int keyCodes[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36}; // Keys 1-6
        simulateKeyPress(keyCodes[newGear - 1]);
    } else if (newGear == -1) {
        simulateKeyPress('R'); // R key for reverse
    } else if (newGear == 0) {
        simulateKeyPress('N'); // N key for neutral
    }
}

void MouseShifter::processMouseMove() {
    POINT mousePos = getMousePosition();
    
    for (const auto& zone : gearZones) {
        if (zone.contains(mousePos.x, mousePos.y)) {
            if (!zone.active) {
                // Mouse entered the zone
                shiftGear(zone.id);
                
                // Mark all zones as inactive
                for (auto& z : gearZones) {
                    z.active = false;
                }
                
                // Activate current zone
                const_cast<GearZone&>(zone).active = true;
            }
            break;
        }
    }
}

void MouseShifter::drawOverlay() {
    // Get device context for the entire screen
    HDC hdcScreen = GetDC(NULL);
    
    if (hdcScreen) {
        // Get screen dimensions
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        
        // Text settings
        SetBkMode(hdcScreen, TRANSPARENT);
        SetTextColor(hdcScreen, RGB(0, 255, 0)); // Green color
        
        // Draw gear zones
        HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
        HBRUSH brush = CreateSolidBrush(RGB(0, 100, 0));
        HPEN oldPen = (HPEN)SelectObject(hdcScreen, pen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdcScreen, brush);
        
        for (const auto& zone : gearZones) {
            // Draw zone rectangle
            Rectangle(hdcScreen, zone.rect.left, zone.rect.top, zone.rect.right, zone.rect.bottom);
            
            // Display gear name
            SetTextColor(hdcScreen, zone.active ? RGB(255, 255, 0) : RGB(0, 255, 0));
            TextOutW(hdcScreen, 
                     zone.rect.left + 30, 
                     zone.rect.top + 15, 
                     zone.name.c_str(), 
                     zone.name.length());
        }
        
        // Restore old objects
        SelectObject(hdcScreen, oldPen);
        SelectObject(hdcScreen, oldBrush);
        
        // Delete created objects
        DeleteObject(pen);
        DeleteObject(brush);
        
        ReleaseDC(NULL, hdcScreen);
    }
}

void MouseShifter::run() {
    running = true;
    enabled = false;
    
    std::wcout << L"=== Mouse Shifter for Sim Racing ===" << std::endl;
    std::wcout << L"F12 - Toggle shifter ON/OFF" << std::endl;
    std::wcout << L"ESC - Exit program" << std::endl;
    std::wcout << L"Move mouse over gear zone to shift" << std::endl;
    std::wcout << std::endl;
    
    bool lastToggleState = false;
    bool lastExitState = false;
    
    while (running) {
        // Check hotkeys
        bool togglePressed = (GetAsyncKeyState(toggleKey) & 0x8000) != 0;
        bool exitPressed = (GetAsyncKeyState(exitKey) & 0x8000) != 0;
        
        // Handle toggle key
        if (togglePressed && !lastToggleState) {
            enabled = !enabled;
            std::wcout << (enabled ? L"Shifter ENABLED" : L"Shifter DISABLED") << std::endl;
        }
        
        // Handle exit key
        if (exitPressed && !lastExitState) {
            running = false;
            break;
        }
        
        lastToggleState = togglePressed;
        lastExitState = exitPressed;
        
        // If shifter is enabled, process mouse movement
        if (enabled) {
            processMouseMove();
        }
        
        // Draw overlay
        drawOverlay();
        
        // Small delay to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void MouseShifter::stop() {
    running = false;
}
