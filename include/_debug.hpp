#pragma once
#include <Windows.h>
#include <iostream>

namespace Mebius::Debug {
    class Console {
    private:
        static Console* _instance;
        static HANDLE _hConsole;
        Console(bool is_cout, bool is_cerr) {
            if (!AllocConsole()) {
                return;
            }
            if (is_cout) freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
            if (is_cerr) freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
            if (is_cout || is_cerr) this->_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        }
        ~Console() {
            FreeConsole();
        }
    public:
        static Console* get_instance(bool is_cout, bool is_cerr) {
            if (!_instance) _instance = new Console(is_cout, is_cerr);
            return _instance;
        }

        static HANDLE get_console_handle() {
            return _hConsole;
        }
    };
    Console* Console::_instance = nullptr;
    HANDLE Console::_hConsole = nullptr;

    static Logger conlog(std::cout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
}