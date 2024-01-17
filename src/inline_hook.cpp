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
static LONG WINAPI hook_inline_veh(struct _EXCEPTION_POINTERS* ExceptionInfo) noexcept;
static LONG WINAPI hook_inline_veh_unsafe(struct _EXCEPTION_POINTERS* ExceptionInfo) noexcept;
static inline void write_halt_opcode(uint32_t address);
static inline void write_call_opcode(uint32_t address, const void* func);
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

MEBIUSAPI void mebius::inline_hook::_SetInlineHook(uint32_t hookTarget, const void* hookFunction, bool isVEH) noexcept {
	auto [unhooked, hook] = add_inline_hook_data(hookTarget);
	hook.AppendInlineHook(hookFunction);
	if (unhooked) {
		try {
			if (isVEH) {
				static PVOID handle = AddVectoredExceptionHandler(TRUE, hook_inline_veh);
				write_halt_opcode(hookTarget);
			}
			else {
				write_call_opcode(hookTarget, hook_inline_cushion);
			}
		}
		catch (const MebiusError& e) {
			ShowErrorDialog(e.what());
		}
	}
}
MEBIUSAPI void mebius::inline_hook::_SetInlineHookUnsafe(uint32_t hookTarget, const void* hookFunction, bool isVEH) noexcept {
	auto [unhooked, hook] = add_inline_hook_data(hookTarget);
	hook.AppendInlineHook(hookFunction);
	if (unhooked) {
		try {
			if (isVEH) {
				static PVOID handle = AddVectoredExceptionHandler(TRUE, hook_inline_veh_unsafe);
				write_halt_opcode(hookTarget);
			}
			else {
				write_call_opcode(hookTarget, hook_inline_cushion_unsafe);
			}
		}
		catch (const MebiusError& e) {
			ShowErrorDialog(e.what());
		}
	}
}

static inline code_t* make_trampoline_code_inline(uint32_t address) noexcept {
	try {
		reassemble::Reassembler code{ address, 5 };
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
		return { true, item->second };
	}
	else {
		return { false, it->second };
	}
}


// CALL用のHOOK実行(Unsafe)
extern "C" inline const void hook_inline(const PMBCONTEXT context) {
	auto hook = _GetInlineHookDataNullable(context->Eip);
	if (!hook) {
		return;
	}

	uint32_t _esp = context->Esp;
	uint32_t _eip = context->Eip;

	for (auto&& f : hook->GetInlineHooks()) {
		auto inline_function = std::bit_cast<internal::pvfc_t>(f);
		inline_function(context);

		context->Esp = _esp;
		context->Eip = _eip;
	}

	auto trampoline = std::bit_cast<DWORD>(hook->GetTrampolineCode());
	context->Eip = trampoline;
	return;
}

// VEH用のHOOK実行(Unsafe)
static LONG __stdcall hook_inline_veh(_EXCEPTION_POINTERS* ExceptionInfo) noexcept
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_PRIVILEGED_INSTRUCTION) {
		auto hook = _GetInlineHookDataNullable(ExceptionInfo->ContextRecord->Eip);
		if (!hook) {
			return EXCEPTION_CONTINUE_SEARCH;
		}

		uint32_t _eip = ExceptionInfo->ContextRecord->Eip;
		uint32_t _esp = ExceptionInfo->ContextRecord->Esp;

		MBCONTEXT context = {
		ExceptionInfo->ContextRecord->EFlags,
		ExceptionInfo->ContextRecord->Edi,
		ExceptionInfo->ContextRecord->Esi,
		ExceptionInfo->ContextRecord->Ebp,
		ExceptionInfo->ContextRecord->Esp,
		ExceptionInfo->ContextRecord->Ebx,
		ExceptionInfo->ContextRecord->Edx,
		ExceptionInfo->ContextRecord->Ecx,
		ExceptionInfo->ContextRecord->Eax,
		ExceptionInfo->ContextRecord->Eip,
		};


		for (auto&& f : hook->GetInlineHooks()) {
			auto inline_function = std::bit_cast<internal::pvfc_t>(f);
			inline_function(&context);

			context.Eip = _eip;
			context.Esp = _esp;
		}

		// Restore
		{
			ExceptionInfo->ContextRecord->EFlags = context.EFlags;
			ExceptionInfo->ContextRecord->Edi = context.Edi;
			ExceptionInfo->ContextRecord->Esi = context.Esi;
			ExceptionInfo->ContextRecord->Ebp = context.Ebp;
			ExceptionInfo->ContextRecord->Ebx = context.Ebx;
			ExceptionInfo->ContextRecord->Edx = context.Edx;
			ExceptionInfo->ContextRecord->Ecx = context.Ecx;
			ExceptionInfo->ContextRecord->Eax = context.Eax;
		}

		auto trampoline = std::bit_cast<DWORD>(hook->GetTrampolineCode());
		ExceptionInfo->ContextRecord->Eip = trampoline;


		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else {
		return EXCEPTION_CONTINUE_SEARCH;
	}
}

// CALL用のHOOK実行(Unsafe)
extern "C" inline const void hook_inline_unsafe(const PMBCONTEXT context) {
	auto hook = _GetInlineHookDataNullable(context->Eip);
	if (!hook) {
		return;
	}
	
	uint32_t _eip = context->Eip;

	for (auto&& f : hook->GetInlineHooks()) {
		auto inline_function = std::bit_cast<internal::pvfc_t>(f);
		inline_function(context);
	}

	// trampoline if not edit eip
	if (_eip == context->Eip) {
		auto trampoline = std::bit_cast<DWORD>(hook->GetTrampolineCode());
		context->Eip = trampoline;
	}
	return;
}

// VEH用のHOOK実行(Unsafe)
static LONG __stdcall hook_inline_veh_unsafe(_EXCEPTION_POINTERS* ExceptionInfo) noexcept
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_PRIVILEGED_INSTRUCTION) {
		auto hook = _GetInlineHookDataNullable(ExceptionInfo->ContextRecord->Eip);
		if (!hook) {
			return EXCEPTION_CONTINUE_SEARCH;
		}

		MBCONTEXT context = {
		ExceptionInfo->ContextRecord->EFlags,
		ExceptionInfo->ContextRecord->Edi,
		ExceptionInfo->ContextRecord->Esi,
		ExceptionInfo->ContextRecord->Ebp,
		ExceptionInfo->ContextRecord->Esp,
		ExceptionInfo->ContextRecord->Ebx,
		ExceptionInfo->ContextRecord->Edx,
		ExceptionInfo->ContextRecord->Ecx,
		ExceptionInfo->ContextRecord->Eax,
		ExceptionInfo->ContextRecord->Eip,
		};

		for (auto&& f : hook->GetInlineHooks()) {
			auto inline_function = std::bit_cast<internal::pvfc_t>(f);
			inline_function(&context);
		}

		uint32_t _eip = ExceptionInfo->ContextRecord->Eip;

		{
			ExceptionInfo->ContextRecord->EFlags = context.EFlags;
			ExceptionInfo->ContextRecord->Edi = context.Edi;
			ExceptionInfo->ContextRecord->Esi = context.Esi;
			ExceptionInfo->ContextRecord->Ebp = context.Ebp;
			ExceptionInfo->ContextRecord->Esp = context.Esp;
			ExceptionInfo->ContextRecord->Ebx = context.Ebx;
			ExceptionInfo->ContextRecord->Edx = context.Edx;
			ExceptionInfo->ContextRecord->Ecx = context.Ecx;
			ExceptionInfo->ContextRecord->Eax = context.Eax;
			ExceptionInfo->ContextRecord->Eip = context.Eip;
		}

		// trampoline if not edit eip
		if (ExceptionInfo->ContextRecord->Eip == _eip) {
			auto trampoline = std::bit_cast<DWORD>(hook->GetTrampolineCode());
			ExceptionInfo->ContextRecord->Eip = trampoline;
		}


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

static inline void write_call_opcode(uint32_t address, const void* func) {
	auto ptr = std::bit_cast<code_t*>(address);
	DWORD oldProtect;
	if (VirtualProtect(ptr, 5, PAGE_EXECUTE_READWRITE, &oldProtect) == 0) {
		throw MebiusError(std::vformat("Can't change the page protect of 0x{:08X}.", std::make_format_args(address)));
	}
	ptr[0] = _OPCODE_REL_CALL;
	auto callee = std::bit_cast<uint32_t*>(address + 1);
	callee[0] = std::bit_cast<uint32_t>(func) - (address + 5);
	VirtualProtect(ptr, 5, oldProtect, &oldProtect);
}

static inline void write_jmp_opcode(uint32_t address, const void* func) noexcept {
	auto ptr = std::bit_cast<code_t*>(address);
	ptr[0] = _OPCODE_REL_JMP;
	auto callee = std::bit_cast<uint32_t*>(address + 1);
	callee[0] = std::bit_cast<uint32_t>(func) - (address + 5);
}
