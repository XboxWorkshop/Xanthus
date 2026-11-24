
#include <kernel/function.h>
#include <gpu/video.h>
#include <gpu/vertexbuffer.h>
#include <external/rapidhash.h>
#include <xxHashMap.h>
#include <kernel/memory.h>
#include <kernel/heap.h>

std::unordered_map<GuestBuffer*, std::vector<DerivedVertexBuffer>> VBsList;
X_STREAMINPUT g_Xbox_SetStreamSource[X_VSH_MAX_STREAMS];

static xxHashMap<GuestBuffer*> g_indexedVertexBufferCache;
// Remembered content hashes per guest vertex buffer: lets repeated draws
// skip rehashing until the buffer is locked or freed.
static ankerl::unordered_dense::map<GuestBuffer*, std::vector<VBRangeHash>> g_vbTrustedHashes;

// RW has some VBs made from its dynamic pool, their content get rebuild every frame so it's not possible
// to cache them properly, so we will make a separate set and use Upload Allocator instead.
static ankerl::unordered_dense::set<GuestBuffer*> g_dynamicVertexBuffers; 

void FreeVB(uint32_t resource)
{
	GuestBuffer* resourcePtr = (GuestBuffer*)g_memory.Translate(resource);

	g_vbTrustedHashes.erase(resourcePtr);
	g_dynamicVertexBuffers.erase(resourcePtr);

	auto it = VBsList.find(resourcePtr);
	if (it != VBsList.end())
	{
		for (auto& vec : it->second)
		{
			auto cacheIt = g_indexedVertexBufferCache.find(vec.hash);
			if (cacheIt == g_indexedVertexBufferCache.end() || cacheIt->second != vec.vertexBuffer)
				continue;

			g_indexedVertexBufferCache.erase(cacheIt);
			AddDestructResource(vec.vertexBuffer);
		}

		VBsList.erase(it);
	}
}

X_STREAMINPUT* get_Xbox_SetStreamSource(uint8_t id)
{
	if (id >= X_VSH_MAX_STREAMS)
		return nullptr;

	return &g_Xbox_SetStreamSource[id];
}


#if XANTHUS_RECOMP
//CreateVertexBuffer (left over debugging)
X86_FUNC_IMPL(__imp__sub_2E6750);
X86_OVERRIDE_FUNC(sub_2E6750)
{
	int size = X86_LOAD_U32(ctx.esp.u32 + 4);
	size = (size + 31) & 0xFFFFFFE0;
	__imp__sub_2E6750(ctx, base);
	auto addr = ctx.eax.u32;
	XD3DResource* buffer = (XD3DResource*)g_memory.Translate(addr);
	//printf("Create VB %x - size: %d\n", (intptr_t)buffer, size);
	//g_VBList.push_back({ buffer, nullptr, size });
}

//CreateAndGetVertexBuffer
// As explained above, RW has its own dynamic pool for dynamc Vertex Buffer,
// Those cannot be cached, so grab them here for upload later.
X86_FUNC_IMPL(__imp__sub_2E67D0);
X86_OVERRIDE_FUNC(sub_2E67D0)
{
	__imp__sub_2E67D0(ctx, base);

	auto resource = X86_LOAD_U32(0x55F8F0);
	if (resource)
		g_dynamicVertexBuffers.insert((GuestBuffer*)(base + resource));
}
#endif

bool IsDynamicVertexBuffer(GuestBuffer* buffer)
{
	return g_dynamicVertexBuffers.contains(buffer);
}

#if XANTHUS_RECOMP
//LockVertexBuffer(X_D3DResource* buffer, ...)
X86_FUNC_IMPL(__imp__sub_3D5FE0);
X86_OVERRIDE_FUNC(sub_3D5FE0)
{
	// The locked buffer's content may be overwritten, so drop only its
	// remembered hashes. All other buffers keep theirs.
	auto resource = X86_LOAD_U32(ctx.esp.u32 + 4);
	if (resource)
		g_vbTrustedHashes.erase((GuestBuffer*)(base + resource));

	__imp__sub_3D5FE0(ctx, base);
}

//SetStreamSource
X86_FUNC_IMPL(__imp__sub_3D1E10);
X86_OVERRIDE_FUNC(sub_3D1E10) {

	auto StreamNumber = (uint32_t)GetHostParameter(ctx, 1);
	auto buffer = (GuestBuffer*)GetHostParameter(ctx, 2, CallConvention::Stdcall, ParameterType::Pointer);
	auto stride = (uint32_t)GetHostParameter(ctx, 3);

	g_Xbox_SetStreamSource[StreamNumber].VertexBuffer = buffer;
	g_Xbox_SetStreamSource[StreamNumber].Stride = stride;

	//we need to let the original code run otherwise this preven't other vertex buffer to be used
	__imp__sub_3D1E10(ctx, base);
}
#endif

void ComputeIndexRange(const int16_t* indices, uint32_t startIndex, uint32_t indexCount, uint32_t& outMin, uint32_t& outMax)
{

	const int16_t* startPtr = indices + startIndex;
	uint32_t minIdx = UINT32_MAX;
	uint32_t maxIdx = 0;
	for (uint32_t i = 0; i < indexCount; ++i)
	{
		uint32_t idx = (uint16_t)startPtr[i];
		if (idx < minIdx)
			minIdx = idx;

		if (idx > maxIdx)
			maxIdx = idx;
	}

	if (minIdx == UINT32_MAX)
	{
		minIdx = 0;
		maxIdx = 0;
	}

	outMin = minIdx;
	outMax = maxIdx;
}

static inline unsigned long GetXboxCommonResourceType(const unsigned long XboxResource_Common)
{
#define X_D3DCOMMON_TYPE_MASK          0x00070000
	unsigned long dwCommonType = XboxResource_Common & X_D3DCOMMON_TYPE_MASK;
	return dwCommonType;
}

static inline unsigned long GetXboxCommonResourceType(X_D3DResource* pXboxResource)
{
	return GetXboxCommonResourceType(pXboxResource->Common);
}

void* GetDataFromXboxResource(X_D3DResource* pXboxResource)
{
	uint32_t pData = pXboxResource->Data;
	unsigned long dwCommonType = GetXboxCommonResourceType(pXboxResource);
	pData |= 0x80000000;
	return (uint32_t*)pData;
}

GuestBuffer* GetGuestVertexBufferIndexed(int i, uint32_t minIdx, uint32_t maxIdx, uint32_t alignedStride)
{
	uint32_t usedVertices = maxIdx - minIdx + 1;
	const uint32_t originalStride = get_Xbox_SetStreamSource(i)->Stride;

	unsigned long dataAddr = (unsigned long)GetDataFromXboxResource((X_D3DResource*)(g_Xbox_SetStreamSource[i].VertexBuffer));
	uint8_t* pXboxVertexData = reinterpret_cast<uint8_t*>(g_memory.Translate(dataAddr));

	pXboxVertexData += (minIdx * originalStride); //we only want to hash and copy the used vertices, the unused can be skipped.

	const size_t originalSize = usedVertices * originalStride;

	// Lock-dirty: the content hash is remembered per drawn range and reused
	// until this exact buffer is locked or freed.
	const uint32_t rangeOffset = minIdx * originalStride;
	const uint32_t rangeLength = usedVertices * originalStride;
	uint64_t hash = 0;
	bool hashTrusted = false;
	auto& trustedRanges = g_vbTrustedHashes[g_Xbox_SetStreamSource[i].VertexBuffer];
	for (const auto& trusted : trustedRanges)
	{
		if (trusted.offset == rangeOffset && trusted.length == rangeLength)
		{
			hash = trusted.hash;
			hashTrusted = true;
			break;
		}
	}

	if (!hashTrusted)
	{
		// rapidhash over XXH3: these ranges average ~0.1 KB, where rapidhash's
		// small-input path wins; XXH3 is faster only on large buffers.
		hash = rapidhash(pXboxVertexData, originalSize);

		// Same range drawn with new content replaces the stale entry.
		bool replaced = false;
		for (auto& trusted : trustedRanges)
		{
			if (trusted.offset == rangeOffset && trusted.length == rangeLength)
			{
				trusted.hash = hash;
				replaced = true;
				break;
			}
		}
		if (!replaced)
			trustedRanges.push_back({ rangeOffset, rangeLength, hash });
	}

	auto& bufferCache = g_indexedVertexBufferCache[hash];
	if (bufferCache == nullptr)
	{
		uint8_t* HostVertexData = ApplyVertexPatch(pXboxVertexData, i, usedVertices, originalStride, alignedStride);
		const size_t size = usedVertices * alignedStride;
		auto newBuffer = CreateVertexBuffer(size);

		newBuffer->hash = hash;
		if (HostVertexData == nullptr) //if patch didn't happen simply copy the vertex as they are
		{
			newBuffer->mappedMemory = g_userHeap.AllocPhysical(newBuffer->dataSize, 0x10);
			memcpy(newBuffer->mappedMemory, pXboxVertexData, originalSize);
		}
		else
		{
			newBuffer->mappedMemory = HostVertexData; //we already have the vertex copied from the patch earlier so just assign them
		}

		UnlockVertexBuffer(newBuffer); //send our VB to the GPU

		// Publish only after the upload is queued, so another thread can
		// never draw with a buffer whose GPU copy is not on the queue yet.
		auto& vbDerived = VBsList[g_Xbox_SetStreamSource[i].VertexBuffer];
		DerivedVertexBuffer newDerivedVB = { newBuffer, hash };
		vbDerived.push_back(newDerivedVB);

		bufferCache = newBuffer;
	}

	return bufferCache;
}
