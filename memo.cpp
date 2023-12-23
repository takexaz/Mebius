class Singleton {
private:
    static Singleton* _instance;
    Singleton(bool is_cout, bool is_cerr) {
        return;
    }
    ~Singleton() {
        return;
    }
public:
    static Singleton* get_instance() {
        if (!_instance) _instance = new Singleton();
        return _instance;
    }
};

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