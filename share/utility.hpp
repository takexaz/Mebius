#pragma once

#include <error.hpp>
#include <Windows.h>
#include <atlstr.h>

#ifdef MEBIUS_EXPORT
#define MEBIUSAPI __declspec(dllexport)
#else
#define MEBIUSAPI __declspec(dllimport)
#endif

namespace mebius::util {
    inline HMODULE GetCurrentModule(void)
    {
        HMODULE hModule = NULL;
        // hModule is NULL if GetModuleHandleEx fails.
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
            | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCTSTR)GetCurrentModule, &hModule);
        return hModule;
    }
    MEBIUSAPI std::string get_module_directory(HMODULE hModule);
    MEBIUSAPI std::string get_module_filename(HMODULE hModule);
    MEBIUSAPI std::string get_module_path(HMODULE hModule);
}