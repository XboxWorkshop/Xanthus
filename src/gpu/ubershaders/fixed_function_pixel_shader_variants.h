#pragma once

#include "shader_variant.h"

#include <plume_render_interface.h>
#include <bit>
#include <cstddef>
#include <type_traits>

struct SharedConstants;

// Shader inputs that select a fixed-function pixel shader variant.
// Fields are one byte each, so hashing the raw bytes is cheaper.
struct FixedFunctionPixelShaderState
{
    static constexpr uint32_t TextureStageCount = 4;

    // Texture combiner controls, one entry per stage.
    uint8_t color_op[TextureStageCount];
    uint8_t color_arg0[TextureStageCount];
    uint8_t color_arg1[TextureStageCount];
    uint8_t color_arg2[TextureStageCount];
    uint8_t alpha_op[TextureStageCount];
    uint8_t alpha_arg0[TextureStageCount];
    uint8_t alpha_arg1[TextureStageCount];
    uint8_t alpha_arg2[TextureStageCount];
    uint8_t result_arg[TextureStageCount];
    uint8_t texture_sample_type[TextureStageCount];

    // Fixed-function pipeline modes.
    uint8_t specular_enable;
    uint8_t fog_enable;
    uint8_t fog_table_mode;
    uint8_t alpha_test_enable;
    uint8_t alpha_func;
};

// State hashing covers these raw bytes, so the layout must not change.
static_assert(sizeof(FixedFunctionPixelShaderState) == 45);
static_assert(std::is_standard_layout_v<FixedFunctionPixelShaderState>);
static_assert(std::endian::native == std::endian::little);
static_assert(offsetof(FixedFunctionPixelShaderState, color_op) == 0);
static_assert(offsetof(FixedFunctionPixelShaderState, color_arg0) == 4);
static_assert(offsetof(FixedFunctionPixelShaderState, color_arg1) == 8);
static_assert(offsetof(FixedFunctionPixelShaderState, color_arg2) == 12);
static_assert(offsetof(FixedFunctionPixelShaderState, alpha_op) == 16);
static_assert(offsetof(FixedFunctionPixelShaderState, alpha_arg0) == 20);
static_assert(offsetof(FixedFunctionPixelShaderState, alpha_arg1) == 24);
static_assert(offsetof(FixedFunctionPixelShaderState, alpha_arg2) == 28);
static_assert(offsetof(FixedFunctionPixelShaderState, result_arg) == 32);
static_assert(offsetof(FixedFunctionPixelShaderState, texture_sample_type) == 36);
static_assert(offsetof(FixedFunctionPixelShaderState, specular_enable) == 40);
static_assert(offsetof(FixedFunctionPixelShaderState, fog_enable) == 41);
static_assert(offsetof(FixedFunctionPixelShaderState, fog_table_mode) == 42);
static_assert(offsetof(FixedFunctionPixelShaderState, alpha_test_enable) == 43);
static_assert(offsetof(FixedFunctionPixelShaderState, alpha_func) == 44);

// Selects and creates specialized fixed-function pixel shaders.
// Generated variants contain all states observed in game (after CC'ing every stage).
// Any other state uses the generic FixedFunctionPixelShader.hlsl fallback.
namespace FixedFunctionPixelShaderVariants
{
    // Stores the device used to create variant shaders.
    // Aborts when reinitialized with a different device or graphics API.
    void Initialize(plume::RenderDevice* device, bool vulkan);

    // Copies the pixel shader inputs out of the shared constants.
    FixedFunctionPixelShaderState Capture(const SharedConstants& constants);

    // Returns the program key matching a captured state.
    // Returns GenericShaderVariant when the state has no specialized variant.
    ShaderVariantKey Select(const FixedFunctionPixelShaderState& state);

    // Returns true when `key` is the generic fallback or a known variant.
    bool Contains(ShaderVariantKey key);

    // Returns the variant shader, creating it on first use.
    // Returns nullptr for the generic key or unknown keys; callers bind the
    // generic fixed-function pixel shader in that case.
    plume::RenderShader* Resolve(ShaderVariantKey key);
}
