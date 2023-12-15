#pragma once
#include <Windows.h>
#include <iostream>

namespace Mebius::Debug {
    class Console {
    public:
        Console() {
            if (!AllocConsole()) {
                return;
            }
            freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
            freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
            freopen_s((FILE**)stderr, "CONIN$", "w", stdin);
        }
        ~Console() {
            FreeConsole();
        }
    };
}

const static Mebius::Debug::Console debug_console;