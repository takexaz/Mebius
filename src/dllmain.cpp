
#include <Windows.h>
#include <Mebius.hpp>
#include <_Mebius.hpp>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: {
		// Config��ǂݍ���Ŋe��ݒ���s��
		mebius::config::CF_MEBIUS conf = mebius::config::get_config();
		if (!conf.Options.Enable) return TRUE;
		if (conf.Debug.Console.Enable) {
			mebius::debug::Console::get_instance(conf.Debug.Console.Log, conf.Debug.Console.Error);
		}

		// Mebius�N�����O��\��
		mebius::debug::Logger meblog(std::cout, FOREGROUND_CYAN);
		meblog << "Initializing Mebius." << std::endl;

		// MUGEN�̃o�[�W�������`�F�b�N
		mebius::debug::Logger mebwarn(std::cout, FOREGROUND_YELLOW);
		bool is_mugen = mebius::util::checksum();
		if (!is_mugen && conf.Options.BypassCheckSum) {
			mebwarn << "Bypass CheckSum!" << std::endl;
			return TRUE;
		}

		break;
	}
	case DLL_THREAD_ATTACH: {
		break;
	}
	case DLL_THREAD_DETACH: {
		break;
	}
	case DLL_PROCESS_DETACH: {
		break;
	}
	}
	return TRUE;
}