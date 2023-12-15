
#include <Windows.h>
#include <Mebius.hpp>
#include <_Mebius.hpp>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        try
        {
            Mebius::Config::conf_mebius = toml::parse_file(Mebius::Config::conf_mebius_path);
            Mebius::Debug::Console::get_instance(true, true);
            Mebius::Debug::conlog << "Mebius Init" << std::endl;
        }
        catch (const toml::parse_error& err)
        {
            std::cerr << "Parsing failed:\n" << err << "\n";
            return 1;
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
