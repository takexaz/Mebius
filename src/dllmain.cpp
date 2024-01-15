
#include <Windows.h>
#include <Mebius.hpp>

#include <_debug.hpp>
#include <_utility.hpp>
#include <_version.hpp>

#include <iostream>
#include <format>


static auto ModeSelect = reinterpret_cast<void (*)(void)>(0x42f0c0);
static auto __except_handler3 = reinterpret_cast<int (*)(PEXCEPTION_RECORD exception_record, void* registration, PCONTEXT context, void* dispatcher)>(0x496150);

struct CF_MEBIUS {
	struct CF_OPTIONS {
		bool Enable = false;
		bool BypassCheckSum = false;
		bool AutoUpdate = false;
	};
	CF_OPTIONS Options;

	struct CF_CONSOLE {
		bool Enable = false;
		int64_t Level = plog::none;
		int64_t Detail = 0;
	};
	CF_CONSOLE Console;
};
static CF_MEBIUS conf{};
const static char* conf_mebius_path = "mods\\mebius.toml";

namespace patch {
	void change_version(mebius::inline_hook::PMBCONTEXT context) {
		static uint32_t frame = 0;
		++frame;
		const static char* mb_version = "MEBIUS v%i.%i.%i";
		if (frame % 800 < 400) {
			void** stack = (void**)context->Esp;
			*(stack + 1) = (void*)PROJECT_VERSION_MAJOR;
			*(stack + 2) = (void*)PROJECT_VERSION_MINOR;
			*(stack + 3) = (void*)PROJECT_VERSION_PATCH;
			*stack = (void*)mb_version;
		}
	}
	
	void exception_logger(PEXCEPTION_RECORD exception_record, void* registration, PCONTEXT context, void* dispatcher) {
		PLOGE << "------ Exception Handled ------";
		PLOGE << "Exception Code: 0x" << std::setfill('0') << std::setw(8) << std::hex << exception_record->ExceptionCode;
		PLOGE << "Exception Address: 0x" << std::setfill('0') << std::setw(8) << std::hex << exception_record->ExceptionAddress;
		PLOGE << "Eax: 0x" << std::setfill('0') << std::setw(8) << std::hex << context->Eax << "\tEcx: 0x" << std::setfill('0') << std::setw(8) << std::hex << context->Ecx;
		PLOGE << "Edx: 0x" << std::setfill('0') << std::setw(8) << std::hex << context->Edx << "\tEbx: 0x" << std::setfill('0') << std::setw(8) << std::hex << context->Ebx;
		PLOGE << "Esp: 0x" << std::setfill('0') << std::setw(8) << std::hex << context->Esp << "\tEbp: 0x" << std::setfill('0') << std::setw(8) << std::hex << context->Ebp;
		PLOGE << "Esi: 0x" << std::setfill('0') << std::setw(8) << std::hex << context->Esi << "\tEdi: 0x" << std::setfill('0') << std::setw(8) << std::hex << context->Edi;
		PLOGE << "EFlags: 0x" << std::setfill('0') << std::setw(8) << std::hex << context->EFlags;
		PLOGE << "ZF:" << ((context->EFlags & mebius::inline_hook::MBEFlags::ZF) != 0) << "\tPF:" << ((context->EFlags & mebius::inline_hook::MBEFlags::PF) != 0) << "\tAF:" << ((context->EFlags & mebius::inline_hook::MBEFlags::AF) != 0);
		PLOGE << "OF:" << ((context->EFlags & mebius::inline_hook::MBEFlags::OF) != 0) << "\tSF:" << ((context->EFlags & mebius::inline_hook::MBEFlags::SF) != 0) << "\tDF:" << ((context->EFlags & mebius::inline_hook::MBEFlags::DF) != 0);
		PLOGE << "CF:" << ((context->EFlags & mebius::inline_hook::MBEFlags::CF) != 0) << "\tTF:" << ((context->EFlags & mebius::inline_hook::MBEFlags::TF) != 0) << "\tIF:" << ((context->EFlags & mebius::inline_hook::MBEFlags::IF) != 0);
		PLOGE << "-------------------------------";

		mebius::ShowErrorDialog(std::format("Exception occurred!", uint32_t(exception_record->ExceptionAddress)).c_str());
	}

	void init(mebius::inline_hook::PMBCONTEXT context) {
		mebius::inline_hook::HookInline(ModeSelect, 0x10A3, patch::change_version);

		mebius::hook::HookOnHead(__except_handler3, exception_logger);

		if (conf.Options.AutoUpdate == true) {
			auto U = mebius::updater::Updater("takexaz/Mebius", GetCurrentModule(), std::format("{}.{}.{}", PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH));
			U.update();
		}

		mebius::loader::Plugins::create("mods/", "mx", true);
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: {
		// Configを読み込んで各種設定を行う
		mebius::config::Config mb_config(conf_mebius_path);
		if (!mb_config.is_loaded()) {
			mebius::ShowErrorDialog(std::format("Could not found {}", conf_mebius_path).c_str());
		}
		conf.Options.Enable = mb_config.get_bool("Options.Enable").value_or(false);
		conf.Options.BypassCheckSum = mb_config.get_bool("Options.BypassCheckSum").value_or(false);
		conf.Options.AutoUpdate = mb_config.get_bool("Options.AutoUpdate").value_or(false);
		conf.Console.Enable = mb_config.get_bool("Console.Enable").value_or(false);
		conf.Console.Level = mb_config.get_int("Console.Level").value_or(6);
		conf.Console.Detail = mb_config.get_int("Console.Detail").value_or(0);

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
			plog::init<0>(plog::Severity(conf.Console.Level), &consoleAppender).addAppender(&fileAppender);
			break;
		}
		case 1: {
			static plog::ColorConsoleAppender<plog::FuncMessageFormatter> consoleAppender;
			static plog::RollingFileAppender<plog::FuncMessageFormatter> fileAppender("mebius.log");
			plog::init<0>(plog::Severity(conf.Console.Level), &consoleAppender).addAppender(&fileAppender);
			break;
		}
		default: {
			static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
			static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("mebius.log");
			plog::init<0>(plog::Severity(conf.Console.Level), &consoleAppender).addAppender(&fileAppender);
			break;
		}
		}

		// MUGENのバージョンをチェック
		uint32_t patch_addr = mebius::util::detect_mugen();
		if (patch_addr == 0xFFFFFFFF && conf.Options.BypassCheckSum) {
			PLOGW << "Bypass CheckSum!";
			patch_addr = mebius::util::default_entry_point;
		}
		else {
			PLOGE << "Mebius initialization failed.";
			return TRUE;
		}

		// Mebius起動ログを表示
		PLOGD << "Initializing Mebius.";

		// プラグインロード用インラインフック(VEH)
		mebius::inline_hook::HookInline((void*)patch_addr, 0, patch::init);

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