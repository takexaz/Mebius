#pragma once

#include <Windows.h>

#include <error.hpp>

#include <bit>
#include <optional>
#include <vector>

#ifdef MEBIUS_EXPORT
#define MEBIUSAPI __declspec(dllexport)
#else
#define MEBIUSAPI __declspec(dllimport)
#endif

namespace mebius::inline_hook {
	using code_t = uint8_t;

	enum MBEFlags {
		CF = 0x00000001,
		PF = 0x00000004,
		AF = 0x00000010,
		ZF = 0x00000040,
		SF = 0x00000080,
		TF = 0x00000100,
		IF = 0x00000200,
		DF = 0x00000400,
		OF = 0x00000800,
	};

	struct MBCONTEXT
	{
		uint32_t 	EFlags;
		uint32_t	Edi;
		uint32_t 	Esi;
		uint32_t	Ebp;
		uint32_t 	Esp;
		uint32_t  	Ebx;
		uint32_t 	Edx;
		uint32_t 	Ecx;
		uint32_t 	Eax;
		uint32_t 	Eip;
	};
	using PMBCONTEXT = MBCONTEXT*;

	class InlineHookData {
	public:
		virtual ~InlineHookData() = default;
		virtual const std::vector<const void*>& GetInlineHooks() const noexcept = 0;
		virtual const code_t* GetTrampolineCode() const noexcept = 0;
	};

	MEBIUSAPI const InlineHookData& _GetInlineHookData(uint32_t address);
	MEBIUSAPI const InlineHookData* _GetInlineHookDataNullable(uint32_t address) noexcept;
	MEBIUSAPI void _SetInlineHook(uint32_t hookTarget, const void* hookFunction, bool isVEH) noexcept;

	namespace internal {
		using pvfc_t = void(*)(PMBCONTEXT);
	}
	static void HookInline(uint32_t address, const internal::pvfc_t callee) noexcept {
		_SetInlineHook(address, std::bit_cast<const void*>(callee), false);
	}
	static void HookInline(uint32_t address, const internal::pvfc_t callee, bool isVEH) noexcept {
		_SetInlineHook(address, std::bit_cast<const void*>(callee), isVEH);
	}
}