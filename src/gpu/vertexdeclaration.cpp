#include "video.h"
#include "vertexdeclaration.h"
#include "xanthus/gpu/vertexshader.h"
#include <xxHashMap.h>
#include <kernel/heap.h>
#include <algorithm>
#include "render_commands.h"
#include "FixedFunction.h"

static Mutex g_vertexDeclarationMutex;
static xxHashMap<GuestVertexDeclaration*> g_vertexDeclarations;
static VertexDeclarationKey g_fixedFunctionVertexDeclarationKey;
static bool g_hasFixedFunctionVertexDeclarationKey;
uint32_t g_fixedFunctionVertexRegisterDefaultMask = 0xFFFF;

static const char* ConvertDeclUsage(uint32_t usage)
{
	switch (usage)
	{
	case D3DDECLUSAGE_POSITION:
		return "POSITION";
	case D3DDECLUSAGE_BLENDWEIGHT:
		return "BLENDWEIGHT";
	case D3DDECLUSAGE_BLENDINDICES:
		return "BLENDINDICES";
	case D3DDECLUSAGE_NORMAL:
		return "NORMAL";
	case D3DDECLUSAGE_PSIZE:
		return "PSIZE";
	case D3DDECLUSAGE_TEXCOORD:
		return "TEXCOORD";
	case D3DDECLUSAGE_TANGENT:
		return "TANGENT";
	case D3DDECLUSAGE_BINORMAL:
		return "BINORMAL";
	case D3DDECLUSAGE_TESSFACTOR:
		return "TESSFACTOR";
	case D3DDECLUSAGE_POSITIONT:
		return "POSITIONT";
	case D3DDECLUSAGE_COLOR:
		return "COLOR";
	case D3DDECLUSAGE_FOG:
		return "FOG";
	case D3DDECLUSAGE_DEPTH:
		return "DEPTH";
	case D3DDECLUSAGE_SAMPLE:
		return "SAMPLE";
	default:
		assert(false && "Unknown usage");
		return "UNKNOWN";
	}
}

static const char* ConvertDeclTypeToString(RenderFormat type)
{
	switch (type)
	{
	case RenderFormat::R32_FLOAT:
		return "R32_FLOAT";
	case RenderFormat::R32_UINT:
		return "R32_UINT";
	case RenderFormat::R32G32_FLOAT:
		return "R32G32_FLOAT";
	case RenderFormat::R32G32B32_FLOAT:
		return "R32G32B32_FLOAT";
	case RenderFormat::R32G32B32A32_FLOAT:
		return "R32G32B32A32_FLOAT";
	case RenderFormat::B8G8R8A8_UNORM:
		return "B8G8R8A8_UNORM";
	case RenderFormat::R8G8B8A8_UINT:
		return "R8G8B8A8_UINT";
	case RenderFormat::R16G16_SINT:
		return "R16G16_SINT";
	case RenderFormat::R16G16B16A16_SINT:
		return "R16G16B16A16_SINT";
	case RenderFormat::R8G8B8A8_UNORM:
		return "R8G8B8A8_UNORM";
	case RenderFormat::R16G16_SNORM:
		return "R16G16_SNORM";
	case RenderFormat::R16G16B16A16_SNORM:
		return "R16G16B16A16_SNORM";
	case RenderFormat::R16G16_UNORM:
		return "R16G16_UNORM";
	case RenderFormat::R16G16B16A16_UNORM:
		return "R16G16B16A16_UNORM";
	case RenderFormat::R16G16_FLOAT:
		return "R16G16_FLOAT";
	case RenderFormat::R16G16B16A16_FLOAT:
		return "R16G16B16A16_FLOAT";
	default:
		assert(false && "Unknown type");
		return "UNKNOWN";
	}
}

static RenderFormat ConvertDeclType(uint32_t type)
{
	switch (type)
	{
	case D3DDECLTYPE_FLOAT1:
		return RenderFormat::R32_FLOAT;
	case D3DDECLTYPE_FLOAT2:
		return RenderFormat::R32G32_FLOAT;
	case D3DDECLTYPE_FLOAT3:
		return RenderFormat::R32G32B32_FLOAT;
	case D3DDECLTYPE_FLOAT4:
		return RenderFormat::R32G32B32A32_FLOAT;
	case D3DDECLTYPE_D3DCOLOR:
		return RenderFormat::B8G8R8A8_UNORM;
	case D3DDECLTYPE_UBYTE4:
		return RenderFormat::R8G8B8A8_UINT;
	case D3DDECLTYPE_SHORT2:
		return RenderFormat::R16G16_SINT;
	case D3DDECLTYPE_SHORT4:
		return RenderFormat::R16G16B16A16_SINT;
	case D3DDECLTYPE_UBYTE4N:
		return RenderFormat::R8G8B8A8_UNORM;
	case D3DDECLTYPE_SHORT2N:
		return RenderFormat::R16G16_SNORM;
	case D3DDECLTYPE_SHORT4N:
		return RenderFormat::R16G16B16A16_SNORM;
	case D3DDECLTYPE_USHORT2N:
		return RenderFormat::R16G16_UNORM;
	case D3DDECLTYPE_USHORT4N:
		return RenderFormat::R16G16B16A16_UNORM;
	case D3DDECLTYPE_FLOAT16_2:
		return RenderFormat::R16G16_FLOAT;
	case D3DDECLTYPE_FLOAT16_4:
		return RenderFormat::R16G16B16A16_FLOAT;
	default:
		assert(false && "Unknown type");
		return RenderFormat::UNKNOWN;
	}
}


GuestVertexDeclaration* CreateVertexDeclarationWithoutAddRef(D3DVERTEXELEMENT9* vertexElements, VertexShaderMode shaderMode)
{
	size_t vertexElementCount = 0;
	auto vertexElement = vertexElements;


	while (vertexElement->Stream != 0xFF && vertexElement->Type != D3DDECLTYPE_UNUSED)
	{

		++vertexElement;
		++vertexElementCount;
	}

	std::lock_guard lock(g_vertexDeclarationMutex);

	XXH64_hash_t hash = XXH3_64bits(vertexElements, vertexElementCount * sizeof(D3DVERTEXELEMENT9));
	auto& vertexDeclaration = g_vertexDeclarations[hash];


	if (vertexDeclaration == nullptr)
	{
		vertexDeclaration = g_userHeap.AllocPhysical<GuestVertexDeclaration>(ResourceType::VertexDeclaration);
		vertexDeclaration->hash = hash;

		static std::vector<RenderInputElement> inputElements;
		inputElements.clear();

		struct Location
		{
			uint32_t usage; //type pos, normal, blendweight etc.
			uint32_t usageIndex; //pos0, pos1, texcor0, texcord1 etc.
			uint32_t location; //actual location in the shader
		};

		constexpr Location locations[] =
		{
			{ D3DDECLUSAGE_TEXCOORD,     0, 0  },
			{ D3DDECLUSAGE_TEXCOORD,     1, 1  },
			{ D3DDECLUSAGE_TEXCOORD,     2, 2  },
			{ D3DDECLUSAGE_TEXCOORD,     3, 3  },
			{ D3DDECLUSAGE_TEXCOORD,     4, 4  },
			{ D3DDECLUSAGE_TEXCOORD,     5, 5  },
			{ D3DDECLUSAGE_TEXCOORD,     6, 6 },
			{ D3DDECLUSAGE_TEXCOORD,     7, 7 },
			{ D3DDECLUSAGE_TEXCOORD,     8, 8 },
			{ D3DDECLUSAGE_TEXCOORD,     9, 9 },
			{ D3DDECLUSAGE_TEXCOORD,    10, 10 },
			{ D3DDECLUSAGE_TEXCOORD,    11, 11 },
			{ D3DDECLUSAGE_TEXCOORD,    12, 12 },
			{ D3DDECLUSAGE_TEXCOORD,    13, 13 },
			{ D3DDECLUSAGE_TEXCOORD,    14, 14 },
			{ D3DDECLUSAGE_TEXCOORD,    15, 15 },
		};

		constexpr Location locationsFixedFunc[] =
		{
			{ D3DDECLUSAGE_POSITION,     0, 0  },
			{ D3DDECLUSAGE_BLENDWEIGHT,  0, 1  },
			{ D3DDECLUSAGE_NORMAL,		 0, 2  },
			{ D3DDECLUSAGE_COLOR,		 0, 3  },
			{ D3DDECLUSAGE_COLOR,		 1, 4  },
			{ D3DDECLUSAGE_FOG,			 0, 5  },
			{ D3DDECLUSAGE_PSIZE,		 0, 6  },

			//backColor
			{ D3DDECLUSAGE_TEXCOORD,     4, 7  },
			{ D3DDECLUSAGE_TEXCOORD,     5, 8  },

			//actual texcoord
			{ D3DDECLUSAGE_TEXCOORD,     0, 9  },
			{ D3DDECLUSAGE_TEXCOORD,     1, 10  },
			{ D3DDECLUSAGE_TEXCOORD,     2, 11 },
			{ D3DDECLUSAGE_TEXCOORD,     3, 12 },

			//reserved
			{ D3DDECLUSAGE_TEXCOORD,     6, 13 },
			{ D3DDECLUSAGE_TEXCOORD,     7, 14 },
			{ D3DDECLUSAGE_TEXCOORD,     8, 15 },

		};

		vertexElement = vertexElements;
		while (vertexElement->Stream != 0xFF && vertexElement->Type != D3DDECLTYPE_UNUSED)
		{
			if (vertexElement->Usage == D3DDECLUSAGE_POSITION && vertexElement->UsageIndex == 2)
			{
				++vertexElement;
				continue;
			}

			auto& inputElement = inputElements.emplace_back();

			inputElement.semanticName = ConvertDeclUsage(vertexElement->Usage);
			inputElement.semanticIndex = vertexElement->UsageIndex;
			inputElement.location = ~0;

			if (shaderMode == VertexShaderMode::FixedFunction)
			{
				for (auto& location : locationsFixedFunc)
				{
					if (location.usage == vertexElement->Usage && location.usageIndex == vertexElement->UsageIndex)
					{
						inputElement.location = location.location;
						break;
					}
				}
			}
			else
			{
				for (auto& location : locations)
				{
					if (location.usage == vertexElement->Usage && location.usageIndex == vertexElement->UsageIndex)
					{
						inputElement.location = location.location;
						break;
					}
				}
			}


			assert(inputElement.location != ~0);

			inputElement.format = ConvertDeclType(vertexElement->Type);
			inputElement.slotIndex = vertexElement->Stream;
			inputElement.alignedByteOffset = vertexElement->Offset;

			switch (vertexElement->Usage)
			{
			case D3DDECLUSAGE_POSITION:
				if (vertexElement->UsageIndex == 1)
					vertexDeclaration->indexVertexStream = vertexElement->Stream;
				break;

			case D3DDECLUSAGE_NORMAL:
			case D3DDECLUSAGE_TANGENT:
			case D3DDECLUSAGE_BINORMAL:
				/**if (vertexElement->Type == D3DDECLTYPE_FLOAT3)
					inputElement.format = RenderFormat::R32G32B32_UINT;
				else
					vertexDeclaration->hasR11G11B10Normal = true;*/
				break;

			case D3DDECLUSAGE_TEXCOORD:
				switch (vertexElement->Type)
				{
				case D3DDECLTYPE_SHORT2:
				case D3DDECLTYPE_SHORT4:
				case D3DDECLTYPE_SHORT2N:
				case D3DDECLTYPE_SHORT4N:
				case D3DDECLTYPE_USHORT2N:
				case D3DDECLTYPE_USHORT4N:
				case D3DDECLTYPE_FLOAT16_2:
				case D3DDECLTYPE_FLOAT16_4:
					vertexDeclaration->swappedTexcoords |= 1 << vertexElement->UsageIndex;
					break;
				}

				break;
			}

			vertexDeclaration->vertexStreams[vertexElement->Stream] = true;

			++vertexElement;
		}

		auto addInputElement = [&](uint32_t usage, uint32_t usageIndex)
			{
				uint32_t location = ~0;

				if (shaderMode == VertexShaderMode::FixedFunction)
				{
					for (auto& alsoLocation : locationsFixedFunc)
					{
						if (alsoLocation.usage == usage && alsoLocation.usageIndex == usageIndex)
						{
							location = alsoLocation.location;
							break;
						}
					}
				}
				else
				{
					for (auto& alsoLocation : locations)
					{
						if (alsoLocation.usage == usage && alsoLocation.usageIndex == usageIndex)
						{
							location = alsoLocation.location;
							break;
						}
					}
				}

				assert(location != ~0);

				for (auto& inputElement : inputElements)
				{
					if (inputElement.location == location)
						return;
				}

				auto format = RenderFormat::R32_FLOAT;
				switch (usage)
				{
				case D3DDECLUSAGE_NORMAL:
				case D3DDECLUSAGE_TANGENT:
				case D3DDECLUSAGE_BINORMAL:
				case D3DDECLUSAGE_BLENDINDICES:
					format = RenderFormat::R32G32B32_FLOAT;
					break;
				}

				inputElements.emplace_back(ConvertDeclUsage(usage), usageIndex, location, format, 15, 0);
			};

		// Assign any unbound usages to null buffer slot.
		if (shaderMode == VertexShaderMode::FixedFunction)
		{
			for (auto& location : locationsFixedFunc)
			{
				addInputElement(location.usage, location.usageIndex);
			}
		}
		else
		{
			for (auto& location : locations)
			{
				addInputElement(location.usage, location.usageIndex);
			}
		}


		vertexDeclaration->inputElements = std::make_unique<RenderInputElement[]>(inputElements.size());
		std::copy(inputElements.begin(), inputElements.end(), vertexDeclaration->inputElements.get());

		vertexDeclaration->vertexElements = std::make_unique<D3DVERTEXELEMENT9[]>(vertexElementCount + 1);
		std::copy(vertexElements, vertexElements + vertexElementCount + 1, vertexDeclaration->vertexElements.get());

		vertexDeclaration->inputElementCount = uint32_t(inputElements.size());
		vertexDeclaration->vertexElementCount = vertexElementCount + 1;
	}

	vertexDeclaration->AddRef();
	//vertexDeclaration->uniqueID = ++uniqueIDG;
	return vertexDeclaration;
}

GuestVertexDeclaration* FindVertexDeclarationByHash(uint64_t hash)
{
	std::lock_guard lock(g_vertexDeclarationMutex);
	auto it = g_vertexDeclarations.find(hash);
	return it != g_vertexDeclarations.end() ? it->second : nullptr;
}

extern uint32_t vsh_constants[HOST_D3DVS_CONSTREG_FULL][4];
void UpdateHostVertexDeclaration(HostVertexDeclaration* pHostVertexDeclaration)
{
	// Titles can specify default values for registers via calls like SetVertexData4f
	// HLSL shaders need to know whether to use vertex data or default vertex shader values
	// Any register not in the vertex declaration should be set to the default value
	float vertexDefaultFlags[X_VSH_MAX_ATTRIBUTES];
	uint32_t vertexRegisterDefaultMask = 0;
	for (int32_t i = 0; i < X_VSH_MAX_ATTRIBUTES; i++)
	{
		vertexDefaultFlags[i] = pHostVertexDeclaration->vRegisterInDeclaration[i] ? 0.0f : 1.0f;

		if (!pHostVertexDeclaration->vRegisterInDeclaration[i])
			vertexRegisterDefaultMask |= 1u << i;
	}
	if (!g_hasFixedFunctionVertexDeclarationKey ||
		g_fixedFunctionVertexDeclarationKey != pHostVertexDeclaration->Key)
	{
		g_fixedFunctionVertexDeclarationKey = pHostVertexDeclaration->Key;
		g_hasFixedFunctionVertexDeclarationKey = true;
		g_fixedFunctionVertexRegisterDefaultMask = vertexRegisterDefaultMask;
		MarkFixedFunctionVertexShaderVariantDirty();
	}


	//192 + 16 = 208 vRegisterDefaultFlagsPacked
	if (memcmp(&vsh_constants[HOST_D3DVS_CONSTREG_VREGDEFAULTS_FLAG_BASE][0], vertexDefaultFlags,
		sizeof(vertexDefaultFlags)) != 0)
		CopyVertexShaderConstant(HOST_D3DVS_CONSTREG_VREGDEFAULTS_FLAG_BASE, (DWORD*)vertexDefaultFlags,
			HOST_D3DVS_CONSTREG_VREGDEFAULTS_FLAG_SIZE);
}


static void SetVertexDeclaration(GuestVertexDeclaration* vertexDeclaration)
{
	RenderCommand cmd;
	cmd.type = RenderCommandType::SetVertexDeclaration;
	cmd.setVertexDeclaration.vertexDeclaration = vertexDeclaration;
	g_pendingRenderCommands.enqueue() = cmd;
}

static GuestVertexDeclaration* CreateVertexDeclaration(D3DVERTEXELEMENT9* vertexElements, VertexShaderMode shaderMode)
{
	auto vertexDeclaration = CreateVertexDeclarationWithoutAddRef(vertexElements, shaderMode);
	//vertexDeclaration->AddRef();
	return vertexDeclaration;
}


void UpdateVertexDeclaration(uint32_t Handle)
{
	size_t host_handle = (size_t)g_memory.Translate(Handle);

	//DWORD vshFlags = GetXboxVertexShaderForHandle(host_handle)->Flags;
	RenderCommand cmd;
	VertexShaderMode shaderMode = g_Xbox_VertexShaderMode;

	//as mentioned above, there is no separate function that create vertex declaration, so we have to manually extract and handle them.
	//we handle vertex declaration immediately to avoid a race issue.

	auto vertexElement = XSR_HostGetVertexDeclaration(host_handle, shaderMode == VertexShaderMode::FixedFunction);

	UpdateHostVertexDeclaration(vertexElement);
	auto vertexDeclaration = CreateVertexDeclaration(vertexElement->pHostVertexElements, shaderMode);
	SetVertexDeclaration(vertexDeclaration);
}