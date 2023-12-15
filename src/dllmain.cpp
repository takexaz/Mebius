#include <Mebius.hpp>
#include <_Mebius.hpp>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        try
        {
            conf_mebius = toml::parse_file(conf_mebius_path);
            std::cout << conf_mebius << "\n";
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
