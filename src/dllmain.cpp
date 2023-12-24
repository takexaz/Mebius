
#include <Windows.h>
#include <Mebius.hpp>

#include <_config.hpp>
#include <_debug.hpp>
#include <_utility.hpp>

#include <iostream>

namespace test_void {
	void hook(void) {
		mebius::debug::Logger meblog(std::cout, FOREGROUND_YELLOW);
		meblog << "Function" << std::endl;
		return;
	}

	void head(void) {
		mebius::debug::Logger meblog(std::cout, FOREGROUND_YELLOW);
		meblog << "Function_Head" << std::endl;
	}

	void tail(void) {
		mebius::debug::Logger meblog(std::cout, FOREGROUND_YELLOW);
		meblog << "Function_Tail" << std::endl;
	}
}

namespace test_str {
	void hook(int num) {
		mebius::debug::Logger meblog(std::cout, FOREGROUND_YELLOW);
		meblog << std::format("Function({})", num) << std::endl;
		return;
	}

	void head(int num) {
		mebius::debug::Logger meblog(std::cout, FOREGROUND_YELLOW);
		meblog << std::format("Function_head({})", num) << std::endl;
	}

	void tail(int num) {
		mebius::debug::Logger meblog(std::cout, FOREGROUND_YELLOW);
		meblog << std::format("Function_tail({})", num) << std::endl;
	}
}

namespace patch {
	void change_version(mebius::inline_hook::PMBCONTEXT context) {
		static uint32_t frame = 0;
		++frame;
		const static char* mb_version = "MEBIUS %i.%02i.%02i";
		if (frame % 800 < 400) {
			void** stack = (void**)context->Esp;
			*(stack + 1) = (void*)2023;
			*(stack + 2) = (void*)12;
			*(stack + 3) = (void*)24;
			*stack = (void*)mb_version;
		}
	}

	void init_plugin(mebius::inline_hook::PMBCONTEXT context) {
		mebius::inline_hook::HookInline(0x00430163, patch::change_version);

		mebius::debug::Logger meblog(std::cout, FOREGROUND_YELLOW);
		meblog << "PATCHED!!" << std::endl;
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: {
		// Config��ǂݍ���Ŋe��ݒ���s��
		mebius::config::CF_MEBIUS conf = mebius::config::get_config();
		if (!conf.Options.Enable) return TRUE;
		if (conf.Debug.Console.Enable) {
			mebius::debug::Console::get_instance(conf.Debug.Console.Default, conf.Debug.Console.Error);
		}

		// Mebius�N�����O��\��
		mebius::debug::Logger meblog(std::cout, FOREGROUND_CYAN);
		meblog << "Initializing Mebius." << std::endl;

		// MUGEN�̃o�[�W�������`�F�b�N
		mebius::debug::Logger mebwarn(std::cout, FOREGROUND_YELLOW);
		uint32_t patch_addr = mebius::util::detect_mugen();
		if (patch_addr == 0xFFFFFFFF && conf.Options.BypassCheckSum) {
			mebwarn << "WARNING: Bypass CheckSum!" << std::endl;
			patch_addr = mebius::util::default_entry_point;
		}

		// �v���O�C�����[�h�p�C�����C���t�b�N(VEH)
		mebius::inline_hook::HookInline(patch_addr, patch::init_plugin, true);

		break;
	}
	case DLL_THREAD_ATTACH: {
		break;
	}
	case DLL_THREAD_DETACH: {
		break;
	}
	case DLL_PROCESS_DETACH: {
		mebius::debug::Logger meblog(std::cout, FOREGROUND_CYAN);
		meblog << "Denitializing Mebius." << std::endl;
		break;
	}
	}
	return TRUE;
}