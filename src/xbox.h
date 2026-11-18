#pragma once
#include <cstdint>
#include <string>

#ifndef IN
#define IN
#endif

#ifndef _WIN32
using PVOID = void*;
#endif

#ifdef _WIN32
#include <windows.h>
#endif

extern "C" void* MmGetHostAddress(uint32_t ptr);

template<typename T>
struct xpointer
{
    uint32_t ptr;

    // Default constructor (nullptr)
    xpointer() : ptr(0) {}

    // Constructor from uint32_t (Xbox address)
    explicit xpointer(uint32_t address) : ptr(address) {}

    // Constructor from host pointer
    explicit xpointer(T* host_ptr) : ptr(host_ptr ?
        (reinterpret_cast<uintptr_t>(host_ptr) - reinterpret_cast<uintptr_t>(MmGetHostAddress(0))) : 0) {
    }

    // Get host pointer
    T* get() const {
        return ptr ? reinterpret_cast<T*>(MmGetHostAddress(ptr)) : nullptr;
    }

    // Dereference operators
    operator T* () const { return get(); }
    T* operator->() const { return get(); }

    // Boolean conversion for if(Dpc) checks
    explicit operator bool() const { return ptr != 0; }
};

struct xaddr32
{
    uint32_t v;

    constexpr xaddr32() : v(0) {}
    constexpr xaddr32(uint32_t val) : v(val) {}

    template<typename T>
    T* as() const
    {
        return v ? reinterpret_cast<T*>(MmGetHostAddress(v)) : nullptr;
    }

    explicit operator bool() const { return v != 0; }
};
static_assert(sizeof(xaddr32) == 4);


using void_xt = void;
using char_xt = char;
using cchar_xt = char;
using wchar_xt = char16_t;
using short_xt = std::int16_t;
using cshort_xt = std::int16_t;
using long_xt = std::int32_t;
using uchar_xt = std::uint8_t;
using byte_xt = std::uint8_t;
using boolean_xt = std::uint8_t;
using ushort_xt = std::uint16_t;
using word_xt = std::uint16_t;
using ulong_xt = std::uint32_t;
using dword_xt = std::uint32_t;
using size_xt = ulong_xt;
using access_mask_xt = ulong_xt;
using physical_address_xt = ulong_xt;
using uint_xt = std::uint32_t;
using int_xt = std::int32_t;
using int_ptr_xt = int_xt;
using long_ptr_xt = long_xt;
using ulong_ptr_xt = ulong_xt;
using longlong_xt = std::int64_t;
using ulonglong_xt = std::uint64_t;
using quad_xt = std::uint64_t; // 8 byte aligned 8 byte long
using bool_xt = std::int32_t;
using hresult_xt = long_xt;
using ntstatus_xt = long_xt;
using float_xt = float;

typedef void_xt(*PKDEFERRED_ROUTINE)
(
    IN struct _KDPC* Dpc,
    IN PVOID         DeferredContext,
    IN PVOID         SystemArgument1,
    IN PVOID         SystemArgument2
    );

typedef struct _XLIST_ENTRY
{
    uint32_t Flink;
    uint32_t Blink;
} XLIST_ENTRY;

struct _XLIST_ENTRY;
typedef _XLIST_ENTRY XLIST_ENTRY;
typedef xpointer<XLIST_ENTRY> PXLIST_ENTRY;

typedef struct _KDPC
{
    cshort_xt              Type;               // 0x00
    union
    {
        uchar_xt           Number;             // 0x02
        boolean_xt         Inserted;           // 0x02
    };
    uchar_xt               Importance;         // 0x03
    XLIST_ENTRY          DpcListEntry;       // 0x04
    xaddr32  DeferredRoutine;    // 0x0C
    xaddr32               DeferredContext;
    xaddr32               SystemArgument1;
    xaddr32               SystemArgument2;
}
KDPC, * PKDPC;

struct TLS
{
    uint32_t dwDataStartAddr;             // raw start address
    uint32_t dwDataEndAddr;               // raw end address
    uint32_t dwTLSIndexAddr;              // tls index address
    uint32_t dwTLSCallbackAddr;           // tls callback address
    uint32_t dwSizeofZeroFill;            // size of zero fill
    uint32_t dwCharacteristics;           // characteristics
};




typedef struct _XDISPATCHER_HEADER
{

    uchar_xt       Type;           // 0x00
    uchar_xt       Absolute;       // 0x01
    uchar_xt       Size;           // 0x02
    uchar_xt       Inserted;       // 0x03
    long_xt        SignalState;    // 0x04
    XLIST_ENTRY  WaitListHead;   // 0x08

} XDISPATCHER_HEADER, * XPDISPATCHER_HEADER;

typedef XDISPATCHER_HEADER XKEVENT;

typedef struct _XRTL_CRITICAL_SECTION
{
    XDISPATCHER_HEADER   Event;
    long_xt             LockCount;
    long_xt             RecursionCount;
    uint32_t           OwningThread;
} XRTL_CRITICAL_SECTION;

typedef struct _XANSI_STRING {
    uint16_t Length;
    uint16_t MaximumLength;
    xpointer<char> Buffer;
} XANSI_STRING;
