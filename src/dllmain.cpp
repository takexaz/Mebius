
#include <Windows.h>
#include <Mebius.hpp>

#include <_debug.hpp>
#include <_utility.hpp>

#include <iostream>
#include <format>

static auto ModeSelect = reinterpret_cast<void (*)(void)>(0x42f0c0);

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

const static uint16_t VERSION_X = 0;
const static uint16_t VERSION_Y = 0;
const static uint16_t VERSION_Z = 1;

namespace patch {
	void change_version(mebius::inline_hook::PMBCONTEXT context) {
		static uint32_t frame = 0;
		++frame;
		const static char* mb_version = "MEBIUS v%i.%i.%i";
		if (frame % 800 < 400) {
			void** stack = (void**)context->Esp;
			*(stack + 1) = (void*)VERSION_X;
			*(stack + 2) = (void*)VERSION_Y;
			*(stack + 3) = (void*)VERSION_Z;
			*stack = (void*)mb_version;
		}
	}

	void init_plugin(mebius::inline_hook::PMBCONTEXT context) {
		mebius::inline_hook::HookInline(ModeSelect, 0x10A3, patch::change_version);

		if (conf.Options.AutoUpdate == true) {
			auto U = mebius::updater::Updater("takexaz/Mebius", "Mebius.dll", std::format("{}.{}.{}", VERSION_X, VERSION_Y, VERSION_Z));
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
		// Config��ǂݍ���Ŋe��ݒ���s��
		mebius::config::Config mb_config(conf_mebius_path);
		mb_config.get_value_from_key("Options.Enable", conf.Options.Enable);
		mb_config.get_value_from_key("Options.BypassCheckSum", conf.Options.BypassCheckSum);
		mb_config.get_value_from_key("Options.AutoUpdate", conf.Options.AutoUpdate);
		mb_config.get_value_from_key("Console.Enable", conf.Console.Enable);
		mb_config.get_value_from_key("Console.Level", conf.Console.Level);
		mb_config.get_value_from_key("Console.Detail", conf.Console.Detail);

		// Mebius�������Ȃ�ǂݍ��ݏI��
		if (!conf.Options.Enable) return TRUE;

		// �R���\�[�����L���Ȃ�\��
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

		// Mebius�N�����O��\��
		PLOGD << "Initializing Mebius.";


		// MUGEN�̃o�[�W�������`�F�b�N
		uint32_t patch_addr = mebius::util::detect_mugen();
		if (patch_addr == 0xFFFFFFFF && conf.Options.BypassCheckSum) {
			PLOGW << "Bypass CheckSum!";
			patch_addr = mebius::util::default_entry_point;
		}

		// �v���O�C�����[�h�p�C�����C���t�b�N(VEH)
		mebius::inline_hook::HookInline((void*)patch_addr, 0, patch::init_plugin);

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