#pragma once

#include <cstddef>
#include <cstdint>

// Number of texture slots addressable by host shaders and shared constants.
static constexpr uint32_t SharedTextureSlotCount = 16;

// Shared constants consumed by the host pixel shader.
// The layout is bound as a GPU buffer, so field order and types must not change
// without updating every shader that reads it.
struct SharedConstants
{
    uint32_t texture2DIndices[SharedTextureSlotCount];
    uint32_t texture3DIndices[SharedTextureSlotCount];
    uint32_t textureCubeIndices[SharedTextureSlotCount];
    uint32_t samplerIndices[SharedTextureSlotCount];

    // Texture combiner state, one entry per texture stage.
    uint32_t color_op[SharedTextureSlotCount];
    uint32_t color_arg0[SharedTextureSlotCount];
    uint32_t color_arg1[SharedTextureSlotCount];
    uint32_t color_arg2[SharedTextureSlotCount];

    uint32_t alpha_op[SharedTextureSlotCount];
    uint32_t alpha_arg0[SharedTextureSlotCount];
    uint32_t alpha_arg1[SharedTextureSlotCount];
    uint32_t alpha_arg2[SharedTextureSlotCount];
    uint32_t result_arg[SharedTextureSlotCount];

    uint32_t TextureSampleType[SharedTextureSlotCount];

    uint32_t BUMPENVMAT00[SharedTextureSlotCount];
    uint32_t BUMPENVMAT01[SharedTextureSlotCount];
    uint32_t BUMPENVMAT11[SharedTextureSlotCount];
    uint32_t BUMPENVMAT10[SharedTextureSlotCount];
    uint32_t BUMPENVLSCALE[SharedTextureSlotCount];
    uint32_t BUMPENVLOFFSET[SharedTextureSlotCount];
    uint32_t TEXCOORDINDEX[SharedTextureSlotCount]; // handled by the VS

    // BORDERCOLOR is set on the sampler.

    float TextureFactor[4]; // RGBA
    uint32_t SpecularEnable;
    uint32_t FogEnable;
    uint32_t FogTableMode;
    uint32_t FogDensity;
    uint32_t FogStart;
    uint32_t FogEnd;
    float FogColor[3];

    uint32_t alphaTestEnable;
    uint32_t alphaFunc; // D3DCMP_* comparison function
    float alphaRef; // reference value, 0.0 - 1.0

    float alphaThreshold;

    uint32_t texCoordMinPadding[3]; //for shader alignment 

    // We originally offsets every viewport by half pixel to reproduce Xbox's vertex pos
    // however, that half pixel once upscaled by higher res becomes full pixels at the left and top of every 
    // Render Target that no draw ever covers, showing what we call "edge bleed".
    // We can fix that by clamping UV for each RT so the shader won't read the edge bleed data.
    float texCoordMin[SharedTextureSlotCount][2];

    // Same idea for the far edge (right side), though, this seems to only happen with FMVs at high resolution.
    // its frame is inside a bigger power of two textures, so the last texel gets blended with the padding causing
    // another bleed.
    // also set for partly drawn render targets (bloom source)
    float texCoordMax[SharedTextureSlotCount][2];
};

static_assert(offsetof(SharedConstants, texCoordMin) == 89 * 16);
static_assert(offsetof(SharedConstants, texCoordMax) == 97 * 16);