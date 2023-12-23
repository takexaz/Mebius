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

	class InlineHookData {
	public:
		virtual ~InlineHookData() = default;
		virtual const std::vector<const void*>& GetInlineHooks() const noexcept = 0;
		virtual const code_t* GetTrampolineCode() const noexcept = 0;
	};

	MEBIUSAPI const InlineHookData& _GetInlineHookData(uint32_t address);
	MEBIUSAPI const InlineHookData* _GetInlineHookDataNullable(uint32_t address) noexcept;
	MEBIUSAPI void _SetHookVEH(uint32_t hookTarget, const void* hookFunction) noexcept;

	namespace internal {
		using pvfc_t = void(*)(PCONTEXT);
	}
	static void HookInlineVEH(uint32_t address, const internal::pvfc_t callee) noexcept {
		_SetHookVEH(address, std::bit_cast<const void*>(callee));
	}
}