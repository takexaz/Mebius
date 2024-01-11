#pragma once
#include <debug.hpp>
#include <Windows.h>
#include <iostream>

namespace mebius::debug {
    class Console {
    private:
        static Console* _instance;
        Console() {
            if (!AllocConsole()) {
                return;
            }
            freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
            freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
        }
        ~Console() {
            FreeConsole();
        }
    public:
        static Console* get_instance(void) {
            if (!_instance) _instance = new Console();
            return _instance;
        }
    };
}