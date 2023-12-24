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


struct REGISTER {
    ULONG SegGs;
    ULONG SegFs;
    ULONG SegEs;
    ULONG SegDs;
    ULONG Edi;
    ULONG Esi;
    ULONG Ebx;
    ULONG Edx;
    ULONG Ecx;
    ULONG Eax;
    ULONG Ebp;
    ULONG Eip;
    ULONG SegCs;
    ULONG EFlags;
    ULONG Esp;
    ULONG SegSs;
};


MBCONTEXT  STRUCT
    STRUCT  X86
        Edi_    DWORD   ?
        Esi_    DWORD   ?
        Ebx_    DWORD   ?
        Edx_    DWORD   ?
        Ecx_    DWORD   ?
        Eax_    DWORD   ?
        Ebp_    DWORD   ?
        Eip_    DWORD   ?
        Esp_    DWORD   ?
    ENDS
    STRUCT  X87
        St0     QWORD   ?
        St1     QWORD   ?
        St2     QWORD   ?
        St3     QWORD   ?
        St4     QWORD   ?
        St5     QWORD   ?
        St6     QWORD   ?
        St7     QWORD   ?
    ENDS
    STRUCT  SEGMENTS
        GsSeg   WORD    ?
        FsSeg   WORD    ?
        EsSeg   WORD    ?
        DsSeg   WORD    ?
        CsSeg   WORD    ?
        SsSeg   WORD    ?
    ENDS
    EFlags  DWORD   ?
MBCONTEXT  ENDS