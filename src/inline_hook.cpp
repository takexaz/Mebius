#include <_inline_hook.hpp>
#include <_alloc.hpp>
#include <_reassemble.hpp>

#include <error.hpp>

#include <Zydis/Zydis.h>

#include <Windows.h>
#include <format>

using namespace mebius;
using namespace mebius::inline_hook;

static inline code_t* make_trampoline_code_inline(uint32_t address) noexcept;
static inline std::pair<bool, InlineHookDataImpl&> add_inline_hook_data(uint32_t address) noexcept;
static LONG WINAPI hook_veh(struct _EXCEPTION_POINTERS* ExceptionInfo) noexcept;
static inline void write_halt_opcode(uint32_t address);
static inline void write_jmp_opcode(uint32_t address, const void* func) noexcept;

MEBIUSAPI const InlineHookData& mebius::inline_hook::_GetInlineHookData(uint32_t address)
{
	decltype(_INLINE_HOOK_LIST)::iterator it = _INLINE_HOOK_LIST.find(address);
	if (it == _INLINE_HOOK_LIST.end()) {
		throw MebiusError(std::vformat("Mebius has not hook on address 0x{:08X}.", std::make_format_args(address)));
	}
	else {
		return it->second;
	}
}
MEBIUSAPI const InlineHookData* mebius::inline_hook::_GetInlineHookDataNullable(uint32_t address) noexcept
{
	decltype(_INLINE_HOOK_LIST)::iterator it = _INLINE_HOOK_LIST.find(address);
	if (it == _INLINE_HOOK_LIST.end()) {
		return nullptr;
	}
	else {
		return &(it->second);
	}
}

MEBIUSAPI void mebius::inline_hook::_SetHookVEH(uint32_t hookTarget, const void* hookFunction) noexcept {
	auto [unhooked, hook] = add_inline_hook_data(hookTarget);
	hook.AppendInlineHook(hookFunction);
	if (unhooked) {
		try {
			write_halt_opcode(hookTarget);
		}
		catch (const MebiusError& e) {
			ShowErrorDialog(e.what());
		}
	}
}

static inline code_t* make_trampoline_code_inline(uint32_t address) noexcept {
	try {
		reassemble::Reassembler code{ address, 1 };
		size_t memSize = code.GetSize() + 5;
		auto mem = alloc::CodeAllocator::GetInstance().Allocate(memSize);
		code.Reassemble(mem, memSize);
		write_jmp_opcode(std::bit_cast<uint32_t>(mem) + code.GetSize(), std::bit_cast<void*>(address + code.GetOriginalSize()));
		return mem;
	}
	catch (const MebiusError& e) {
		ShowErrorDialog(e.what());
	}
}

InlineHookDataImpl::InlineHookDataImpl(uint32_t address) noexcept {
	_trampoline_code = make_trampoline_code_inline(address);
}

InlineHookDataImpl::~InlineHookDataImpl() noexcept {
	if (_trampoline_code != nullptr) {
		alloc::CodeAllocator::GetInstance().DeAllocate(_trampoline_code);
		_trampoline_code = nullptr;
	}
}

static inline std::pair<bool, InlineHookDataImpl&> add_inline_hook_data(uint32_t address) noexcept
{
	decltype(_INLINE_HOOK_LIST)::iterator it = _INLINE_HOOK_LIST.find(address);
	if (it == _INLINE_HOOK_LIST.end()) {
		decltype(_INLINE_HOOK_LIST)::iterator item = std::get<0>(_INLINE_HOOK_LIST.emplace(address, address));
		AddVectoredExceptionHandler(TRUE, hook_veh);
		return { true, item->second };
	}
	else {
		return { false, it->second };
	}
}

static LONG __stdcall hook_veh(_EXCEPTION_POINTERS* ExceptionInfo) noexcept
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode = STATUS_PRIVILEGED_INSTRUCTION) {
		uint32_t _esp = ExceptionInfo->ContextRecord->Esp;

		auto hook = _GetInlineHookDataNullable(ExceptionInfo->ContextRecord->Eip);
		if (!hook) {
			return EXCEPTION_CONTINUE_SEARCH;
		}

		for (auto&& f : hook->GetInlineHooks()) {
			auto inline_function = std::bit_cast<internal::pvfc_t>(f);
			inline_function(ExceptionInfo->ContextRecord);
		}

		auto trampoline = std::bit_cast<DWORD>(hook->GetTrampolineCode());
		ExceptionInfo->ContextRecord->Eip = trampoline;
		ExceptionInfo->ContextRecord->Esp = _esp;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else {
		return EXCEPTION_CONTINUE_SEARCH;
	}
}

static inline void write_halt_opcode(uint32_t address) {
	auto ptr = std::bit_cast<code_t*>(address);
	DWORD oldProtect;
	if (VirtualProtect(ptr, 5, PAGE_EXECUTE_READWRITE, &oldProtect) == 0) {
		throw MebiusError(std::vformat("Can't change the page protect of 0x{:08X}.", std::make_format_args(address)));
	}
	ptr[0] = _OPCODE_EX_HALT;
	VirtualProtect(ptr, 5, oldProtect, &oldProtect);
}

static inline void write_jmp_opcode(uint32_t address, const void* func) noexcept {
	auto ptr = std::bit_cast<code_t*>(address);
	ptr[0] = _OPCODE_REL_JMP;
	auto callee = std::bit_cast<uint32_t*>(address + 1);
	callee[0] = std::bit_cast<uint32_t>(func) - (address + 5);
}
