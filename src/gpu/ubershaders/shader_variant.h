#pragma once

#include <cstdint>

// Identifies one compiled fixed-function shader variant.
// Zero is reserved for the generic ubershader fallback.
using ShaderVariantKey = uint64_t;

// Variant key for the generic (non-specialized) ubershader.
constexpr ShaderVariantKey GenericShaderVariant = 0;
