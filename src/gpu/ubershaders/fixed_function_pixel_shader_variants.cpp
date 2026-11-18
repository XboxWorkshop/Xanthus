#include "fixed_function_pixel_shader_variants.h"
#include "ubershader_catalog.h"
#include "gpu/shared_constants.h"
#include <external/rapidhash.h>
#include <cassert>
#include "generated/fixed_function_pixel_shader/fixed_function_pixel_shader_variants.generated.h"

namespace
{
    // Generated catalog for the fixed-function pixel shader family.
    struct PixelShaderFamily
    {
        static constexpr auto& States = FixedFunctionPixelShaderVariants::Generated::States;
        static constexpr auto& Programs = FixedFunctionPixelShaderVariants::Generated::Programs;
        static constexpr const char* Name = "FixedFunctionPixelShader";

        static uint64_t Hash(const FixedFunctionPixelShaderState& state)
        {
            const uint64_t hash = rapidhash(&state, sizeof(state));
            return hash != 0 ? hash : 1; // 0 is special cased for the fallback.
        }

        static const FixedFunctionPixelShaderState& Normalize(const FixedFunctionPixelShaderState& state)
        {
            return state;
        }
    };

    static UbershaderCatalog<PixelShaderFamily> g_catalog;

    // Sewer note: Convert the current shader state to key, the same way generate.py does.
    //             Prevents breakages when FixedFunctionPixelShaderState changes.
    ShaderVariantKey ProgramKey(const FixedFunctionPixelShaderState& state)
    {
        constexpr uint8_t ColorOpDisable = 1;
        constexpr uint32_t StageCount = FixedFunctionPixelShaderState::TextureStageCount;

        uint32_t activeStageCount = 0;
        for (uint32_t i = 0; i < StageCount; i++)
        {
            if (state.color_op[i] != ColorOpDisable)
                activeStageCount = i + 1;
        }

        uint32_t values[1 + StageCount * 10 + 5];
        size_t count = 0;
        values[count++] = activeStageCount;
        for (uint32_t i = 0; i < activeStageCount; i++)
        {
            values[count++] = state.color_op[i];
            values[count++] = state.color_arg0[i];
            values[count++] = state.color_arg1[i];
            values[count++] = state.color_arg2[i];
            values[count++] = state.alpha_op[i];
            values[count++] = state.alpha_arg0[i];
            values[count++] = state.alpha_arg1[i];
            values[count++] = state.alpha_arg2[i];
            values[count++] = state.result_arg[i];
            values[count++] = state.texture_sample_type[i];
        }
        values[count++] = state.specular_enable != 0;
        values[count++] = state.fog_enable != 0;
        values[count++] = state.fog_enable != 0 ? state.fog_table_mode : 0;
        values[count++] = state.alpha_test_enable != 0;
        values[count++] = state.alpha_test_enable != 0 ? state.alpha_func : 0;

        const uint64_t key = rapidhash(values, count * sizeof(uint32_t));
        return key != 0 ? key : 1;
    }
}

namespace FixedFunctionPixelShaderVariants
{
    void Initialize(plume::RenderDevice* device, bool vulkan)
    {
        g_catalog.Initialize(device, vulkan);

#ifndef NDEBUG
        for (const auto& entry : PixelShaderFamily::States)
            assert(ProgramKey(entry.state) == entry.programKey);
#endif
    }

    FixedFunctionPixelShaderState Capture(const SharedConstants& constants)
    {
        FixedFunctionPixelShaderState state{};
        // A source value above 255 would wrap and merge distinct states.
        auto narrow = [](uint32_t value)
        {
            assert(value <= UINT8_MAX);
            return static_cast<uint8_t>(value);
        };

        for (uint32_t i = 0; i < FixedFunctionPixelShaderState::TextureStageCount; i++)
        {
            state.color_op[i] = narrow(constants.color_op[i]);
            state.color_arg0[i] = narrow(constants.color_arg0[i]);
            state.color_arg1[i] = narrow(constants.color_arg1[i]);
            state.color_arg2[i] = narrow(constants.color_arg2[i]);
            state.alpha_op[i] = narrow(constants.alpha_op[i]);
            state.alpha_arg0[i] = narrow(constants.alpha_arg0[i]);
            state.alpha_arg1[i] = narrow(constants.alpha_arg1[i]);
            state.alpha_arg2[i] = narrow(constants.alpha_arg2[i]);
            state.result_arg[i] = narrow(constants.result_arg[i]);
            state.texture_sample_type[i] = narrow(constants.TextureSampleType[i]);
        }
        state.specular_enable = narrow(constants.SpecularEnable);
        state.fog_enable = narrow(constants.FogEnable);
        state.fog_table_mode = narrow(constants.FogTableMode);
        state.alpha_test_enable = narrow(constants.alphaTestEnable);
        state.alpha_func = narrow(constants.alphaFunc);
        return state;
    }

    ShaderVariantKey Select(const FixedFunctionPixelShaderState& state)
    {
        const ShaderVariantKey key = ProgramKey(state);
        return g_catalog.Contains(key) ? key : GenericShaderVariant;
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
