#pragma once

#include <hook.hpp>

#include <unordered_map>

namespace mebius {
	using code_t = uint8_t;

	constexpr static inline code_t _OPCODE_REL_CALL = 0xE8;
	constexpr static inline code_t _OPCODE_REL_JMP = 0xE9;

	class HookDataImpl : public HookData {
	public:
		explicit HookDataImpl(uint32_t address) noexcept;
		~HookDataImpl() noexcept override;

		HookDataImpl(const HookDataImpl&) = delete;
		HookDataImpl& operator=(const HookDataImpl&) = delete;

		HookDataImpl(HookDataImpl&& hookData) noexcept : _head_hooks(hookData._head_hooks), _tail_hooks(hookData._tail_hooks), _trampoline_code(hookData._trampoline_code) {
			hookData._trampoline_code = nullptr;
		}
		HookDataImpl& operator=(HookDataImpl&& hookData) noexcept {
			this->_head_hooks = std::move(hookData._head_hooks);
			this->_tail_hooks = std::move(hookData._tail_hooks);
			this->_trampoline_code = std::move(hookData._trampoline_code);
			hookData._trampoline_code = nullptr;
		}

		inline const std::vector<const void*>& GetHeadHooks() const noexcept override {
			return _head_hooks;
		}
		inline const std::vector<const void*>& GetTailHooks() const noexcept override {
			return _tail_hooks;
		}
		inline const code_t* GetTrampolineCode() const noexcept override {
			return _trampoline_code;
		}
		inline void AppendHeadHook(const void* func) noexcept {
			_head_hooks.push_back(func);
		}
		inline void AppendTailHook(const void* func) noexcept {
			_tail_hooks.push_back(func);
		}

	private:
		std::vector<const void*> _head_hooks;
		std::vector<const void*> _tail_hooks;
		code_t* _trampoline_code;
	};

	static inline std::unordered_map<uint32_t, HookDataImpl> _HOOK_LIST{};
}