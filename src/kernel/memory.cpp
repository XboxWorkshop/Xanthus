#ifdef _MSC_VER
#include <stdafx.h>
#endif
#include "heap.h"
#include "memory.h"
#include "function.h"
#if XANTHUS_RECOMP
#include "x86_context.h"
#endif

Memory::Memory()
{
#ifdef _WIN32

	static constexpr uintptr_t kCandidateBases[] = 
	{
		0x100000000ull,  
		0x200000000ull,
		0x300000000ull,
		0x400000000ull,
		0x500000000ull,
	};

	for (uintptr_t candidate : kCandidateBases)
	{
		base = (uint8_t*)VirtualAlloc(
			(void*)candidate,
			X86_MEMORY_SIZE,
			MEM_RESERVE | MEM_COMMIT,
			PAGE_READWRITE
		);

		if (base)
			break;
	}


	if (base == nullptr) 
	{
		DWORD error = GetLastError();
		fprintf(stderr, "Failed to allocate at 0x10000000 (Error: 0x%lX)\n", error);
		return;
	}

	DWORD oldProtect;
	VirtualProtect(base, 4096, PAGE_NOACCESS, &oldProtect);
#else
	base = (uint8_t*)mmap((void*)0x100000000ull, X86_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

	if (base == (uint8_t*)MAP_FAILED)
		base = (uint8_t*)mmap(NULL, X86_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

	mprotect(base, 4096, PROT_NONE);
#endif


	for (size_t i = 0; X86FuncMappings[i].guest != 0; i++)
	{
		if (X86FuncMappings[i].host != nullptr)
			InsertFunction(X86FuncMappings[i].guest, X86FuncMappings[i].host);
	}
}

void* MmGetHostAddress(uint32_t ptr)
{
	return g_memory.Translate(ptr);
}


