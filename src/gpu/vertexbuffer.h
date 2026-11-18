#pragma once

// Remembered vertex content hashes per guest buffer, dropped when that exact
// buffer is locked or freed. The hash covers one byte range, so
// offset + length fully identify it.
struct VBRangeHash
{
	uint32_t offset;
	uint32_t length;
	uint64_t hash;
};


struct DerivedVertexBuffer
{
	GuestBuffer* vertexBuffer;
	uint64_t hash;
};

struct X_STREAMINPUT
{
	GuestBuffer* VertexBuffer;
	UINT                Stride;

};

void ComputeIndexRange(const int16_t* indices, uint32_t startIndex, uint32_t indexCount, uint32_t& outMin, uint32_t& outMax);
GuestBuffer* GetGuestVertexBufferIndexed(int i, uint32_t minIdx, uint32_t maxIdx, uint32_t alignedStride);
X_STREAMINPUT* get_Xbox_SetStreamSource(uint8_t id);
void FreeVB(uint32_t resource);
bool IsDynamicVertexBuffer(GuestBuffer* buffer);