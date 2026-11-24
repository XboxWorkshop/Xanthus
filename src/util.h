#pragma once

#include <string>
#include <vector>
#include <utility> 
#include <variant>
#include <cstdio>

static uint32_t RoundUp(uint32_t dwValue, uint32_t dwMult)
{
	if (dwMult == 0) { return dwValue; }

	int remainder = dwValue % dwMult;
	if (remainder == 0)
		return dwValue;

	return dwValue + dwMult - remainder;
}

template<typename TValue>
constexpr size_t FirstBitLow(TValue value)
{
    constexpr size_t nbits = sizeof(TValue) * 8;
    constexpr auto zero = TValue{};
    constexpr auto one = static_cast<TValue>(1);

    for (size_t i = 0; i < nbits; i++)
    {
        if ((value & (one << i)) != zero)
        {
            return i;
        }
    }

    return 0;
}


using FormatArg = std::variant<int, double, const char*, std::string>;

template <typename... Args>
static int call_sprintf_impl(char* buffer, const char* format, Args... args) 
{
    return std::sprintf(buffer, format, args...);
}

static int call_sprintf(char* buffer, const char* format, const std::vector<FormatArg>& args) 
{
    int res = 0;

    if (args.size() == 1) 
    {
        std::visit([&](auto&& arg) 
            {
                res = call_sprintf_impl(buffer, format, arg);
            }, args[0]);
    }
    else if (args.size() == 2) 
    {
        std::visit([&](auto&& a1) 
            {
            std::visit([&](auto&& a2) 
                {
                    res = call_sprintf_impl(buffer, format, a1, a2);
                }, args[1]);
            }, args[0]);
    }

    return res;
}

#include <kernel/memory.h>
enum class FormatType { INT, STRING, FLOAT, CHAR, OTHER };

static void AddArgument(std::vector<FormatArg>& vec, FormatType format, uint32_t esp)
{

    int32_t espRes = esp + 4 + (vec.size() + 2 * 4);
    auto test = *reinterpret_cast<uint32_t*>(g_memory.base + espRes);
    test = test;
    char* t = (char* )test;
    t = t;
}

static std::vector<FormatArg> parse_format_specifiers(const std::string& format, uint32_t esp)
{
    std::vector<FormatArg> specifiers;
    int curArgumentNumber = 0;

    for (size_t i = 0; i < format.size(); ) 
    {
        if (format[i] == '%') 
        {
            int count = 0;
            bool found = false;

            for (int j = 1; j < 5; j++)
            {
                switch (format[i + j])
                {
                case 'd': case 'i': case 'u': case 'x': case 'X': case 'o':
                    //specifiers.emplace_back(i, FormatType::INT);
                    AddArgument(specifiers, FormatType::INT, esp);
                    i += 2;
                    found = true;
                    break;
                case 'f': case 'F': case 'e': case 'E': case 'g': case 'G': case 'a': case 'A':
                   // specifiers.emplace_back(i, FormatType::FLOAT);
                    i += 2;
                    found = true;
                    AddArgument(specifiers, FormatType::FLOAT, esp);
                    break;
                case 'c':
                   // specifiers.emplace_back(i, FormatType::CHAR);
                    i += 2;
                    found = true;
                    AddArgument(specifiers, FormatType::CHAR, esp);
                    break;
                case 's':
                    //specifiers.emplace_back(i, FormatType::STRING);
                    i += 2;
                    found = true;
                    AddArgument(specifiers, FormatType::STRING, esp);
                    break;
                case '%': // escaped percent sign
                    i += 2;
                    found = true;
                    break;
                }

                if (found)
                {
                    curArgumentNumber++;
                    break;
                }
            }
        }
        else 
        {
            i++;
        }
    }

    return specifiers;
}


static void AppendUtf8(std::string& out, uint32_t c)
{
    if (c < 0x80)
    {
        out += (char)c;
    }
    else if (c < 0x800)
    {
        out += (char)(0xC0 | (c >> 6));
        out += (char)(0x80 | (c & 0x3F));
    }
    else if (c < 0x10000)
    {
        out += (char)(0xE0 | (c >> 12));
        out += (char)(0x80 | ((c >> 6) & 0x3F));
        out += (char)(0x80 | (c & 0x3F));
    }
    else
    {
        out += (char)(0xF0 | (c >> 18));
        out += (char)(0x80 | ((c >> 12) & 0x3F));
        out += (char)(0x80 | ((c >> 6) & 0x3F));
        out += (char)(0x80 | (c & 0x3F));
    }
}

#if XANTHUS_RECOMP
constexpr uint32_t g_ResourceManager = 0x541C5C;

// Helper function to get the slot of the first BON inside a loaded .one file, returns 0 if it had none.
// Memo: ResourceOneFile is stored at +4 (node key at +12 and value at +16).
// ResourceOneFile has its entries at +0x24 and their count at +0x28.
static uint32_t FindBonSlot(uint8_t* base, uint32_t archive)
{
    uint32_t manager = X86_LOAD_U32(g_ResourceManager);

    if (manager == 0)
        return 0;

    uint32_t resource = X86_LOAD_U32(manager + 4);
    uint32_t curResource = resource;

    for (uint32_t node = X86_LOAD_U32(resource + 4); X86_LOAD_U8(node + 21) == 0;)
    {
        if (X86_LOAD_U32(node + 12) >= archive)
        {
            curResource = node;
            node = X86_LOAD_U32(node);
        }
        else
        {
            node = X86_LOAD_U32(node + 8);
        }
    }

    if (curResource == resource || X86_LOAD_U32(curResource + 12) != archive)
        return 0;

    uint32_t oneFile = X86_LOAD_U32(curResource + 16);
    uint32_t entries = X86_LOAD_U32(oneFile + 0x24);
    uint32_t count = X86_LOAD_U32(oneFile + 0x28);

    for (uint32_t i = 0; i < count; i++)
    {
        if (X86_LOAD_U32(entries + i * 0x3C + 0x2C) == 1)
            return i + 2;
    }

    return 0;
}
#endif
