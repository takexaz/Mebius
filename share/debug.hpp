#pragma once
#include <iostream>

#ifdef MEBIUS_EXPORT
#define MEBIUSAPI __declspec(dllexport)
#else
#define MEBIUSAPI __declspec(dllimport)
#endif

namespace Mebius::Debug {
    class Console {
    public:
        Console() {
            AllocConsole();
            freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        }
        ~Console() {
            FreeConsole();
        }
    };
}
