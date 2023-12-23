
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
	void patch_WM_private_beta(PCONTEXT context) {
		mebius::debug::Logger meblog(std::cout, FOREGROUND_YELLOW);
		meblog << "PATCHED!!" << std::endl;
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: {
		// Configを読み込んで各種設定を行う
		mebius::config::CF_MEBIUS conf = mebius::config::get_config();
		if (!conf.Options.Enable) return TRUE;
		if (conf.Debug.Console.Enable) {
			mebius::debug::Console::get_instance(conf.Debug.Console.Default, conf.Debug.Console.Error);
		}

		// Mebius起動ログを表示
		mebius::debug::Logger meblog(std::cout, FOREGROUND_CYAN);
		meblog << "Initializing Mebius." << std::endl;

		// MUGENのバージョンをチェック
		mebius::debug::Logger mebwarn(std::cout, FOREGROUND_YELLOW);
		bool is_mugen = mebius::util::checksum();
		if (!is_mugen && conf.Options.BypassCheckSum) {
			mebwarn << "WARNING: Bypass CheckSum!" << std::endl;
			return TRUE;
		}

		{
			using namespace test_void;
			mebius::hook::HookOnHead(hook, head);
			mebius::hook::HookOnTail(hook, tail);
			hook();
		}

		{
			using namespace test_str;
			mebius::hook::HookOnHead(hook, head);
			mebius::hook::HookOnTail(hook, tail);
			hook(0xFFFFFFFF);
		}

		// 004C37A0
		mebius::inline_hook::HookInlineVEH(0x4C38FC, patch::patch_WM_private_beta);

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