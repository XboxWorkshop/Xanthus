#pragma once

#include "shader_variant.h"

#include <plume_render_interface.h>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <type_traits>

// Shader inputs that select a fixed-function vertex shader variant.
// Fields are one byte each, so hashing the raw bytes is cheaper; the struct
// must be zero-initialized, since the trailing padding byte is hashed too.
struct FixedFunctionVertexShaderControlState
{
    static constexpr uint32_t LightCount = 8;
    static constexpr uint32_t TextureStageCount = 4;

    // Vertex blending.
    uint8_t vertex_blend_matrix_count;
    uint8_t vertex_blend_calc_last_weight;

    // Lighting configuration.
    uint8_t lighting_enable;
    uint8_t two_sided_lighting_enable;
    uint8_t local_viewer_enable;
    uint8_t normalize_normals_enable;
    uint8_t ambient_material_source;
    uint8_t diffuse_material_source;
    uint8_t specular_material_source;
    uint8_t emissive_material_source;

    // Fog output depth calculation.
    uint8_t fog_enable;
    uint8_t fog_depth_mode;

    // Per-light type, or zero when the light slot is unused.
    uint8_t light_type[LightCount];

    // Per-stage texture coordinate input and transform controls.
    uint8_t texture_transform_count[TextureStageCount];
    uint8_t texcoord_index[TextureStageCount];
    uint8_t texcoord_generation[TextureStageCount];
    uint8_t texcoord_component_count[TextureStageCount];

    // Vertex registers absent from the current vertex declaration.
    uint32_t vertex_register_default_mask;

    // Point rendering mode.
    uint8_t point_sprite_enable;
    uint8_t point_scale_enable;
    // Specular lighting output; zero when lighting is off.
    uint8_t specular_enable;
};

// State hashing covers these raw bytes, so the layout must not change.
static_assert(sizeof(FixedFunctionVertexShaderControlState) == 44);
static_assert(std::is_standard_layout_v<FixedFunctionVertexShaderControlState>);
static_assert(std::endian::native == std::endian::little);
static_assert(offsetof(FixedFunctionVertexShaderControlState, vertex_blend_matrix_count) == 0);
static_assert(offsetof(FixedFunctionVertexShaderControlState, light_type) == 12);
static_assert(offsetof(FixedFunctionVertexShaderControlState, texture_transform_count) == 20);
static_assert(offsetof(FixedFunctionVertexShaderControlState, texcoord_index) == 24);
static_assert(offsetof(FixedFunctionVertexShaderControlState, texcoord_generation) == 28);
static_assert(offsetof(FixedFunctionVertexShaderControlState, texcoord_component_count) == 32);
static_assert(offsetof(FixedFunctionVertexShaderControlState, vertex_register_default_mask) == 36);
static_assert(offsetof(FixedFunctionVertexShaderControlState, point_sprite_enable) == 40);
static_assert(offsetof(FixedFunctionVertexShaderControlState, specular_enable) == 42);

// Selects and creates specialized fixed-function vertex shaders.
// Generated variants contain all states observed in game (after CC'ing every stage).
// Any other state uses the generic FixedFunctionVertexShader.hlsl fallback.
namespace FixedFunctionVertexShaderVariants
{
    // Stores the device used to create variant shaders.
    // Aborts when reinitialized with a different device or graphics API.
    void Initialize(plume::RenderDevice* device, bool vulkan);

    // Returns the program key matching a control state.
    // Returns GenericShaderVariant when the state has no specialized variant.
    ShaderVariantKey Select(const FixedFunctionVertexShaderControlState& state);

    // Returns true when `key` is the generic fallback or a known variant.
    bool Contains(ShaderVariantKey key);

    // Returns the variant shader, creating it on first use.
    // Returns nullptr for the generic key or unknown keys; callers bind the
    // generic fixed-function vertex shader in that case.
    plume::RenderShader* Resolve(ShaderVariantKey key);
}
