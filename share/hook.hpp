#pragma once

#include <Error.hpp>

#include <bit>
#include <optional>
#include <vector>

#ifdef MEBIUS_EXPORT
#define MEBIUSAPI __declspec(dllexport)
#else
#define MEBIUSAPI __declspec(dllimport)
#endif

namespace mebius {
	using code_t = uint8_t;

	class HookData {
	public:
		virtual ~HookData() = default;
		virtual const std::vector<const void*>& GetHeadHooks() const noexcept = 0;
		virtual const std::vector<const void*>& GetTailHooks() const noexcept = 0;
		virtual const code_t* GetTrampolineCode() const noexcept = 0;
	};

	MEBIUSAPI const HookData& _GetHookData(uint32_t address);
	MEBIUSAPI const HookData* _GetHookDataNullable(uint32_t address) noexcept;
	MEBIUSAPI void _SetHookOnHead(uint32_t hookTarget, const void* hookFunction, const void* internalHookFunction) noexcept;
	MEBIUSAPI void _SetHookOnTail(uint32_t hookTarget, const void* hookFunction, const void* internalHookFunction) noexcept;

	namespace internal {
		using pvfv_t = void(*)(void);
		template <typename T>
		using ptfv_t = T(*)(void);
		template <typename... Args>
		using pvfa_t = void(*)(Args...);
		template <typename T, typename... Args>
		using ptfa_t = T(*)(Args...);
		template <typename T>
		using ptft_t = T(*)(T);
		template <typename T, typename... Args>
		using ptfta_t = T(*)(T, Args...);


		static void hook_vfv(pvfv_t returnAddress) {
			try {
				uint32_t hookedFunction = 0;
				__asm {
					PUSH DWORD PTR[EBP + 0x04]
					POP DWORD PTR hookedFunction
					SUB DWORD PTR hookedFunction, 0x05
				}
				auto& hook = _GetHookData(hookedFunction);

				for (auto&& f : hook.GetHeadHooks()) {
					auto head = std::bit_cast<pvfv_t>(f);
					head();
				}

				auto trampoline = std::bit_cast<pvfv_t>(hook.GetTrampolineCode());
				trampoline();

				for (auto&& f : hook.GetTailHooks()) {
					auto tail = std::bit_cast<pvfv_t>(f);
					tail();
				}
			}
			catch (const MebiusError& e) {
				ShowErrorDialog(e.what());
			}

			__asm {
				mov ecx, returnAddress
				leave
				mov dword ptr[esp], ecx
				ret 4
			};
		}

		template <typename T>
		void hook_tfv(ptfv_t<T> returnAddress) {
			{
				uint32_t hookedFunction = 0;
				T result;
				try {
					__asm {
						PUSH DWORD PTR[EBP + 0x04]
						POP DWORD PTR hookedFunction
						SUB DWORD PTR hookedFunction, 0x05
					}
					auto& hook = _GetHookData(hookedFunction);

					for (auto&& f : hook.GetHeadHooks()) {
						auto head = std::bit_cast<pvfv_t>(f);
						head();
					}

					auto trampoline = std::bit_cast<ptfv_t<T>>(hook.GetTrampolineCode());
					result = trampoline();

					for (auto&& f : hook.GetTailHooks()) {
						auto tail = std::bit_cast<ptft_t<T>>(f);
						result = tail(result);
					}
				}
				catch (const MebiusError& e) {
					ShowErrorDialog(e.what());
				}

				__asm {
					mov eax, returnAddress
					leave
					mov dword ptr[esp], eax
					mov eax, result
					ret 4
				};
			}
		}

		template <typename... Args>
		void hook_vfa(pvfa_t<Args...> returnAddress, Args... args) {
			try {
				uint32_t hookedFunction = 0;
				__asm {
					PUSH DWORD PTR[EBP + 0x04]
					POP DWORD PTR hookedFunction
					SUB DWORD PTR hookedFunction, 0x05
				}
				auto& hook = _GetHookData(hookedFunction);

				for (auto&& f : hook.GetHeadHooks()) {
					auto head = std::bit_cast<pvfa_t<Args...>>(f);
					head(args...);
				}

				auto trampoline = std::bit_cast<pvfa_t<Args...>>(hook.GetTrampolineCode());
				trampoline(args...);

				for (auto&& f : hook.GetTailHooks()) {
					auto tail = std::bit_cast<pvfa_t<Args...>>(f);
					tail(args...);
				}
			}
			catch (const MebiusError& e) {
				ShowErrorDialog(e.what());
			}

			__asm {
				mov ecx, returnAddress
				leave
				mov dword ptr[esp], ecx
				ret 4
			};
		}

		template <typename T, typename... Args>
		void hook_tfa(ptfa_t<T, Args...> returnAddress, Args... args) {
			T result;
			try {
				uint32_t hookedFunction = 0;
				__asm {
					PUSH DWORD PTR[EBP + 0x04]
					POP DWORD PTR hookedFunction
					SUB DWORD PTR hookedFunction, 0x05
				}
				auto& hook = _GetHookData(hookedFunction);

				for (auto&& f : hook.GetHeadHooks()) {
					auto head = std::bit_cast<pvfa_t<Args...>>(f);
					head(args...);
				}

				auto trampoline = std::bit_cast<ptfa_t<T, Args...>>(hook.GetTrampolineCode());
				T result = trampoline(args...);

				for (auto&& f : hook.GetTailHooks()) {
					auto tail = std::bit_cast<ptfta_t<T, Args...>>(f);
					result = tail(result, args...);
				}
			}
			catch (const MebiusError& e) {
				ShowErrorDialog(e.what());
			}

			__asm {
				mov eax, returnAddress
				leave
				mov dword ptr[esp], eax
				mov eax, result
				ret 4
			};
		}
	}

	static void HookOnHead(internal::pvfv_t caller, const internal::pvfv_t callee) noexcept {
		_SetHookOnHead(std::bit_cast<uint32_t>(caller), std::bit_cast<const void*>(callee), std::bit_cast<const void*>(&internal::hook_vfv));
	}

	template <typename T>
	void HookOnHead(internal::ptfv_t<T> caller, const internal::pvfv_t callee) noexcept {
		_SetHookOnHead(std::bit_cast<uint32_t>(caller), std::bit_cast<const void*>(callee), std::bit_cast<const void*>(&internal::hook_tfv<T>));
	}

	template <typename... Args>
	void HookOnHead(internal::pvfa_t<Args...> caller, const internal::pvfa_t<Args...> callee) noexcept {
		_SetHookOnHead(std::bit_cast<uint32_t>(caller), std::bit_cast<const void*>(callee), std::bit_cast<const void*>(&internal::hook_vfa<Args...>));
	}

	template <typename T, typename... Args>
	void HookOnHead(internal::ptfa_t<T, Args...> caller, const internal::pvfa_t<Args...> callee) noexcept {
		_SetHookOnHead(std::bit_cast<uint32_t>(caller), std::bit_cast<const void*>(callee), std::bit_cast<const void*>(&internal::hook_tfa<T, Args...>));
	}

	inline void HookOnTail(internal::pvfv_t caller, const internal::pvfv_t callee) noexcept {
		_SetHookOnTail(std::bit_cast<uint32_t>(caller), std::bit_cast<const void*>(callee), std::bit_cast<const void*>(&internal::hook_vfv));
	}

	template <typename T>
	void HookOnTail(internal::ptfv_t<T> caller, const internal::ptft_t<T> callee) noexcept {
		_SetHookOnTail(std::bit_cast<uint32_t>(caller), std::bit_cast<const void*>(callee), std::bit_cast<const void*>(&internal::hook_tfv<T>));
	}

	template <typename... Args>
	void HookOnTail(internal::pvfa_t<Args...> caller, const internal::pvfa_t<Args...> callee) noexcept {
		_SetHookOnTail(std::bit_cast<uint32_t>(caller), std::bit_cast<const void*>(callee), std::bit_cast<const void*>(&internal::hook_vfa<Args...>));
	}

	template <typename T, typename... Args>
	void HookOnTail(internal::ptfa_t<T, Args...> caller, const internal::ptfta_t<T, Args...> callee) noexcept {
		_SetHookOnTail(std::bit_cast<uint32_t>(caller), std::bit_cast<const void*>(callee), std::bit_cast<const void*>(&internal::hook_tfa<T, Args...>));
	}

}




















