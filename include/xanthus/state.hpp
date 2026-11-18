#pragma once

#include <cstdint>
namespace xanthus {
// matches SDL_GamepadButton up to Touchpad
enum class EPadButton : std::uint32_t
{
    A,
    B,
    X,
    Y,
    Back,
    Guide,
    Start,
    LeftStick,
    RightStick,
    LeftShoulder,
    RightShoulder,
    DPadUp,
    DPadDown,
    DPadLeft,
    DPadRight,
    Misc1,
    Paddle1,
    Paddle2,
    Paddle3,
    Paddle4,
    Touchpad,
    LeftTrigger,
    RightTrigger,
    None,
    Count
};

enum class EGraphicsAPI : std::uint32_t
{
    Auto,
#ifdef XANTHUS_D3D12
    D3D12,
#endif
    Vulkan
};

enum class EAspectRatio : std::uint32_t
{
    Auto,
    Original,
    Wide16x9,
    Wide16x10,
    Narrow5x4,
    Wide21x9
};

enum class EWindowState : std::uint32_t
{
    Normal,
    Maximised
};

enum class ETripleBuffering : std::uint32_t
{
    Auto,
    On,
    Off
};

enum class EFMVAspectRatio : std::uint32_t
{
    Stretched,
    Fit,
    Fill
};

#define SET_GETTER(type, Name, var, pName) \
    void set##Name(type pName) { var = pName; } \
    type get##Name() const { return var; }

class State {
public:
    static State& instance() {
        static State obj;
        return obj;
    }

    SET_GETTER(bool,   Vsync,        vsync_,      truthy); // applies on next swapchain check
    SET_GETTER(double, TargetFPS,    fpsTarget_,  fps);
    SET_GETTER(int, WindowWidth,  width_,      width);
    SET_GETTER(int, WindowHeight, height_,     height);
    SET_GETTER(double, ResolutionScale, resScale_, scale);
    SET_GETTER(float,  Brightness, brightness_, brightness);
    SET_GETTER(int,    AnisotropicFiltering, aniFilter_, mult);

    SET_GETTER(bool,   DisplayFPS, fpsDisplay_, truthy);
    SET_GETTER(bool,   DynamicResolution, dynamicRes_, truthy);

    SET_GETTER(EAspectRatio, AspectRatio, aspectRatio_, ratio);
private:
    EAspectRatio aspectRatio_ = EAspectRatio::Original;
    float brightness_ = 1.0f;
    double fpsTarget_ = 30;
    int width_ = 640;
    int height_ = 480;
    double resScale_ = 1.0;
    bool vsync_ = false;
    bool fpsDisplay_ = false;
    bool dynamicRes_ = false;
    int aniFilter_ = 1;
};
}
