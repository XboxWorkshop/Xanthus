#if XANTHUS_RECOMP // This header is really only necessary or valid for recompilation projects.

#pragma once
#include "memory.h"
#include <tuple>
#include <array>
#include "xbox_types.h"
#include "types.h"
#include "heap.h"
#ifdef _WIN32
#include <winnt.h>
#include <minwindef.h>
#include <Windows.h>
#include <windef.h>
#endif
#include <unordered_map>
#include <type_traits>
#include <cassert>
#include <bit>

enum CallConvention
{
    Cdecl = 0,
    Stdcall = 1,
	Fastcall,
	Thiscall,
    Usercall,
    Userpurge
};

enum ParameterType
{
    Integer,
    Float,
    Pointer
};

enum ParameterSize
{
    U8,
    U16,
    U32,
    F32,
    F64,
    F80
};

enum Register
{
    rEAX,
    rEBX,
    rECX,
    rEDX,
};

template <typename R, typename... T>
constexpr std::tuple<T...> function_args(R(*)(T...)) noexcept
{
    return std::tuple<T...>();
}

template <typename R, typename... T>
constexpr std::tuple<T...> function_args(R(*)(X86Context&, T...)) noexcept
{
    return std::tuple<T...>();
}

template <typename F>
struct takes_ctx_t : std::false_type {};

template <typename R, typename... T>
struct takes_ctx_t<R(*)(X86Context&, T...)> : std::true_type {};

template<auto V>
static constexpr decltype(V) constant_v = V;

template<typename T>
static constexpr bool is_float_v = std::is_same_v<T, float>;

template<typename T>
static constexpr bool is_double_v = std::is_same_v<T, double>;

template<auto Func>
struct arg_count_t
{
    static constexpr size_t value = std::tuple_size_v<decltype(function_args(Func))>;
};

template<typename TCallable, int I = 0, typename ...TArgs>
std::enable_if_t<(I >= sizeof...(TArgs)), void> _tuple_for(std::tuple<TArgs...>&, const TCallable& callable) noexcept
{
}

template<typename TCallable, int I = 0, typename ...TArgs>
std::enable_if_t<(I < sizeof...(TArgs)), void> _tuple_for(std::tuple<TArgs...>& tpl, const TCallable& callable) noexcept
{
    callable(std::get<I>(tpl), I);
    _tuple_for<TCallable, I + 1>(tpl, callable);
}


struct ArgTranslator
{
    static uint32_t GetIntegerArgumentValue(const X86Context& ctx, uint8_t* base, size_t arg, size_t argCount = 0, CallConvention cc = Cdecl) noexcept
    {
        if (cc == Fastcall)
        {
            // First two arguments in registers
            if (arg == 0) return ctx.ecx.u32;
            if (arg == 1) return ctx.edx.u32;

            // Remaining arguments are on the stack, after the return address
            const uint32_t stack_ptr = ctx.esp.u32;
            const uint32_t offset = 4 + ((arg - 2) * 4);
            return *reinterpret_cast<uint32_t*>(base + stack_ptr + offset);
        }

        if (cc == Thiscall)
        {
            // 'this' pointer is in ECX
            if (arg == 0)
            {
                return ctx.ecx.u32;
            }

            const uint32_t stack_ptr = ctx.esp.u32;
            const uint32_t offset = 4 + ((arg - 1) * 4);
            auto value = *reinterpret_cast<uint32_t*>(base + stack_ptr + offset);
            return value;
        }


        uint32_t esp = ctx.esp.u32 + 4 + (arg * 4);
        return *reinterpret_cast<uint32_t*>(base + esp);
    }

    // Set integer value on the stack
    static void SetIntegerArgumentValue(X86Context& ctx, uint8_t* base, size_t arg, uint32_t value, CallConvention cc = Cdecl) noexcept
    {
        if (cc == Thiscall)
        {
            if (arg == 0) 
            { ctx.ecx.u32 = value; return; }


            const uint32_t stack_ptr = ctx.esp.u32;
            *reinterpret_cast<uint32_t*>(base + stack_ptr + ((arg - 1) * 4)) = value;
            return;
        }

        if (cc == Fastcall)
        {
            if (arg == 0) 
            { ctx.ecx.u32 = value; return; }

            if (arg == 1) 
            { ctx.edx.u32 = value; return; }


            const uint32_t stack_ptr = ctx.esp.u32;
            *reinterpret_cast<uint32_t*>(base + stack_ptr + ((arg - 2) * 4)) = value;
            return;
        }

        const uint32_t stack_ptr = ctx.esp.u32;
        *reinterpret_cast<uint32_t*>(base + stack_ptr + (arg * 4)) = value;
    }

    static float GetFloatArgumentValue(const X86Context& ctx, uint8_t* base, size_t arg, size_t argCount = 0, CallConvention cc = Cdecl) noexcept
    {
        if (cc == Fastcall)
        {
            // First two arguments in registers
            if (arg == 0) 
                return ctx.ecx.u32;

            if (arg == 1) 
                return ctx.edx.u32;

            // Remaining arguments are on the stack, after the return address
            const uint32_t stack_ptr = ctx.esp.u32;
            const uint32_t offset = 4 + ((arg - 2) * 4);
            return *reinterpret_cast<float*>(base + stack_ptr + offset);
        }

        if (cc == Thiscall)
        {
            // 'this' pointer is in ECX
            if (arg == 0)
            {
                return ctx.ecx.u32;
            }

            const uint32_t stack_ptr = ctx.esp.u32;
            const uint32_t offset = 4 + ((arg - 1) * 4);
            auto value = *reinterpret_cast<float*>(base + stack_ptr + offset);
            return value;
        }


        uint32_t esp = ctx.esp.u32 + 4 + (arg * 4);
        return *reinterpret_cast<float*>(base + esp);
    }

    // Set float value on the stack
    static void SetFloatArgumentValue(X86Context& ctx, uint8_t* base, size_t arg, uint32_t value, CallConvention cc = Cdecl) noexcept
    {
        if (cc == Thiscall)
        {
            if (arg == 0) 
            { ctx.ecx.u32 = value; return; }

            const uint32_t stack_ptr = ctx.esp.u32;
            *reinterpret_cast<uint32_t*>(base + stack_ptr + ((arg - 1) * 4)) = value;
            return;
        }

        if (cc == Fastcall)
        {
            if (arg == 0) 
            { ctx.ecx.u32 = value; return; }

            if (arg == 1) 
            { ctx.edx.u32 = value; return; }

            const uint32_t stack_ptr = ctx.esp.u32;
            *reinterpret_cast<uint32_t*>(base + stack_ptr + ((arg - 2) * 4)) = value;
            return;
        }

        const uint32_t stack_ptr = ctx.esp.u32;
        *reinterpret_cast<uint32_t*>(base + stack_ptr + (arg * 4)) = value;
    }


    template<typename T>
    static std::enable_if_t<!std::is_pointer_v<T>, T> GetValue(const X86Context& ctx, uint8_t* base, size_t idx, const size_t argCount = 0, CallConvention cc = Cdecl) noexcept
    {

        if constexpr (is_float_v<T>)
        {
            return static_cast<T>(GetFloatArgumentValue(ctx, base, idx));
        }
        /**else if constexpr (is_double_v<T>)
        {
            return static_cast<T>(GetDoubleArgumentValue(ctx, base, idx));
        }*/
        else
        {
            return static_cast<T>(GetIntegerArgumentValue(ctx, base, idx, argCount, cc));
        }
    }

    template<typename T>
    static std::enable_if_t<std::is_pointer_v<T>, T> GetValue(const X86Context& ctx, uint8_t* base, size_t idx, const size_t argCount = 0, CallConvention cc = Cdecl) noexcept
    {
        const auto v = GetIntegerArgumentValue(ctx, base, idx, argCount, cc);
        if (!v)
        {
            return nullptr;
        }

        return reinterpret_cast<T>(base + v);
    }

    template<typename T>
    static std::enable_if_t<!std::is_pointer_v<T>, void> SetValue(X86Context& ctx, uint8_t* base, size_t idx, T value, CallConvention cc = CallConvention::Cdecl) noexcept
    {
        if constexpr (std::is_null_pointer_v<T>)
        {
            SetIntegerArgumentValue(ctx, base, idx, 0, cc);
        }
        else if constexpr (std::is_pointer_v<T>)
        {
            SetIntegerArgumentValue(ctx, base, idx, g_memory.MapVirtual(value), cc);
        }
        else if constexpr (is_float_v<T>)
        {
            SetFloatArgumentValue(ctx, base, idx, std::bit_cast<uint32_t>(value), cc);
        }
        else
        {
            SetIntegerArgumentValue(ctx, base, idx, static_cast<uint32_t>(value), cc);
        }
    }

    template<typename T>
    static std::enable_if_t<std::is_pointer_v<T>, void> SetValue(X86Context& ctx, uint8_t* base, size_t idx, T value, CallConvention cc = CallConvention::Cdecl) noexcept
    {
        const auto v = g_memory.MapVirtual((void*)value);
        if (!v)
        {
            return;
        }

        SetValue(ctx, base, idx, v, cc);
    }
};



struct Argument
{
    enum Type { Integer = 0, Float = 1, Double = 2 };
    Type type{};
    int ordinal{};
};

template<typename T1>
constexpr std::array<Argument, std::tuple_size_v<T1>> GatherFunctionArguments(const T1& tpl)
{
    std::array<Argument, std::tuple_size_v<T1>> args{};

    size_t i{};

    if constexpr (!args.empty())
    {
        std::apply([&](const auto& first, const auto&... rest)
            {
                auto append = [&]<typename T2>(const T2 & v)
                {
                    if constexpr (is_float_v<T2>)
                    {
                        args[i] = { Argument::Float, static_cast<int>(i) };
                    }
                    else if constexpr (is_double_v<T2>)
                    {
                        args[i] = { Argument::Double, static_cast<int>(i) };
                    }
                    else
                    {
                        args[i] = { Argument::Integer, static_cast<int>(i) };
                    }
                    i++;
                };

                append(first);
                (append(rest), ...);
            }, tpl);
    }

    return args;
}

template<auto Func>
constexpr std::array<Argument, arg_count_t<Func>::value> GatherFunctionArguments()
{
    return GatherFunctionArguments(function_args(Func));
}

template<auto Func, size_t I>
struct arg_ordinal_t
{
    static constexpr size_t value = GatherFunctionArguments<Func>()[I].ordinal;
};

// Update the argument translation functions to accept CallConvention
template<auto Func, CallConvention CC, int I = 0, typename ...TArgs>
std::enable_if_t<(I >= sizeof...(TArgs)), void>
_translate_args_to_host(const X86Context& ctx, uint8_t* base, std::tuple<TArgs...>& tpl) noexcept
{
}

template<auto Func, CallConvention CC, int I = 0, typename ...TArgs>
std::enable_if_t<(I < sizeof...(TArgs)), void>
    _translate_args_to_host(const X86Context& ctx, uint8_t* base, std::tuple<TArgs...>& tpl) noexcept
{
    using T = std::tuple_element_t<I, std::remove_reference_t<decltype(tpl)>>;
    std::get<I>(tpl) = ArgTranslator::GetValue<T>(ctx, base, arg_ordinal_t<Func, I>::value, arg_count_t<Func>::value, CC);
    _translate_args_to_host<Func, CC, I + 1>(ctx, base, tpl);
}



template <CallConvention CC, int I = 0, typename ...TArgs>
std::enable_if_t<(I >= sizeof...(TArgs)), void>
_translate_args_to_guest(X86Context& ctx, uint8_t* base, std::tuple<TArgs...>& tpl) noexcept
{
}

template <CallConvention CC, int I = 0, typename ...TArgs>
std::enable_if_t<(I < sizeof...(TArgs)), void>
    _translate_args_to_guest(X86Context& ctx, uint8_t* base, std::tuple<TArgs...>& tpl) noexcept
{
    using T = std::tuple_element_t<I, std::remove_reference_t<decltype(tpl)>>;
    ArgTranslator::SetValue<T>(ctx, base,
        GatherFunctionArguments(std::tuple<TArgs...>{})[I].ordinal,
        std::get<I>(tpl), 
        CC);
    _translate_args_to_guest<CC, I + 1>(ctx, base, tpl);
}

//Function to handle translation from host (C++) to guest (x86)
//return esp is handled automatically for some specific cases ONLY. There is no support for jmp or 64 bits arguments (int64, long double etc.)
template<auto Func, CallConvention CC>
X86_FUNC(HostToGuestFunction)
{
    auto args = function_args(Func);
    _translate_args_to_host<Func, CC>(ctx, base, args);

    auto call = [&]() -> decltype(auto)
    {
        if constexpr (takes_ctx_t<decltype(Func)>::value)
            return std::apply([&](auto... rest) { return Func(ctx, rest...); }, args);
        else
            return std::apply(Func, args);
    };
    using ret_t = decltype(call());

    if constexpr (std::is_same_v<ret_t, void>)
    {
        call();
    }
    else
    {
        auto v = call();

        if constexpr (std::is_pointer_v<ret_t>)
        {
            if (v != nullptr)
            {
                ctx.eax.u32 = static_cast<uint32_t>(reinterpret_cast<size_t>(v) - reinterpret_cast<size_t>(base));
            }
            else
            {
                ctx.eax.u32 = 0;
            }
        }
        else
        {
            ctx.eax.u32 = static_cast<uint32_t>(v);

            // 64-bit results come back in edx:eax.
            if constexpr (sizeof(ret_t) == 8)
                ctx.edx.u32 = static_cast<uint32_t>(static_cast<uint64_t>(v) >> 32);
        }
    }

    if constexpr (CC == Stdcall || CC == Thiscall)
    {
        // Remove arguments from the stack (callee cleans up)
        ctx.esp.u32 += static_cast<uint32_t>((arg_count_t<Func>::value - (CC == Thiscall ? 1 : 0)) * 4);
    }
    else if constexpr (CC == Fastcall)
    {
        // Only clean up stack arguments (first two are in registers)
        const int stackArgs = std::max(0, static_cast<int>(arg_count_t<Func>::value) - 2);
        ctx.esp.u32 += static_cast<uint32_t>(stackArgs * 4);
    }

	ctx.esp.u32 += 0x4; //Adjust ESP for the return address (this will affect jmp tail todo: find a way to detect those)
}

// Function to call from guest (x86) to host (C++)
template<typename T, CallConvention CC = CallConvention::Cdecl,
    typename TFunction, typename... TArgs>
T GuestToHostFunction(const X86Context& currentCtx, const TFunction& func, TArgs&&... argv)
{
    auto args = std::make_tuple(std::forward<TArgs>(argv)...);

    X86Context newCtx{}; // Create a fresh context
    newCtx.esp = currentCtx.esp;
    newCtx.ebp = currentCtx.ebp;
    newCtx.kpcr_ptr = currentCtx.kpcr_ptr;

    // reserve the stack args below caller esp
    constexpr int regArgs = CC == CallConvention::Thiscall ? 1 : CC == CallConvention::Fastcall ? 2 : 0;
    constexpr int stackArgs = std::max(0, static_cast<int>(sizeof...(TArgs)) - regArgs);
    newCtx.esp.u32 -= stackArgs * 4;

    // Set up the arguments
    _translate_args_to_guest<CC>(newCtx, g_memory.base, args);

    newCtx.esp.u32 -= 4; //adjust esp for CALL instruction

    if constexpr (std::is_function_v<TFunction>)
        func(newCtx, g_memory.base);
    else
        g_memory.FindFunction(func)(newCtx, g_memory.base);

    if constexpr (std::is_pointer_v<T>)
    {
        return reinterpret_cast<T>(g_memory.Translate(newCtx.eax.u32));
    }
    else if constexpr (std::is_integral_v<T>)
    {
        return static_cast<T>(newCtx.eax.u32);
    }
    else
    {
        static_assert(std::is_void_v<T>, "Unsupported return type.");
    }
}

/*
* Get and convert a parameter of a guest function for the host, this can then be used to edit or check data
@param
* paramID - the parameter ID requested, example if you want to grab the first argument of the function, pick 1, if you want the second 2 etc.
* funcType - specify the function type, cdecl, stdcall etc. Default to Cdecl and does not work with usercall and userpurge at the moment
* ParamType - specify if the parameter is a int, a float or a pointer using ParameterType enum. Default to int.
* Float parameters are returned as raw bits; use GetHostParameterFloat to get the value as a float.
*/
inline uintptr_t GetHostParameter(const X86Context& ctx, int8_t paramID, CallConvention funcType = CallConvention::Cdecl, ParameterType ParamType = ParameterType::Integer, ParameterSize paramSize = ParameterSize::U32)
{
    if (paramID <= 0)
        paramID = 1;

    auto base = g_memory.base;

    switch (funcType)
    {
    case CallConvention::Cdecl:
    case CallConvention::Stdcall:
    default:
        if (ParamType == ParameterType::Pointer)
        {
            auto ptr = X86_LOAD_U32(ctx.esp.u32 + (4 * paramID));
            if (!ptr)
                return 0;

            return (uintptr_t)g_memory.Translate(ptr);
        }
        else
        {
            if (paramSize == ParameterSize::U8)
                return (uintptr_t)X86_LOAD_U8(ctx.esp.u32 + (4 * paramID));
            else if (paramSize == ParameterSize::U16)
                return (uintptr_t)X86_LOAD_U16(ctx.esp.u32 + (4 * paramID));
            else
                return (uintptr_t)X86_LOAD_U32(ctx.esp.u32 + (4 * paramID));
        }
        break;
    case CallConvention::Thiscall:
        if (paramID == 1)
        {
            if (ctx.ecx.u32 == 0)
                return 0;

            return (uintptr_t)g_memory.Translate(ctx.ecx.u32);
        }
        if (ParamType == ParameterType::Pointer)
        {
            auto ptr = X86_LOAD_U32(ctx.esp.u32 + (4 * (paramID - 1)));
            if (!ptr)
                return 0;

            return (uintptr_t)g_memory.Translate(ptr);
        }
        else
        {
            return (uintptr_t)X86_LOAD_U32(ctx.esp.u32 + (4 * (paramID - 1)));
        }
        break;
    case CallConvention::Fastcall:
        if (paramID == 1)
        {
            if (ParamType == ParameterType::Pointer)
            {
                if (ctx.ecx.u32 == 0)
                    return 0;

                return (uintptr_t)g_memory.Translate(ctx.ecx.u32);
            }
            else
                return (uintptr_t)X86_LOAD_U32(ctx.ecx.u32);
        }
        else if (paramID == 2)
        {
            if (ParamType == ParameterType::Pointer)
            {
                if (ctx.edx.u32 == 0)
                    return 0;

                return (uintptr_t)g_memory.Translate(ctx.edx.u32);
            }
            else
                return (uintptr_t)X86_LOAD_U32(ctx.edx.u32);
        }
        else
        {
            if (ParamType == ParameterType::Pointer)
            {
                auto ptr = X86_LOAD_U32(ctx.esp.u32 + (4 * (paramID - 2)));
                if (!ptr)
                    return 0;

                return (uintptr_t)g_memory.Translate(ptr);
            }
            else
                return (uintptr_t)X86_LOAD_U32(ctx.esp.u32 + (4 * (paramID - 2)));
        }

       break;
    }
}

inline float GetHostParameterFloat(const X86Context& ctx, int8_t paramID, CallConvention funcType = CallConvention::Cdecl)
{
    return std::bit_cast<float>(static_cast<uint32_t>(GetHostParameter(ctx, paramID, funcType, ParameterType::Float)));
}

X86_FUNC(StubWithCleanStack);

// Macro for hooking functions
#define GUEST_FUNCTION_HOOK(subroutine, function) \
    X86_OVERRIDE_FUNC(subroutine) { HostToGuestFunction<function, CallConvention::Cdecl>(ctx, base); }

#define GUEST_FUNCTION_HOOK_STDCALL(subroutine, function) \
    X86_OVERRIDE_FUNC(subroutine) { HostToGuestFunction<function, CallConvention::Stdcall>(ctx, base); }

//todo
#define GUEST_FUNCTION_HOOK_FASTCALL(subroutine, function) \
    X86_OVERRIDE_FUNC(subroutine) { HostToGuestFunction<function, CallConvention::Fastcall>(ctx, base); }

//todo
#define GUEST_FUNCTION_HOOK_THISCALL(subroutine, function) \
    X86_OVERRIDE_FUNC(subroutine) { HostToGuestFunction<function, CallConvention::Thiscall>(ctx, base); }

// Macro for creating function stubs
#define GUEST_FUNCTION_STUB(subroutine) \
    X86_OVERRIDE_FUNC(subroutine) { StubWithCleanStack(ctx, base); }
#endif
