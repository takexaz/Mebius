#include <_hook.hpp>
#include <_alloc.hpp>
#include <_reassemble.hpp>

#include <error.hpp>

#include <Zydis/Zydis.h>

#include <Windows.h>
#include <format>

using namespace mebius;
using namespace mebius::hook;

static inline code_t* make_trampoline_code(uint32_t address) noexcept;
static inline std::pair<bool, HookDataImpl&> add_hook_data(uint32_t address) noexcept;
static inline void write_call_opcode(uint32_t address, const void* func);
static inline void write_jmp_opcode(uint32_t address, const void* func) noexcept;

MEBIUSAPI const HookData& mebius::hook::_GetHookData(uint32_t address)
{
	decltype(_HOOK_LIST)::iterator it = _HOOK_LIST.find(address);
	if (it == _HOOK_LIST.end()) {
		throw MebiusError(std::vformat("Mebius has not hook on address 0x{:08X}.", std::make_format_args(address)));
	}
	else {
		return it->second;
	}
}

MEBIUSAPI const HookData* mebius::hook::_GetHookDataNullable(uint32_t address) noexcept
{
	decltype(_HOOK_LIST)::iterator it = _HOOK_LIST.find(address);
	if (it == _HOOK_LIST.end()) {
		return nullptr;
	}
	else {
		return &(it->second);
	}
}

MEBIUSAPI void mebius::hook::_SetHookOnHead(uint32_t hookTarget, const void* hookFunction, const void* internalHookFunction) noexcept
{
	auto [unhooked, hook] = add_hook_data(hookTarget);
	hook.AppendHeadHook(hookFunction);
	if (unhooked) {
		try {
			write_call_opcode(hookTarget, internalHookFunction);
		}
		catch (const MebiusError& e) {
			ShowErrorDialog(e.what());
		}
	}
}

MEBIUSAPI void mebius::hook::_SetHookOnTail(uint32_t hookTarget, const void* hookFunction, const void* internalHookFunction) noexcept
{
	auto [unhooked, hook] = add_hook_data(hookTarget);
	hook.AppendTailHook(hookFunction);
	if (unhooked) {
		try {
			write_call_opcode(hookTarget, internalHookFunction);
		}
		catch (const MebiusError& e) {
			ShowErrorDialog(e.what());
		}
	}
}

HookDataImpl::HookDataImpl(uint32_t address) noexcept {
	_trampoline_code = make_trampoline_code(address);
}

HookDataImpl::~HookDataImpl() noexcept {
	if (_trampoline_code != nullptr) {
		alloc::CodeAllocator::GetInstance().DeAllocate(_trampoline_code);
		_trampoline_code = nullptr;
	}
}

static inline code_t* make_trampoline_code(uint32_t address) noexcept {
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

static inline std::pair<bool, HookDataImpl&> add_hook_data(uint32_t address) noexcept
{
	decltype(_HOOK_LIST)::iterator it = _HOOK_LIST.find(address);
	if (it == _HOOK_LIST.end()) {
		decltype(_HOOK_LIST)::iterator item = std::get<0>(_HOOK_LIST.emplace(address, address));
		return { true, item->second };
	}
	else {
		return { false, it->second };
	}
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








	




