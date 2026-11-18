#pragma once

#include <plume_render_interface.h>

#include <cassert>
#include <cstdint>
#include <mutex>
#include <vector>

// A slice of upload memory.
// - memory: CPU pointer to write the data to.
// - buffer + offset: where the GPU reads it from.
// - deviceAddress: unused here, other allocators use it.
struct UploadAllocation
{
	const plume::RenderBuffer* buffer;
	uint64_t offset;
	uint8_t* memory;
	uint64_t deviceAddress;
};

// Staging memory for vertex and index buffer uploads on discrete GPUs.
//
// How it works:
// - The game thread writes the data here.
// - The render thread copies it into the real buffer.
//
// Info:
// - There's one per game frame, g_stagedBufferUploads.
// - Present resets this once the GPU has finished that frame.
//
// Sizing:
// - Expands by 16 MB when a frame needs more.
struct StagedBufferUploadAllocator
{
	static constexpr size_t SIZE = 16 * 1024 * 1024;

	// A 16 MB GPU buffer the CPU can write to.
	struct Block
	{
		std::unique_ptr<plume::RenderBuffer> buffer;
		uint8_t* memory = nullptr;
	};

	std::mutex mutex;
	std::vector<Block> buffers;
	uint32_t index = 0;
	uint32_t offset = 0;

	// Reserves `size` bytes at the given alignment.
	UploadAllocation allocate(uint32_t size, uint32_t alignment, plume::RenderDevice& device)
	{
		assert(size <= SIZE);

		std::lock_guard lock(mutex);
		offset = (offset + alignment - 1) & ~(alignment - 1); // align

		// Move to the next block if this one is full.
		if (offset + size > SIZE)
		{
			++index;
			offset = 0;
		}

		// Create the block on first use.
		if (buffers.size() <= index)
			buffers.resize(index + 1);

		auto& buffer = buffers[index];
		if (buffer.buffer == nullptr)
		{
			// Make new block if we're out of space.
			buffer.buffer = device.createBuffer(plume::RenderBufferDesc::UploadBuffer(SIZE, plume::RenderBufferFlag::VERTEX | plume::RenderBufferFlag::INDEX));
			buffer.memory = reinterpret_cast<uint8_t*>(buffer.buffer->map());
		}

		// Carve out slice, advance cursor.
		auto ref = buffer.buffer->at(offset);
		offset += size;

		return { ref.ref, ref.offset, buffer.memory + ref.offset, 0 };
	}

	// Starts again from the first block.
	void reset()
	{
		std::lock_guard lock(mutex);
		index = 0;
		offset = 0;
	}
};
