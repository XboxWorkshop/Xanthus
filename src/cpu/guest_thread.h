#pragma once

#include <kernel/kernel.h>
#include <kernel/types.h>
#include <condition_variable>


#define CURRENT_THREAD_HANDLE uint32_t(-2)

struct GuestThreadParams
{
    uint32_t function;
    uint32_t parameter;
    uint32_t value;
    uint32_t flags;
    uint32_t tlsSize;
    uint32_t kernelStackSize;
};

struct GuestThreadContext
{
#if XANTHUS_RECOMP
    X86Context x86Context{};
#endif
    uint8_t* thread = nullptr;

    GuestThreadContext(GuestThreadParams params);
    ~GuestThreadContext();
};

struct GuestThreadHandle : KernelObject
{
    GuestThreadParams params;
    std::atomic<bool> suspended;
    std::atomic<bool> resumeRequested{false};
    std::mutex suspendMutex;
    std::condition_variable suspendCV;
    std::thread thread;

    GuestThreadHandle(const GuestThreadParams& params);
    ~GuestThreadHandle() override;

    uint32_t Wait(uint32_t timeout) override;
};

struct GuestThread
{
    static uint32_t Start(const GuestThreadParams& params);
    static GuestThreadHandle* Start(const GuestThreadParams& params, uint32_t* threadId);

    static uint32_t GetCurrentThreadId();

    static uint32_t GetCurrentThreadEntry();
    //static void SetLastError(uint32_t error);

#ifdef _WIN32
    static void SetThreadName(uint32_t threadId, const char* name);
#endif
};

void DPCRoutine(X86Context* ctx);
void system_events(X86Context* ctx);
void NV2ADevice_Init();

extern TLS* m_TLS;