#pragma once
#include <debug.hpp>
#include <Windows.h>
#include <iostream>

namespace Mebius::Debug {
    class Console {
    private:
        static Console* _instance;
        Console(bool is_cout, bool is_cerr) {
            if (!AllocConsole()) {
                return;
            }
            if (is_cout) freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
            if (is_cerr) freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
        }
        ~Console() {
            FreeConsole();
        }
    public:
        static Console* get_instance(bool is_cout, bool is_cerr) {
            if (!_instance) _instance = new Console(is_cout, is_cerr);
            return _instance;
        }
    };
}