
#include <Windows.h>
#include <Mebius.hpp>

#include <_debug.hpp>
#include <_utility.hpp>

#include <iostream>



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

		const static mebius::loader::Plugins mebi_ex("mods/", "mx");
		const static mebius::loader::Scripts mebi_sc("scripts/", "ms", true);
	}
}

struct CF_MEBIUS {
	struct CF_OPTIONS {
		bool Enable = false;
		bool BypassCheckSum = false;
	};
	CF_OPTIONS Options;

	struct CF_CONSOLE {
		bool Enable = false;
		bool Default = false;
		bool Error = false;
	};
	CF_CONSOLE Console;
};

const static char* conf_mebius_path = "mods\\mebius.toml";

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: {
		// Configを読み込んで各種設定を行う
		CF_MEBIUS conf{};
		mebius::config::Config mb_config(conf_mebius_path);
		mb_config.get_value_from_key("Options.Enable", conf.Options.Enable);
		mb_config.get_value_from_key("Options.BypassCheckSum", conf.Options.BypassCheckSum);
		mb_config.get_value_from_key("Console.Enable", conf.Console.Enable);
		mb_config.get_value_from_key("Console.Default", conf.Console.Default);
		mb_config.get_value_from_key("Console.Error", conf.Console.Error);

		// Mebiusが無効なら読み込み終了
		if (!conf.Options.Enable) return TRUE;

		// コンソールが有効なら表示
		if (conf.Console.Enable) {
			mebius::debug::Console::get_instance(conf.Console.Default, conf.Console.Error);
		}

		// Mebius起動ログを表示
		mebius::debug::Logger meblog(std::cout, FOREGROUND_CYAN);
		meblog << "Initializing Mebius." << std::endl;

		// MUGENのバージョンをチェック
		mebius::debug::Logger mebwarn(std::cout, FOREGROUND_YELLOW);
		uint32_t patch_addr = mebius::util::detect_mugen();
		if (patch_addr == 0xFFFFFFFF && conf.Options.BypassCheckSum) {
			mebwarn << "WARNING: Bypass CheckSum!" << std::endl;
			patch_addr = mebius::util::default_entry_point;
		}

		// プラグインロード用インラインフック(VEH)
		mebius::inline_hook::HookInline(patch_addr, patch::init_plugin);

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