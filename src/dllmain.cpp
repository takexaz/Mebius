
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

		mebius::loader::Plugins::create("mods/", "mx", true);
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
		int64_t Level = plog::none;
		int64_t Detail = 0;
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
		mb_config.get_value_from_key("Console.Level", conf.Console.Level);
		mb_config.get_value_from_key("Console.Detail", conf.Console.Detail);

		// Mebiusが無効なら読み込み終了
		if (!conf.Options.Enable) return TRUE;

		// コンソールが有効なら表示
		if (conf.Console.Enable) {
			mebius::debug::Console::get_instance();
		}


		switch (conf.Console.Detail)
		{
		case 0: {
			static plog::ColorConsoleAppender<plog::MessageOnlyFormatter> consoleAppender;
			static plog::RollingFileAppender<plog::MessageOnlyFormatter> fileAppender("mebius.log");
			plog::init(plog::Severity(conf.Console.Level), &consoleAppender).addAppender(&fileAppender);
			break;
		}
		case 1: {
			static plog::ColorConsoleAppender<plog::FuncMessageFormatter> consoleAppender;
			static plog::RollingFileAppender<plog::FuncMessageFormatter> fileAppender("mebius.log");
			plog::init(plog::Severity(conf.Console.Level), &consoleAppender).addAppender(&fileAppender);
			break;
		}
		default: {
			static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
			static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("mebius.log");
			plog::init(plog::Severity(conf.Console.Level), &consoleAppender).addAppender(&fileAppender);
			break;
		}
		}

		// Mebius起動ログを表示
		PLOGD << "Initializing Mebius.";

		// MUGENのバージョンをチェック
		uint32_t patch_addr = mebius::util::detect_mugen();
		if (patch_addr == 0xFFFFFFFF && conf.Options.BypassCheckSum) {
			PLOGW << "Bypass CheckSum!";
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
		PLOGD << "Denitializing Mebius." << std::endl;
		break;
	}
	}
	return TRUE;
}