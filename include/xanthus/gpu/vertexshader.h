#pragma once

#include <plume_render_interface.h>
#include <shader/shader_cache.h>

enum VSH_SWIZZLE {
	SWIZZLE_X = 0,
	SWIZZLE_Y,
	SWIZZLE_Z,
	SWIZZLE_W
};

enum VSH_IMD_PARAMETER_TYPE {
	PARAM_UNKNOWN = 0,
	PARAM_R,          // Temporary (scRatch) registers
	PARAM_V,          // Vertex registers
	PARAM_C,          // Constant registers, set by SetVertexShaderConstant
	PARAM_O // = 0??
};

typedef struct {
	VSH_IMD_PARAMETER_TYPE  Type;   // Parameter type, R, V or C
	bool                Neg;             // true if negated, false if not
	VSH_SWIZZLE         Swizzle[4];      // The four swizzles
	int16_t             Address;         // Register address
} VSH_IMD_PARAMETER;



struct X_D3DVertexShader
{
#if 0 
	DWORD Signature; // Note : Debug XBE's have a 'Vshd' DWORD signature prefix
#endif
	DWORD RefCount; // Based on the observation this member is set to 1 in D3DDevice_CreateVertexShader and decreased in D3DDevice_DeleteVertexShader
	DWORD Flags; // Contains X_VERTEXSHADER_FLAG_* bits
	DWORD ProgramSize;
	DWORD ProgramAndConstantsDwords; // Sum of ProgramSize + constant count, expressed in instruction slots, taking 4 DWORD's per slot (see X_VSH_INSTRUCTION_SIZE)
	BYTE Dimensionality[4]; // Guesswork, since all 4 bytes (for all 4 textures) are most often set to 0 (or 2 when a texture isn't used) and 1, 3 and 4 also occur (and nothing else)
	struct X_VERTEXATTRIBUTEFORMAT VertexAttribute;
	DWORD ProgramAndConstants[X_VSH_MAX_INSTRUCTION_COUNT]; // The binary function data and constants (contents continues futher outside this struct, up to ProgramAndConstantsDwords * 4 (=X_VSH_INSTRUCTION_SIZE) DWORD's)
};

extern uint32_t g_Xbox_VertexShader_FunctionSlots_StartAddress;
extern DWORD g_Xbox_VertexShaderConstantMode;

X_D3DVertexShader* GetXboxVertexShaderForHandle(size_t Handle);
struct GuestShader* FindShaderCacheEntryNew();
uint64_t GetVertexShaderKey();
void UpdateShaderCacheEntryNew(GuestShader* shader, const uint64_t key);
void SetVertexShaderSlots(DWORD* pTokens, DWORD Address, DWORD NrInstructions);
inline X_D3DVertexShader* VshHandleToXboxVertexShader(size_t Handle) { return (X_D3DVertexShader*)(Handle & ~X_D3DFVF_RESERVED0); }
DWORD* GetVertexShaderTokens(X_D3DVertexShader* pXboxVertexShader, DWORD* pNrTokens);
void SetVertexShaderPassthroughProgram();

extern VertexShaderMode g_Xbox_VertexShaderMode;
extern uint32_t g_VertexShader_Handle;
const uint8_t* CreateRecompiledShader(size_t handle, size_t* outSize);
plume::RenderShader* GetOrLinkShader(struct GuestShader* guestShader, uint32_t specConstants);
ShaderCacheEntry* FindShaderCacheEntry(XXH64_hash_t hash);
void CopyVertexShaderConstant(int32_t Register, DWORD* pConstantData, const uint32_t count);
struct LocalRenderCommandQueue;
void HandleShadersConstant(LocalRenderCommandQueue& queue);
