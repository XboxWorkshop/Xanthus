// ******************************************************************
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license, or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  (c) 2020 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#pragma once

#include <cstdint>
#include <cstddef>
#include <climits>
#include <cuchar>


namespace xbox
{
	// ******************************************************************
	// * Calling conventions
	// ******************************************************************

#define XBOXAPI             __stdcall
#define XCALLBACK           XBOXAPI

// ******************************************************************
// * Basic types
// ******************************************************************
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
	/*! addr is the type of a physical address */
	using addr_xt = std::uint32_t;
	/*! zero is the type of null address or value */
	inline constexpr addr_xt zero = 0;
	/*! zeroptr is the type of null pointer address */
	using zeroptr_xt = std::nullptr_t;
	inline constexpr zeroptr_xt zeroptr = nullptr;

	// ******************************************************************
	// * Pointer types
	// ******************************************************************
	typedef char_xt* PCHAR;
	typedef char_xt* PSZ;
	typedef const char_xt* PCSZ;
	typedef byte_xt* PBYTE;
	typedef boolean_xt* PBOOLEAN;
	typedef uchar_xt* PUCHAR;
	typedef ushort_xt* PUSHORT;
	typedef uint_xt* PUINT;
	typedef ulong_xt* PULONG;
	typedef dword_xt* PDWORD, * LPDWORD;
	typedef long_xt* PLONG;
	typedef int_ptr_xt* PINT_PTR;
	typedef void_xt* PVOID, * LPVOID;
	typedef void_xt* HANDLE;
	typedef HANDLE* PHANDLE;
	typedef size_xt* PSIZE_T;
	typedef access_mask_xt* PACCESS_MASK;
	typedef longlong_xt* PLONGLONG;
	typedef quad_xt* PQUAD;

	// ******************************************************************
	// ANSI (Multi-byte Character) types
	// ******************************************************************
	typedef char_xt* PCHAR, * LPCH, * PCH;
	typedef const char_xt* LPCCH, * PCCH;
	typedef wchar_xt* LPWSTR, * PWSTR;
	typedef /*_Null_terminated_*/ const wchar_xt* LPCWSTR, * PCWSTR;

	// ******************************************************************
	// Misc
	// ******************************************************************
	typedef struct _XD3DVECTOR {
		float_xt x;
		float_xt y;
		float_xt z;
	} D3DVECTOR;

	template<typename A, typename B>
	inline void CopyD3DVector(A& a, const B& b)
	{
		a.x = b.x;
		a.y = b.y;
		a.z = b.z;
	}

	// ******************************************************************
	// Type assertions
	// ******************************************************************
	static_assert(CHAR_BIT == 8);
	static_assert(sizeof(char16_t) == 2);

	// ******************************************************************
	// Defines
	// ******************************************************************
	constexpr uint_xt max_path{ 260 }; // Xbox file path max limitation
}


typedef struct _XOVERLAPPED {
	uint32_t Internal;
	uint32_t InternalHigh;
	uint32_t Offset;
	uint32_t OffsetHigh;
	uint32_t hEvent;
} XOVERLAPPED;

#pragma pack(push, 1)
typedef struct _FLOATING_SAVE_AREA
{
	uint16_t ControlWord;
	uint16_t StatusWord;
	uint16_t TagWord;
	uint16_t ErrorOpcode;
	uint32_t ErrorOffset;
	uint32_t ErrorSelector;
	uint32_t DataOffset;
	uint32_t DataSelector;
	uint32_t MXCsr;
	uint32_t Reserved2;
	uint8_t RegisterArea[128];
	uint8_t XmmRegisterArea[128];
	uint8_t Reserved4[224];
	uint32_t Cr0NpxState;
} FLOATING_SAVE_AREA, * PFLOATING_SAVE_AREA;
#pragma pack(pop)
static_assert(sizeof(_FLOATING_SAVE_AREA) == 0x204);

typedef struct _FX_SAVE_AREA {
	FLOATING_SAVE_AREA FloatSave;
	uint32_t Unknown[3];
} FX_SAVE_AREA, * PFX_SAVE_AREA;
static_assert(sizeof(_FX_SAVE_AREA) == 0x210);

