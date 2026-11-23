#include "video.h"
#include "vertexdeclaration.h"
#include "xanthus/gpu/vertexshader.h"
#include <kernel/function.h>
#include <kernel/heap.h>
#include <kernel/memory.h>
#include <xanthus/utility.hpp>

// Xbox originally supports immediate drawing by sending some kind of command queue to the GPU directly, bypassing the need of 
// calling DrawPrimitive functions, although, this system can also be used for non drawing operations.
// For rendering, it is usually done by calling D3DDevice_BeginPush call, a D3DDevice_SetVertexData* call per vertex and one 
// D3DDevice_EndPush API call. Similar to the other Xbox D3D APIs, these APIs "push" commands and data towards the Xbox GPU 
// (called NV2A) to perform drawing. More Info: https://github.com/Cxbx-Reloaded/Cxbx-Reloaded-legacy/wiki/Xbox-D3D-drawing-via-NV2A-Kelvin

// In the case of ShadowRecomp, rather than trying to emulate the entire push system command of Xbox, we're going to take shortcuts 
// and manually patch the few functions that need it.


static float ClearQuadVertex[16]; // 4 vertices, Stride 16 (float X Y Z W)
static float RasterRenderQuadVertex[24]; // 4 vertices, Stride 24 (float X Y Z W)

static float SubmitNodeQuadVertex[256]; //arbitrary limitation, should be enough to handle all case scenarios

DWORD GetStrideFromVertexDeclaration(HostVertexDeclaration* pVertexDeclaration)
{
	DWORD Stride = 0;

	if (pVertexDeclaration)
	{

		if (pVertexDeclaration->NumberOfVertexStreams == 1) {
			// Note : This assumes that the only stream in use will be stream zero :
			Stride = pVertexDeclaration->VertexStreams[0].HostVertexStride;
		}
		else {
			XANTHUS_ERROR("Non-FVF Vertex Shaders with multiple streams not supported for PushBuffer emulation!");
		}
	}
	else {
		XANTHUS_ERROR("Missing Vertex Declaration not supported for PushBuffer emulation!");
	}

	return Stride;
}
bool drawCompositeRT = false;
// this is to handle the composite for merging RT, this is mandatory to have anything rendering in-game post title screen.
void HLE_draw_inline_array(X_D3DPRIMITIVETYPE primitiveMode, int32_t VertexCount, void* VertexData)
{

	//DWORD vertex data array, 
	// To be used as a replacement for DrawVerticesUP, the caller needs to set the vertex format using IDirect3DDevice8::SetVertexShader before calling BeginPush.
	// All attributes in the vertex format must be padded DWORD multiples, and the vertex attributes must be specified in the canonical FVF ordering
	// (position followed by weight, normal, diffuse, and so on).
	// retrieve vertex shader


	DWORD dwVertexStride = GetStrideFromVertexDeclaration(XSR_HostGetVertexDeclaration((size_t)g_memory.Translate(g_VertexShader_Handle), g_Xbox_VertexShaderMode == VertexShaderMode::FixedFunction));
	if (dwVertexStride > 0)
	{
		drawCompositeRT = true;
		DrawPrimitiveUP(primitiveMode, VertexCount, VertexData, dwVertexStride);
		drawCompositeRT = false;
	}
}

#if XANTHUS_RECOMP
void ClearWithQuadsMidAsmHook(X86Context& __restrict ctx, uint8_t* base)
{

	//since the pointer of push buffer moved until its end, we have to go back to the beginning to extract vertex data
	intptr_t addr = ctx.eax.u32 - (16 * sizeof(DWORD)) - 4; //16 (size of the stride) * number of vertex - offset

	X_D3DPRIMITIVETYPE primitiveMode = (X_D3DPRIMITIVETYPE)X86_LOAD_U32(addr - 8);
	for (int i = 0; i < 16; i++)
	{
		ClearQuadVertex[i] = X86_LOAD_F32(addr + i * sizeof(float));
	}

	//printf("draw quad clear for 640x480 RT composite.\n");
	HLE_draw_inline_array(primitiveMode, 4, ClearQuadVertex);
}

//todo check if this one is correct, I don't know where the game uses that
void RasterRenderQuadMidAsmHook(X86Context& __restrict ctx, uint8_t* base)
{



	intptr_t addr = ctx.eax.u32 - (24 * sizeof(DWORD)) - 4;

	X_D3DPRIMITIVETYPE primitiveMode = (X_D3DPRIMITIVETYPE)X86_LOAD_U32(addr - 8);
	// Vertex data starts after: 0x417FC, 8, 0x40601818
	intptr_t vertexDataAddr = addr - (24 * sizeof(DWORD));


	for (int i = 0; i < 24; i++)
	{
		RasterRenderQuadVertex[i] = X86_LOAD_F32(vertexDataAddr + i * sizeof(float));
	}

	HLE_draw_inline_array(primitiveMode, 4, RasterRenderQuadVertex);
}

const int32_t dword_55FDEC = 0x55FDEC;  //used to detect the flag for stride 24 vs 16
// This is used to draw the homing attack or spin dash aura, as well as shooting from guns.
void _XbSubmitNodeMidAsmHook(X86Context& __restrict ctx, uint8_t* base)
{


	//Get stride from flag
	uint32_t v1_addr = X86_LOAD_U32(dword_55FDEC) + 12;
	uint8_t flagStride = X86_LOAD_U8(v1_addr);

	const int32_t stride = (flagStride & 1) ? 6 : 4; // 6 or 4 DWORDs per vertex

	// Get vertex count from dword_55FDEC
	uint32_t dword_55FDEC_ptr = X86_LOAD_U32(dword_55FDEC);
	uint16_t vertexCount = X86_LOAD_U16(dword_55FDEC_ptr);

	if (vertexCount > 256) //if this condition ever pass, we should consisder increasing the size of SubmitNodeQuadVertex array
	{
		XANTHUS_WARN("Too many vertices for SubmitNodeQuad detected, capping to 256!!!");
		vertexCount = 256;
	}

	// Calculate total vertex data size
	const int32_t totalDwords = vertexCount * stride;

	int32_t vertexDataAddr = ctx.eax.u32 - sizeof(DWORD) - (totalDwords * sizeof(DWORD));

	// Primitive type is at vertexDataAddr - 8
	X_D3DPRIMITIVETYPE primitiveType = (X_D3DPRIMITIVETYPE)X86_LOAD_U32(vertexDataAddr - 8);

	for (int i = 0; i < totalDwords; i++)
	{
		SubmitNodeQuadVertex[i] = X86_LOAD_F32(vertexDataAddr + i * sizeof(float));
	}

	HLE_draw_inline_array(primitiveType, vertexCount, SubmitNodeQuadVertex);
}

//this is needed so weighted characters have proper textures / material color, and lighting.
// no push function used here, so we just copy constants.
void _rpSkinXboxSetMaterialColor(int32_t a1addr, int32_t a2)
{
	intptr_t a1 = (intptr_t)g_memory.Translate(a1addr);
	float g_VSConstants[4];
	auto base = g_memory.base;

	//lighting
	constexpr int LIGHT_CONST = 5;
	auto multi = *((float*)a1 + 3);

	float r = X86_LOAD_F32(0x55B3C0);
	float g = X86_LOAD_F32(0x55B3C4);
	float b = X86_LOAD_F32(0x55B3C8);
	float a = X86_LOAD_F32(0x55B3CC);
	if (multi == 1.0)
	{
		g_VSConstants[0] = r;
		g_VSConstants[1] = g;
		g_VSConstants[2] = b;
	}
	else
	{
		g_VSConstants[0] = r * multi;
		g_VSConstants[1] = g * multi;
		g_VSConstants[2] = b * multi;
	}

	g_VSConstants[3] = a;

	CopyVertexShaderConstant(LIGHT_CONST, (DWORD*)g_VSConstants, 1);

	//material texture
	constexpr int COLOR_CONST = 191;

	if (!a2 || *(intptr_t*)(a1 + 4) == -1)
	{
		float c = *(float*)(a1 + 20);
		g_VSConstants[0] = c;
		g_VSConstants[1] = c;
		g_VSConstants[2] = c;
		g_VSConstants[3] = 1.0f;
	
	}
	else
	{
		uint8_t* color = (uint8_t*)(a1 + 4);
		float alpha = *(float*)(a1 + 20) * (1.0f / 255.0f);

		g_VSConstants[0] = color[0] * alpha;
		g_VSConstants[1] = color[1] * alpha;
		g_VSConstants[2] = color[2] * alpha;
		g_VSConstants[3] = color[3] * (1.0f / 255.0f);
	}

	CopyVertexShaderConstant(COLOR_CONST, (DWORD*)g_VSConstants, 1);
}
#endif

// Begin and End push are essentially command system like DX12, this can be used for many things including draw.
// this patch here just make it so it won't crash when the game calls that.
static thread_local DWORD* g_beginPush = nullptr;
uint32_t D3DDevice_BeginPush(DWORD Count)
{
	auto pRet = (DWORD*)(g_userHeap.Alloc(sizeof(DWORD) * Count));
	g_beginPush = (DWORD*)pRet;
	return g_memory.MapVirtual(pRet);
}

void D3DDevice_EndPush(DWORD* pPush)
{
	if (pPush && g_beginPush)
	{
		g_userHeap.Free(g_beginPush);
		g_beginPush = nullptr;
	}
}

#if XANTHUS_RECOMP
GUEST_FUNCTION_HOOK_STDCALL(sub_3D4740, D3DDevice_BeginPush);
GUEST_FUNCTION_HOOK_STDCALL(sub_3D4760, D3DDevice_EndPush);

GUEST_FUNCTION_HOOK(sub_2C51C0, _rpSkinXboxSetMaterialColor);
#endif
