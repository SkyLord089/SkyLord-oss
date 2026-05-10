#include "MouseShifter.h"
#include <iostream>
#include <cmath>

const int OVERLAY_WIDTH = 400;
const int OVERLAY_HEIGHT = 350;
const int MARGIN_RIGHT = 320;
const int MARGIN_BOTTOM = 100;

const float DEADZONE_RADIUS = 30.0f;
const float CENTER_DEADZONE = 40.0f;

MouseShifter::MouseShifter() {
    currentGear = GEAR_N;
    isEnabled = false;
    isGrabbing = false;
    currentHoveredGear = GEAR_NONE;
    virtualStickX = 0.0f;
    virtualStickY = 0.0f;
    lastGearSendTime = 0;
    
    float slotDistX = 60.0f;
    float slotDistY = 60.0f;
    
    gearSlots[GEAR_1] = { -slotDistX, -slotDistY };
    gearSlots[GEAR_2] = { -slotDistX,  slotDistY };
    gearSlots[GEAR_3] = {  0.0f,      -slotDistY };
    gearSlots[GEAR_4] = {  0.0f,       slotDistY };
    gearSlots[GEAR_5] = {  slotDistX, -slotDistY };
    gearSlots[GEAR_6] = {  slotDistX,  slotDistY };
    gearSlots[GEAR_R] = { -slotDistX * 1.8f, -slotDistY };
    gearSlots[GEAR_N] = {  0.0f,       0.0f };
}

void MouseShifter::initOverlay() {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, wndProc, 0L, 0L,
                      GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr,
                      L"MouseShifterOverlay", nullptr };
    RegisterClassEx(&wc);

    hwndOverlay = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST, // TOPMOST ensures it's above everything
        wc.lpszClassName, L"Sim Racing Shifter Overlay",
        WS_POPUP,
        0, 0, OVERLAY_WIDTH, OVERLAY_HEIGHT,
        nullptr, nullptr, wc.hInstance, nullptr
    );

    if (hwndOverlay) {
        int posX = GetSystemMetrics(SM_CXSCREEN) - MARGIN_RIGHT - OVERLAY_WIDTH;
        int posY = GetSystemMetrics(SM_CYSCREEN) - MARGIN_BOTTOM - OVERLAY_HEIGHT;
        
        SetWindowPos(hwndOverlay, HWND_TOPMOST, posX, posY, 0, 0, SWP_NOSIZE);
        SetLayeredWindowAttributes(hwndOverlay, RGB(0,0,0), 200, LWA_COLORKEY | LWA_ALPHA);
        ShowWindow(hwndOverlay, SW_SHOW);
        UpdateWindow(hwndOverlay);
    }
}

void MouseShifter::run() {
    initOverlay();
    
    std::cout << "=== Mouse Shifter Started ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  - HOLD Left Mouse Button to grab the stick" << std::endl;
    std::cout << "  - MOVE mouse to shift gears (H-Pattern)" << std::endl;
    std::cout << "  - RELEASE Left Mouse Button to engage gear" << std::endl;
    std::cout << "  - F12 to Toggle Overlay Visibility" << std::endl;
    std::cout << "  - ESC to Exit" << std::endl;

    MSG msg = {};
    while (true) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        handleInput();
        
        if (isEnabled) {
            updateShifterLogic();
        }

        if (needsRedraw) {
            renderOverlay();
            needsRedraw = false;
        }

        Sleep(10);
    }

    DestroyWindow(hwndOverlay);
    UnregisterClass(L"MouseShifterOverlay", GetModuleHandle(nullptr));
}

void MouseShifter::handleInput() {
    if (GetAsyncKeyState(VK_F12) & 1) {
        isEnabled = !isEnabled;
        ShowWindow(hwndOverlay, isEnabled ? SW_SHOW : SW_HIDE);
        std::cout << (isEnabled ? "Shifter ENABLED" : "Shifter DISABLED") << std::endl;
        needsRedraw = true;
    }

    if (GetAsyncKeyState(VK_ESCAPE) & 1) {
        PostQuitMessage(0);
    }

    if (GetKeyState(VK_LBUTTON) & 0x8000) {
        if (!isGrabbing) {
            isGrabbing = true;
        }
    } else {
        if (isGrabbing) {
            isGrabbing = false;
            engageGear();
        }
    }
}

void MouseShifter::updateShifterLogic() {
    if (!isGrabbing) return;

    static POINT lastMousePos = {0, 0};
    POINT currentMousePos;
    GetCursorPos(&currentMousePos);

    if (lastMousePos.x == 0 && lastMousePos.y == 0) {
        lastMousePos = currentMousePos;
    }

    float deltaX = (float)(currentMousePos.x - lastMousePos.x);
    float deltaY = (float)(currentMousePos.y - lastMousePos.y);

    virtualStickX += deltaX;
    virtualStickY += deltaY;

    lastMousePos = currentMousePos;

    Gear hoveredGear = detectHoveredGear();
    
    if (hoveredGear != currentHoveredGear) {
        currentHoveredGear = hoveredGear;
        needsRedraw = true;
    }
}

Gear MouseShifter::detectHoveredGear() {
    // Define gear order for iteration
    Gear gearOrder[] = {GEAR_1, GEAR_2, GEAR_3, GEAR_4, GEAR_5, GEAR_6, GEAR_R};
    
    for (Gear g : gearOrder) {
        float dist = sqrt(pow(virtualStickX - gearSlots[g].x, 2) + pow(virtualStickY - gearSlots[g].y, 2));
        if (dist < DEADZONE_RADIUS) {
            return g;
        }
    }
    
    float distCenter = sqrt(pow(virtualStickX, 2) + pow(virtualStickY, 2));
    if (distCenter < CENTER_DEADZONE) {
        return GEAR_N;
    }

    return GEAR_NONE;
}

void MouseShifter::engageGear() {
    Gear targetGear = currentHoveredGear;

    if (targetGear == GEAR_NONE || targetGear == GEAR_INVALID) {
        targetGear = GEAR_N;
    }

    if (targetGear != currentGear) {
        currentGear = targetGear;
        sendKeyPress(currentGear);
        std::cout << "Gear Engaged: " << getGearName(currentGear) << std::endl;
        needsRedraw = true;
    }
    
    if (currentGear != GEAR_NONE) {
        virtualStickX = gearSlots[currentGear].x;
        virtualStickY = gearSlots[currentGear].y;
        needsRedraw = true;
    }
}

void MouseShifter::sendKeyPress(Gear gear) {
    WORD key = 0;
    switch (gear) {
        case GEAR_1: key = '1'; break;
        case GEAR_2: key = '2'; break;
        case GEAR_3: key = '3'; break;
        case GEAR_4: key = '4'; break;
        case GEAR_5: key = '5'; break;
        case GEAR_6: key = '6'; break;
        case GEAR_R: key = 'R'; break;
        case GEAR_N: key = 'N'; break;
        default: return;
    }

    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = key;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = key;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, inputs, sizeof(INPUT));
}

void MouseShifter::renderOverlay() {
    if (!hwndOverlay) return;

    HDC hdc = GetDC(hwndOverlay);
    RECT rect;
    GetClientRect(hwndOverlay, &rect);

    // Create memory DC for double buffering
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    SelectObject(memDC, bmp);

    // Fill with transparent color (matches LWA_COLORKEY)
    HBRUSH bgBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(memDC, &rect, bgBrush);
    DeleteObject(bgBrush);

    // Draw Settings
    SetBkMode(memDC, TRANSPARENT);
    
    // Title
    HFONT fontTitle = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    SelectObject(memDC, fontTitle);
    SetTextColor(memDC, RGB(255, 255, 255));
    TextOut(memDC, 20, 20, L"MOUSE SHIFTER", 13);
    DeleteObject(fontTitle);

    // Status
    HFONT fontSmall = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    SelectObject(memDC, fontSmall);
    
    std::wstring statusText = isEnabled ? L"STATUS: ACTIVE" : L"STATUS: DISABLED (Press F12)";
    SetTextColor(memDC, isEnabled ? RGB(0, 255, 0) : RGB(255, 0, 0));
    TextOut(memDC, 20, 50, statusText.c_str(), statusText.length());

    // Current Gear Text
    std::wstring gearText = L"Current Gear: " + std::wstring(getGearName(currentGear));
    SetTextColor(memDC, RGB(0, 200, 255));
    TextOut(memDC, 20, 80, gearText.c_str(), gearText.length());

    // Instructions
    SetTextColor(memDC, RGB(200, 200, 200));
    TextOut(memDC, 20, 110, L"Hold LMB to move stick", 22);
    TextOut(memDC, 20, 130, L"Release to engage gear", 22);

    // Draw H-Pattern Base
    int centerX = rect.right / 2;
    int centerY = rect.bottom / 2 + 20;

    HPEN linePen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    SelectObject(memDC, linePen);
    
    // Draw Gates (Lines)
    MoveToEx(memDC, centerX - 60, centerY - 80, nullptr); LineTo(memDC, centerX - 60, centerY + 80);
    MoveToEx(memDC, centerX, centerY - 80, nullptr);       LineTo(memDC, centerX, centerY + 80);
    MoveToEx(memDC, centerX + 60, centerY - 80, nullptr);  LineTo(memDC, centerX + 60, centerY + 80);
    MoveToEx(memDC, centerX - 90, centerY, nullptr);       LineTo(memDC, centerX + 90, centerY);

    // Draw Gear Slots Labels
    SetTextColor(memDC, RGB(150, 150, 150));
    auto drawLabel = [&](int x, int y, const wchar_t* txt) {
        RECT r = { centerX + x - 10, centerY + y - 10, centerX + x + 10, centerY + y + 10 };
        DrawText(memDC, txt, -1, &r, DT_CENTER | DT_SINGLELINE);
    };

    // Top Row
    drawLabel(-108, -60, L"R");
    drawLabel(-60, -60, L"1");
    drawLabel(0, -60, L"3");
    drawLabel(60, -60, L"5");
    // Bottom Row
    drawLabel(-60, 60, L"2");
    drawLabel(0, 60, L"4");
    drawLabel(60, 60, L"6");
    // Center
    drawLabel(0, 0, L"N");

    // Draw Virtual Stick Position
    if (isEnabled) {
        int stickScreenX = centerX + (int)virtualStickX;
        int stickScreenY = centerY + (int)virtualStickY;

        // Draw Stick Line
        HPEN stickPen = CreatePen(PS_SOLID, 4, RGB(255, 50, 50));
        SelectObject(memDC, stickPen);
        MoveToEx(memDC, centerX, centerY, nullptr);
        LineTo(memDC, stickScreenX, stickScreenY);
        DeleteObject(stickPen);

        // Draw Stick Head
        HBRUSH stickBrush = CreateSolidBrush(RGB(255, 50, 50));
        Ellipse(memDC, stickScreenX - 8, stickScreenY - 8, stickScreenX + 8, stickScreenY + 8);
        DeleteObject(stickBrush);

        // Highlight active slot
        if (currentHoveredGear != GEAR_NONE && currentHoveredGear != GEAR_INVALID) {
            int slotX = centerX + (int)gearSlots[currentHoveredGear].x;
            int slotY = centerY + (int)gearSlots[currentHoveredGear].y;
            
            HPEN highlightPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
            HBRUSH highlightBrush = CreateHatchBrush(HS_BDIAGONAL, RGB(255, 255, 0));
            
            SelectObject(memDC, highlightPen);
            SelectObject(memDC, highlightBrush);
            Ellipse(memDC, slotX - 15, slotY - 15, slotX + 15, slotY + 15);
            
            DeleteObject(highlightPen);
            DeleteObject(highlightBrush);
        }
    }

    // Copy to screen
    BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);

    // Cleanup
    DeleteDC(memDC);
    DeleteObject(bmp);
    ReleaseDC(hwndOverlay, hdc);
    DeleteObject(linePen);
    DeleteObject(fontSmall);
}

LRESULT CALLBACK MouseShifter::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

const wchar_t* MouseShifter::getGearName(Gear gear) {
    switch (gear) {
        case GEAR_1: return L"1";
        case GEAR_2: return L"2";
        case GEAR_3: return L"3";
        case GEAR_4: return L"4";
        case GEAR_5: return L"5";
        case GEAR_6: return L"6";
        case GEAR_R: return L"R";
        case GEAR_N: return L"N";
        default: return L"-";
    }
}
