
#pragma once

#include <plume_render_interface.h>
#include "external/ddspp.h"

plume::RenderFormat ConvertFormat(enum _X_D3DFORMAT format);
plume::RenderTextureAddressMode ConvertTextureAddressMode(size_t value);
plume::RenderFilter ConvertTextureFilter(uint32_t value);
plume::RenderBorderColor ConvertBorderColor(uint32_t value);
plume::RenderPrimitiveTopology ConvertPrimitiveType(uint32_t primitiveType);
plume::RenderBlend ConvertBlendMode(uint32_t blendMode);
plume::RenderBlend ConvertAlphaBlendMode(uint32_t blendMode);
plume::RenderBlendOperation ConvertBlendOp(uint32_t blendOp);
plume::RenderComparisonFunction ConvertD3DCMP(uint32_t value);
plume::RenderStencilOp ConvertStencilOp(uint32_t stencilOp);
plume::RenderFormat ConvertDXGIFormat(ddspp::DXGIFormat format);
plume::RenderTextureDimension ConvertTextureDimension(ddspp::TextureType type);
plume::RenderTextureViewDimension ConvertTextureViewDimension(ddspp::TextureType type);