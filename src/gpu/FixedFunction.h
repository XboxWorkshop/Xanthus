#pragma once
#include "ubershaders/shader_variant.h"

// Rebuilds the fixed-function vertex shader state constants when dirty and
// returns the variant key for the current controls.
//
// Parameters:
// - `Handle` - guest vertex shader handle
// - `isPSFixed` - pixel shader is fixed function; selects fog depth output
//   over fog factor in the vertex shader
// - `vertexRegisterDefaultMask` - marks vertex registers absent from the
//   current vertex declaration
//
// Returns GenericShaderVariant when the state has no specialized variant.
ShaderVariantKey UpdateFixedFunctionVertexShaderState(size_t Handle, bool isPSFixed, uint32_t vertexRegisterDefaultMask);

// Schedules a refresh of the fixed-function vertex shader constants.
void MarkFixedFunctionVertexShaderStateDirty();

// Schedules a refresh of the transform registers only.
void MarkFixedFunctionVertexShaderTransformsDirty();

// Schedules a refresh that also reselects the specialized vertex shader.
void MarkFixedFunctionVertexShaderVariantDirty();

// Schedules a rebuild of the fixed-function light block only; the light
// inputs are the view transform, specular enable, and enabled light data.
void MarkFixedFunctionVertexShaderLightsDirty();

extern std::array<bool, 4> bWorldViewDirty;


struct Material
{
	D3DCOLORVALUE Diffuse; // base for Materials[0], these will be array-offsetted
	D3DCOLORVALUE Ambient;
	D3DCOLORVALUE Specular;
	D3DCOLORVALUE Emissive;
	D3DCOLORVALUE Power; // c110.w/y/z free
};
static_assert(sizeof(Material) % 16 == 0, "Material must be multiple of 16 bytes");

void UpdateMaterial(Material* pMaterial);
void UpdateLightData(uint32_t index, X_D3DLIGHT8* pLight);
void MarkFixedFunctionVertexShaderStateDirty();