#pragma once
#include <cstdint>

//HostVertexDeclaration* HostGetVertexDeclaration(size_t handle, const bool isFixedFunction);
GuestVertexDeclaration* CreateVertexDeclarationWithoutAddRef(D3DVERTEXELEMENT9* vertexElements, VertexShaderMode shaderMode);
GuestVertexDeclaration* FindVertexDeclarationByHash(uint64_t hash);
void SetFixedFunctionDefaultVertexAttributes(size_t Handle);
X_VERTEXATTRIBUTEFORMAT* GetXboxVertexAttributeFormat(size_t handle);
void UpdateVertexDeclaration(uint32_t Handle);