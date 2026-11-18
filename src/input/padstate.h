
#pragma once

namespace PJSPeripheral
{

    struct ControllerSlot {
        uint32_t buttons;        // +0
        uint32_t buttonsInverted;// +4
        uint32_t justPressed;    // +8
        uint32_t justReleased;   // +12
        float    leftX;          // +16
        float    leftY;          // +20
        uint16_t rawLeftY;       // +24
        uint16_t rawLeftX;       // +26
        float    rightX;         // +28
        float    rightY;         // +32
        uint16_t rawRightX;      // +36
        uint16_t rawRightY;      // +38
        uint32_t status;         // +40 
    };          

    enum GameButton : uint32_t {
        BTN_A = 0x0001,
        BTN_B = 0x0002,
        BTN_X = 0x0008,
        BTN_Y = 0x0010,
        BTN_BLACK = 0x0020,
        BTN_WHITE = 0x0004,
        BTN_LEFT_TRIG = 0x1000,
        BTN_RIGHT_TRIG = 0x2000,
        BTN_START = 0x0400,
        BTN_BACK = 0x0800,
        BTN_DPAD_UP = 0x0040,
        BTN_DPAD_DOWN = 0x0080,
        BTN_DPAD_LEFT = 0x0100,
        BTN_DPAD_RIGHT = 0x0200,
    };

    inline ControllerSlot* GetControllerSlot(uint32_t index)
    {
        uint8_t* base = (uint8_t*)MmGetHostAddress(0x541A68); // get singleton of ControllerManager
        if (!base)
            return nullptr;

        return reinterpret_cast<ControllerSlot*>(base + index * 44);
    }


    inline bool IsConnected(uint32_t index)
    {
        auto* s = GetControllerSlot(index);
        return s && s->status == 0;
    }

    inline bool IsHeld(uint32_t index, uint32_t btn) { auto* s = GetControllerSlot(index); return s && (s->buttons & btn); }
    inline bool IsJustPressed(uint32_t index, uint32_t btn) { auto* s = GetControllerSlot(index); return s && (s->justPressed & btn); }
    inline bool IsJustReleased(uint32_t index, uint32_t btn) { auto* s = GetControllerSlot(index); return s && (s->justReleased & btn); }

}