#include "video.h"
#include "guest_convert.h"

RenderFormat ConvertFormat(X_D3DFORMAT format)
{
	switch (format)
	{

	case X_D3DFMT_L8:
		return RenderFormat::R8_UNORM;
	case X_D3DFMT_A1R5G5B5:
	case X_D3DFMT_A4R4G4B4:
	case X_D3DFMT_R5G6B5:
		return RenderFormat::R16_UNORM;
	case X_D3DFMT_INDEX16:
		return RenderFormat::R16_UINT;
	case X_D3DFMT_A8R8G8B8:
	case X_D3DFMT_X8R8G8B8:
	case X_D3DFMT_LIN_X8R8G8B8:
	case X_D3DFMT_P8: 	 //Palette get converted by the CPU to ARGB in ConvertD3DTextureToARGBBuffer
		return RenderFormat::B8G8R8A8_UNORM;

		// Those are not used by the game afaik, but this will help modding
	case X_D3DFMT_DXT1:
		return RenderFormat::BC1_UNORM;
	case X_D3DFMT_DXT3:
		return RenderFormat::BC2_UNORM;
	case X_D3DFMT_DXT5:
		return RenderFormat::BC3_UNORM;

	case X_D3DFMT_LIN_D24S8: //Linear 24-bit depth + 8-bit stencil

		return RenderFormat::D32_FLOAT;
	default:

		assert(false && "Unknown format");
		return RenderFormat::R8_UNORM;
	}
}


RenderTextureAddressMode ConvertTextureAddressMode(size_t value)
{
	switch (value)
	{
	case 0: //DX8 can set to 0, it's safe to default to wrap.
	case D3DTADDRESS_WRAP:
		return RenderTextureAddressMode::WRAP;
	case D3DTADDRESS_MIRROR:
		return RenderTextureAddressMode::MIRROR;
	case D3DTADDRESS_CLAMP:
		return RenderTextureAddressMode::CLAMP;
	case D3DTADDRESS_BORDER:
		return RenderTextureAddressMode::BORDER;
	default:
		assert(false && "Unknown texture address mode");
		return RenderTextureAddressMode::UNKNOWN;
	}
}

RenderFilter ConvertTextureFilter(uint32_t value)
{
	switch (value)
	{
	case X_D3DTEXF_POINT:
	case X_D3DTEXF_NONE:
		return RenderFilter::NEAREST;
	case X_D3DTEXF_LINEAR:
	case X_D3DTEXF_ANISOTROPIC:
		return RenderFilter::LINEAR;
	default:
		assert(false && "Unknown texture filter");
		return RenderFilter::UNKNOWN;
	}
}

RenderBorderColor ConvertBorderColor(uint32_t value)
{
	//Extract ARGB
	uint8_t a = (value >> 24) & 0xFF;
	uint8_t r = (value >> 16) & 0xFF;
	uint8_t g = (value >> 8) & 0xFF;
	uint8_t b = value & 0xFF;

	if (a == 0x00) // Fully transparent
		return RenderBorderColor::TRANSPARENT_BLACK;

	if (r == 0 && g == 0 && b == 0)
		return RenderBorderColor::OPAQUE_BLACK;

	if (r == 255 && g == 255 && b == 255)
		return RenderBorderColor::OPAQUE_WHITE;


	assert(false && "Unknown border color");
	return RenderBorderColor::UNKNOWN;
}

bool isCapabilityTriangleFan();
RenderPrimitiveTopology ConvertPrimitiveType(uint32_t primitiveType)
{
	switch (primitiveType)
	{
	case X_D3DPT_POINTLIST:
		return RenderPrimitiveTopology::POINT_LIST;
	case X_D3DPT_LINELIST:
		return RenderPrimitiveTopology::LINE_LIST;
	case X_D3DPT_LINESTRIP:
		return RenderPrimitiveTopology::LINE_STRIP;
	case X_D3DPT_TRIANGLELIST:
	case X_D3DPT_QUADLIST:
		return RenderPrimitiveTopology::TRIANGLE_LIST;
	case X_D3DPT_TRIANGLESTRIP:
		return RenderPrimitiveTopology::TRIANGLE_STRIP;
	case X_D3DPT_TRIANGLEFAN:
		return isCapabilityTriangleFan() ? RenderPrimitiveTopology::TRIANGLE_FAN : RenderPrimitiveTopology::TRIANGLE_LIST;
		//todo check if this is correct:
	case X_D3DPT_QUADSTRIP:
		return RenderPrimitiveTopology::TRIANGLE_STRIP;
	case X_D3DPT_POLYGON:
		return isCapabilityTriangleFan() ?
			RenderPrimitiveTopology::TRIANGLE_FAN :
			RenderPrimitiveTopology::TRIANGLE_LIST;
	default:
		assert(false && "Unknown primitive type");
		return RenderPrimitiveTopology::UNKNOWN;
	}
}

RenderBlend ConvertBlendMode(uint32_t blendMode)
{
	switch (blendMode)
	{
	case D3DBLEND_ZERO:
		return RenderBlend::ZERO;
	case D3DBLEND_ONE:
		return RenderBlend::ONE;
	case D3DBLEND_SRCCOLOR:
		return RenderBlend::SRC_COLOR;
	case D3DBLEND_INVSRCCOLOR:
		return RenderBlend::INV_SRC_COLOR;
	case D3DBLEND_SRCALPHA:
		return RenderBlend::SRC_ALPHA;
	case D3DBLEND_INVSRCALPHA:
		return RenderBlend::INV_SRC_ALPHA;
	case D3DBLEND_DESTCOLOR:
		return RenderBlend::DEST_COLOR;
	case D3DBLEND_INVDESTCOLOR:
		return RenderBlend::INV_DEST_COLOR;
	case D3DBLEND_DESTALPHA:
		return RenderBlend::DEST_ALPHA;
	case D3DBLEND_INVDESTALPHA:
		return RenderBlend::INV_DEST_ALPHA;
	default:
		assert(false && "Invalid blend mode");
		return RenderBlend::ZERO;
	}
}

RenderBlend ConvertAlphaBlendMode(uint32_t blendMode)
{
	switch (blendMode)
	{
	case D3DBLEND_ZERO:
		return RenderBlend::ZERO;
	case D3DBLEND_ONE:
		return RenderBlend::ONE;
	case D3DBLEND_SRCALPHA:
	case D3DBLEND_SRCCOLOR:
		return RenderBlend::SRC_ALPHA;
	case D3DBLEND_INVSRCALPHA:
	case D3DBLEND_INVSRCCOLOR:
		return RenderBlend::INV_SRC_ALPHA;
	case D3DBLEND_DESTALPHA:
	case D3DBLEND_DESTCOLOR:
		return RenderBlend::DEST_ALPHA;
	case D3DBLEND_INVDESTALPHA:
	case D3DBLEND_INVDESTCOLOR:
		return RenderBlend::INV_DEST_ALPHA;
	default:
		assert(false && "Invalid blend mode");
		return RenderBlend::ZERO;
	}
}

RenderBlendOperation ConvertBlendOp(uint32_t blendOp)
{
	switch (blendOp)
	{
	case D3DBLENDOP_ADD:
		return RenderBlendOperation::ADD;
	case D3DBLENDOP_SUBTRACT:
		return RenderBlendOperation::SUBTRACT;
	case D3DBLENDOP_REVSUBTRACT:
		return RenderBlendOperation::REV_SUBTRACT;
	case D3DBLENDOP_MIN:
		return RenderBlendOperation::MIN;
	case D3DBLENDOP_MAX:
		return RenderBlendOperation::MAX;
	default:
		assert(false && "Unknown blend operation");
		return RenderBlendOperation::ADD;
	}
}

RenderComparisonFunction ConvertD3DCMP(uint32_t value)
{
	RenderComparisonFunction comparisonFunc;

	switch (value)
	{
	case D3DCMP_NEVER:
		comparisonFunc = RenderComparisonFunction::NEVER;
		break;
	case D3DCMP_LESS:
		comparisonFunc = RenderComparisonFunction::LESS;
		break;
	case D3DCMP_EQUAL:
		comparisonFunc = RenderComparisonFunction::EQUAL;
		break;
	case D3DCMP_LESSEQUAL:
		comparisonFunc = RenderComparisonFunction::LESS_EQUAL;
		break;
	case D3DCMP_GREATER:
		comparisonFunc = RenderComparisonFunction::GREATER;
		break;
	case D3DCMP_NOTEQUAL:
		comparisonFunc = RenderComparisonFunction::NOT_EQUAL;
		break;
	case D3DCMP_GREATEREQUAL:
		comparisonFunc = RenderComparisonFunction::GREATER_EQUAL;
		break;
	case D3DCMP_ALWAYS:
		comparisonFunc = RenderComparisonFunction::ALWAYS;
		break;
	default:
		assert(false && "Unknown comparison function");
		comparisonFunc = RenderComparisonFunction::NEVER;
		break;
	}

	return comparisonFunc;
}


RenderStencilOp ConvertStencilOp(uint32_t stencilOp)
{
	switch (stencilOp)
	{
	case D3DSTENCILOP_KEEP:
		return RenderStencilOp::KEEP;
	case D3DSTENCILOP_ZERO:
		return RenderStencilOp::ZERO;
	case D3DSTENCILOP_REPLACE:
		return RenderStencilOp::REPLACE;
	case D3DSTENCILOP_INCRSAT:
		return RenderStencilOp::INCREMENT_AND_CLAMP;
	case D3DSTENCILOP_DECRSAT:
		return RenderStencilOp::DECREMENT_AND_CLAMP;
	case D3DSTENCILOP_INVERT:
		return RenderStencilOp::INVERT;
	case D3DSTENCILOP_INCR:
		return RenderStencilOp::INCREMENT_AND_WRAP;
	case D3DSTENCILOP_DECR:
		return RenderStencilOp::DECREMENT_AND_WRAP;
	default:
		assert(false && "Unknown stencil op");
		return RenderStencilOp::KEEP;
	}
}

RenderFormat ConvertDXGIFormat(ddspp::DXGIFormat format)
{
	switch (format)
	{
	case ddspp::R32G32B32A32_TYPELESS:
		return RenderFormat::R32G32B32A32_TYPELESS;
	case ddspp::R32G32B32A32_FLOAT:
		return RenderFormat::R32G32B32A32_FLOAT;
	case ddspp::R32G32B32A32_UINT:
		return RenderFormat::R32G32B32A32_UINT;
	case ddspp::R32G32B32A32_SINT:
		return RenderFormat::R32G32B32A32_SINT;
	case ddspp::R32G32B32_TYPELESS:
		return RenderFormat::R32G32B32_TYPELESS;
	case ddspp::R32G32B32_FLOAT:
		return RenderFormat::R32G32B32_FLOAT;
	case ddspp::R32G32B32_UINT:
		return RenderFormat::R32G32B32_UINT;
	case ddspp::R32G32B32_SINT:
		return RenderFormat::R32G32B32_SINT;
	case ddspp::R16G16B16A16_TYPELESS:
		return RenderFormat::R16G16B16A16_TYPELESS;
	case ddspp::R16G16B16A16_FLOAT:
		return RenderFormat::R16G16B16A16_FLOAT;
	case ddspp::R16G16B16A16_UNORM:
		return RenderFormat::R16G16B16A16_UNORM;
	case ddspp::R16G16B16A16_UINT:
		return RenderFormat::R16G16B16A16_UINT;
	case ddspp::R16G16B16A16_SNORM:
		return RenderFormat::R16G16B16A16_SNORM;
	case ddspp::R16G16B16A16_SINT:
		return RenderFormat::R16G16B16A16_SINT;
	case ddspp::R32G32_TYPELESS:
		return RenderFormat::R32G32_TYPELESS;
	case ddspp::R32G32_FLOAT:
		return RenderFormat::R32G32_FLOAT;
	case ddspp::R32G32_UINT:
		return RenderFormat::R32G32_UINT;
	case ddspp::R32G32_SINT:
		return RenderFormat::R32G32_SINT;
	case ddspp::R8G8B8A8_TYPELESS:
		return RenderFormat::R8G8B8A8_TYPELESS;
	case ddspp::R8G8B8A8_UNORM:
		return RenderFormat::R8G8B8A8_UNORM;
	case ddspp::R8G8B8A8_UINT:
		return RenderFormat::R8G8B8A8_UINT;
	case ddspp::R8G8B8A8_SNORM:
		return RenderFormat::R8G8B8A8_SNORM;
	case ddspp::R8G8B8A8_SINT:
		return RenderFormat::R8G8B8A8_SINT;
	case ddspp::B8G8R8A8_UNORM:
		return RenderFormat::B8G8R8A8_UNORM;
	case ddspp::B8G8R8X8_UNORM:
		return RenderFormat::B8G8R8A8_UNORM;
	case ddspp::R16G16_TYPELESS:
		return RenderFormat::R16G16_TYPELESS;
	case ddspp::R16G16_FLOAT:
		return RenderFormat::R16G16_FLOAT;
	case ddspp::R16G16_UNORM:
		return RenderFormat::R16G16_UNORM;
	case ddspp::R16G16_UINT:
		return RenderFormat::R16G16_UINT;
	case ddspp::R16G16_SNORM:
		return RenderFormat::R16G16_SNORM;
	case ddspp::R16G16_SINT:
		return RenderFormat::R16G16_SINT;
	case ddspp::R32_TYPELESS:
		return RenderFormat::R32_TYPELESS;
	case ddspp::D32_FLOAT:
		return RenderFormat::D32_FLOAT;
	case ddspp::R32_FLOAT:
		return RenderFormat::R32_FLOAT;
	case ddspp::R32_UINT:
		return RenderFormat::R32_UINT;
	case ddspp::R32_SINT:
		return RenderFormat::R32_SINT;
	case ddspp::R8G8_TYPELESS:
		return RenderFormat::R8G8_TYPELESS;
	case ddspp::R8G8_UNORM:
		return RenderFormat::R8G8_UNORM;
	case ddspp::R8G8_UINT:
		return RenderFormat::R8G8_UINT;
	case ddspp::R8G8_SNORM:
		return RenderFormat::R8G8_SNORM;
	case ddspp::R8G8_SINT:
		return RenderFormat::R8G8_SINT;
	case ddspp::R16_TYPELESS:
		return RenderFormat::R16_TYPELESS;
	case ddspp::R16_FLOAT:
		return RenderFormat::R16_FLOAT;
	case ddspp::D16_UNORM:
		return RenderFormat::D16_UNORM;
	case ddspp::R16_UNORM:
		return RenderFormat::R16_UNORM;
	case ddspp::R16_UINT:
		return RenderFormat::R16_UINT;
	case ddspp::R16_SNORM:
		return RenderFormat::R16_SNORM;
	case ddspp::R16_SINT:
		return RenderFormat::R16_SINT;
	case ddspp::R8_TYPELESS:
		return RenderFormat::R8_TYPELESS;
	case ddspp::R8_UNORM:
		return RenderFormat::R8_UNORM;
	case ddspp::R8_UINT:
		return RenderFormat::R8_UINT;
	case ddspp::R8_SNORM:
		return RenderFormat::R8_SNORM;
	case ddspp::R8_SINT:
		return RenderFormat::R8_SINT;
	case ddspp::BC1_TYPELESS:
		return RenderFormat::BC1_TYPELESS;
	case ddspp::BC1_UNORM:
		return RenderFormat::BC1_UNORM;
	case ddspp::BC1_UNORM_SRGB:
		return RenderFormat::BC1_UNORM_SRGB;
	case ddspp::BC2_TYPELESS:
		return RenderFormat::BC2_TYPELESS;
	case ddspp::BC2_UNORM:
		return RenderFormat::BC2_UNORM;
	case ddspp::BC2_UNORM_SRGB:
		return RenderFormat::BC2_UNORM_SRGB;
	case ddspp::BC3_TYPELESS:
		return RenderFormat::BC3_TYPELESS;
	case ddspp::BC3_UNORM:
		return RenderFormat::BC3_UNORM;
	case ddspp::BC3_UNORM_SRGB:
		return RenderFormat::BC3_UNORM_SRGB;
	case ddspp::BC4_TYPELESS:
		return RenderFormat::BC4_TYPELESS;
	case ddspp::BC4_UNORM:
		return RenderFormat::BC4_UNORM;
	case ddspp::BC4_SNORM:
		return RenderFormat::BC4_SNORM;
	case ddspp::BC5_TYPELESS:
		return RenderFormat::BC5_TYPELESS;
	case ddspp::BC5_UNORM:
		return RenderFormat::BC5_UNORM;
	case ddspp::BC5_SNORM:
		return RenderFormat::BC5_SNORM;
	case ddspp::BC6H_TYPELESS:
		return RenderFormat::BC6H_TYPELESS;
	case ddspp::BC6H_UF16:
		return RenderFormat::BC6H_UF16;
	case ddspp::BC6H_SF16:
		return RenderFormat::BC6H_SF16;
	case ddspp::BC7_TYPELESS:
		return RenderFormat::BC7_TYPELESS;
	case ddspp::BC7_UNORM:
		return RenderFormat::BC7_UNORM;
	case ddspp::BC7_UNORM_SRGB:
		return RenderFormat::BC7_UNORM_SRGB;
	default:
		assert(false && "Unsupported format from DDS.");
		return RenderFormat::UNKNOWN;
	}
}

RenderTextureDimension ConvertTextureDimension(ddspp::TextureType type)
{
	switch (type)
	{
	case ddspp::Texture1D:
		return RenderTextureDimension::TEXTURE_1D;
	case ddspp::Texture2D:
	case ddspp::Cubemap:
		return RenderTextureDimension::TEXTURE_2D;
	case ddspp::Texture3D:
		return RenderTextureDimension::TEXTURE_3D;
	default:
		assert(false && "Unknown texture type from DDS.");
		return RenderTextureDimension::UNKNOWN;
	}
}

RenderTextureViewDimension ConvertTextureViewDimension(ddspp::TextureType type)
{
	switch (type)
	{
	case ddspp::Texture1D:
		return RenderTextureViewDimension::TEXTURE_1D;
	case ddspp::Texture2D:
		return RenderTextureViewDimension::TEXTURE_2D;
	case ddspp::Texture3D:
		return RenderTextureViewDimension::TEXTURE_3D;
	case ddspp::Cubemap:
		return RenderTextureViewDimension::TEXTURE_CUBE;
	default:
		assert(false && "Unknown texture type from DDS.");
		return RenderTextureViewDimension::UNKNOWN;
	}
}