#pragma once
#include "video.h"

typedef void(*FormatToARGBRow)(const uint8_t* src, uint8_t* dst_argb, int width);

BOOL FormatIsSwizzled(X_D3DFORMAT Format);

void UnswizzleBox
(
	const void* pSrcBuff,
	const uint32_t dwWidth,
	const uint32_t dwHeight,
	const uint32_t dwDepth,
	const uint32_t dwBytesPerPixel,
	const void* pDstBuff,
	const uint32_t dwDstRowPitch,
	const uint32_t dwDstSlicePitch
);

DWORD XBFormatBitsPerPixel(X_D3DFORMAT Format);
BOOL XBFormatIsCompressed(X_D3DFORMAT Format);

bool XBFormatRequiresConversionToARGB(X_D3DFORMAT Format);

bool ConvertD3DTextureToARGBBuffer(
	X_D3DFORMAT X_Format,
	uint8_t* pSrc,
	int32_t SrcWidth, int32_t SrcHeight, int32_t SrcRowPitch, int32_t SrcSlicePitch,
	uint8_t* pDst, int32_t DstRowPitch, int32_t DstSlicePitch,
	uint32_t uiDepth,
	void* pPalleteData
);

BOOL FormatIsLinear(X_D3DFORMAT Format);
BOOL FormatIsDepthBuffer(X_D3DFORMAT Format);
BOOL FormatIsRenderTarget(X_D3DFORMAT Format);
