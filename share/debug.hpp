#pragma once
#include <Windows.h>
#include <iostream>
#include <chrono>
#include <wincon.h>

#ifdef MEBIUS_EXPORT
#define MEBIUSAPI __declspec(dllexport)
#else
#define MEBIUSAPI __declspec(dllimport)
#endif

namespace Mebius::Debug {
    class Console;
    class MEBIUSAPI Logger {
    private:
        static char16_t _default_color;
        std::ostream& _stream;
        char16_t _color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    public:
        Logger(std::ostream& _stream) : _stream(_stream) {
            set_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }

        Logger(std::ostream& _stream, char16_t color) : _stream(_stream) {
            set_color(color);
        }

        void set_color(char16_t color) {
            this->_color = color;
        }

        static void set_default_color(char16_t color) {
            _default_color = color;
        }

        void print_time() {
            std::time_t t = std::time(nullptr);
            this->_stream << "[" << std::put_time(std::localtime(&t), "%F %T") << "] ";
        }

        template <typename T>
        std::ostream& operator<<(const T& thing) {
            HANDLE hConsole = Mebius::Debug::Console::get_console_handle();
            if (hConsole) SetConsoleTextAttribute(hConsole, this->_color);
            this->_stream << thing;
            if (hConsole) SetConsoleTextAttribute(hConsole, _default_color);
            return this->_stream;
        }
    };
    char16_t Logger::_default_color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
}

