#pragma once

#include <inline_hook.hpp>

#include <unordered_map>


// クッション
extern "C" void hook_inline_cushion();
// ASM用に宣言
extern "C" inline const void hook_inline(const mebius::inline_hook::PMBCONTEXT context);

namespace mebius::inline_hook {
	using code_t = uint8_t;

	constexpr static inline code_t _OPCODE_EX_HALT = 0xF4;
	constexpr static inline code_t _OPCODE_REL_CALL = 0xE8;
	constexpr static inline code_t _OPCODE_REL_JMP = 0xE9;


	class InlineHookDataImpl : public InlineHookData {
	public:
		explicit InlineHookDataImpl(uint32_t address) noexcept;
		~InlineHookDataImpl() noexcept override;

		InlineHookDataImpl(const InlineHookDataImpl&) = delete;
		InlineHookDataImpl& operator=(const InlineHookDataImpl&) = delete;

		InlineHookDataImpl(InlineHookDataImpl&& inlineHookData) noexcept : _inline_hooks(inlineHookData._inline_hooks), _trampoline_code(inlineHookData._trampoline_code) {
			inlineHookData._trampoline_code = nullptr;
		}
		InlineHookDataImpl& operator=(InlineHookDataImpl&& inlineHookData) noexcept {
			this->_inline_hooks = std::move(inlineHookData._inline_hooks);
			this->_trampoline_code = std::move(inlineHookData._trampoline_code);
			inlineHookData._trampoline_code = nullptr;
		}
		inline const std::vector<const void*>& GetInlineHooks() const noexcept override {
			return _inline_hooks;
		}
		inline const code_t* GetTrampolineCode() const noexcept override {
			return _trampoline_code;
		}
		inline void AppendInlineHook(const void* address) noexcept {
			_inline_hooks.push_back(address);
		}
	private:
		std::vector<const void*> _inline_hooks;
		code_t* _trampoline_code;
	};

	static inline std::unordered_map<uint32_t, InlineHookDataImpl> _INLINE_HOOK_LIST{};
}