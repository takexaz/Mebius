
#include <Windows.h>
#include <Mebius.hpp>
#include <_Mebius.hpp>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH: {
        // Config��ǂݍ���Ŋe��ݒ���s��
        Mebius::Config::CF_MEBIUS conf = Mebius::Config::get_config();
        if (!conf.Options.Enable) return TRUE;
        if (conf.Debug.Console.Enable) {
            Mebius::Debug::Console::get_instance(conf.Debug.Console.Log, conf.Debug.Console.Error);
        }

        // Mebius�N�����O��\��
        Mebius::Debug::Logger meblog(std::cout, FOREGROUND_CYAN);
        meblog << "Initializing Mebius." << std::endl;

        // MUGEN�̃o�[�W�������`�F�b�N
        Mebius::Utility::check_mugen();
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
