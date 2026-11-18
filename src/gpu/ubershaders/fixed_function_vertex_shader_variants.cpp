#include "fixed_function_vertex_shader_variants.h"

#include "ubershader_catalog.h"

#include <external/rapidhash.h>

#include <algorithm>
#include <cstring>

#include "generated/fixed_function_vertex_shader/fixed_function_vertex_shader_variants.generated.h"

namespace
{
    // Generated catalog for the fixed-function vertex shader family.
    // States are normalized before lookup so raw states that execute
    // identical code select the same variant.
    struct VertexShaderFamily
    {
        static constexpr auto& States = FixedFunctionVertexShaderVariants::Generated::States;
        static constexpr auto& Programs = FixedFunctionVertexShaderVariants::Generated::Programs;
        static constexpr const char* Name = "FixedFunctionVertexShader";

        static uint64_t Hash(const FixedFunctionVertexShaderControlState& state)
        {
            const FixedFunctionVertexShaderControlState normalized = Normalize(state);
            const uint64_t hash = rapidhash(&normalized, sizeof(normalized));
            return hash != 0 ? hash : 1; // 0 is special cased for the fallback.
        }

        static FixedFunctionVertexShaderControlState Normalize(FixedFunctionVertexShaderControlState state)
        {
            state.vertex_blend_calc_last_weight = state.vertex_blend_calc_last_weight != 0;
            state.lighting_enable = state.lighting_enable != 0;
            state.two_sided_lighting_enable = state.two_sided_lighting_enable != 0;
            state.local_viewer_enable = state.local_viewer_enable != 0;
            state.normalize_normals_enable = state.normalize_normals_enable != 0;
            state.fog_enable = state.fog_enable != 0;
            state.point_sprite_enable = state.point_sprite_enable != 0;
            state.point_scale_enable = state.point_scale_enable != 0;
            state.specular_enable = state.specular_enable != 0;

            if (!state.lighting_enable)
            {
                state.two_sided_lighting_enable = 0;
                state.local_viewer_enable = 0;
                state.ambient_material_source = 0;
                state.diffuse_material_source = 0;
                state.specular_material_source = 0;
                state.emissive_material_source = 0;
                state.specular_enable = 0;
                memset(state.light_type, 0, sizeof(state.light_type));
            }
            else if (std::all_of(std::begin(state.light_type), std::end(state.light_type), [](uint32_t type) { return type == 0; }))
            {
                state.local_viewer_enable = 0;
            }

            bool normalUsed = state.lighting_enable != 0;
            bool usedComponentCounts[FixedFunctionVertexShaderControlState::TextureStageCount]{};
            // One bit per Xbox vertex data register (D3DVSDE_*) whose
            // default value can matter in this state: 0 position,
            // 1 blend weight, 2 normal, 3 diffuse, 4 specular or fog depth,
            // 7/8 back colors, 9+stage texcoords.
            uint32_t usedRegisterMask = 1u << 0;
            if (state.vertex_blend_matrix_count > 1 || !state.vertex_blend_calc_last_weight)
                usedRegisterMask |= 1u << 1;
            for (uint32_t stage = 0; stage < FixedFunctionVertexShaderControlState::TextureStageCount; stage++)
            {
                state.texture_transform_count[stage] = state.texture_transform_count[stage] != 0;
                uint32_t generation = state.texcoord_generation[stage];
                if (generation == 0)
                {
                    uint32_t index = state.texcoord_index[stage];
                    if (index < FixedFunctionVertexShaderControlState::TextureStageCount)
                    {
                        usedComponentCounts[index] = true;
                        usedRegisterMask |= 1u << (9 + index);
                    }
                }
                else
                {
                    state.texcoord_index[stage] = 0;
                }

                normalUsed |= generation == 1 || generation == 3 || generation == 5;
            }

            if (!normalUsed)
                state.normalize_normals_enable = 0;
            else
                usedRegisterMask |= 1u << 2;

            if (!state.lighting_enable || state.ambient_material_source == 1 ||
                state.diffuse_material_source == 1 || state.specular_material_source == 1 ||
                state.emissive_material_source == 1)
                usedRegisterMask |= 1u << 3;
            if (!state.lighting_enable || state.ambient_material_source == 2 ||
                state.diffuse_material_source == 2 || state.specular_material_source == 2 ||
                state.emissive_material_source == 2 || (state.fog_enable && state.fog_depth_mode == 0))
                usedRegisterMask |= 1u << 4;
            if (!(state.lighting_enable && state.two_sided_lighting_enable) ||
                state.ambient_material_source == 1 || state.diffuse_material_source == 1 ||
                state.specular_material_source == 1 || state.emissive_material_source == 1)
                usedRegisterMask |= 1u << 7;
            if (!(state.lighting_enable && state.two_sided_lighting_enable) ||
                state.ambient_material_source == 2 || state.diffuse_material_source == 2 ||
                state.specular_material_source == 2 || state.emissive_material_source == 2)
                usedRegisterMask |= 1u << 8;
            for (uint32_t index = 0; index < FixedFunctionVertexShaderControlState::TextureStageCount; index++)
            {
                if (!usedComponentCounts[index])
                    state.texcoord_component_count[index] = 0;
            }
            if (!state.fog_enable)
                state.fog_depth_mode = 0;
            if (!state.point_sprite_enable)
                state.point_scale_enable = 0;
            state.vertex_register_default_mask &= usedRegisterMask;

            return state;
        }
    };

    static UbershaderCatalog<VertexShaderFamily> g_catalog;
}

namespace FixedFunctionVertexShaderVariants
{
    void Initialize(plume::RenderDevice* device, bool vulkan)
    {
        g_catalog.Initialize(device, vulkan);
    }

    ShaderVariantKey Select(const FixedFunctionVertexShaderControlState& state)
    {
        return g_catalog.Select(state);
    }

    bool Contains(ShaderVariantKey key)
    {
        return g_catalog.Contains(key);
    }

    plume::RenderShader* Resolve(ShaderVariantKey key)
    {
        return g_catalog.Resolve(key);
    }
}
