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

#define FOREGROUND_PINK FOREGROUND_RED | FOREGROUND_INTENSITY
#define FOREGROUND_LIME FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define FOREGROUND_CYAN FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define FOREGROUND_YELLOW FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define FOREGROUND_WHITE FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE

namespace mebius::debug {
    class MEBIUSAPI Logger {
    private:
        static HANDLE _hConsole;
        static char16_t _default_color;
        std::ostream& _stream;
        char16_t _color = FOREGROUND_WHITE;

        static HANDLE get_console_handle(void) {
            if (!_hConsole) _hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            return _hConsole;
        }
    public:
        Logger(std::ostream& _stream) : _stream(_stream) {
            set_color(FOREGROUND_WHITE);
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

        static std::string print_time() {
            std::time_t t = std::time(nullptr);
            std::string test = "[";
            test += static_cast<std::ostringstream&&>(std::ostringstream() << std::put_time(std::localtime(&t), "%F %T")).str();
            test += "] ";
            return test;
        }

        template <typename T>
        std::ostream& operator<<(const T& thing) {
            HANDLE hConsole = get_console_handle();
            if (!hConsole) {
                return this->_stream << thing;
            }
            else {
                SetConsoleTextAttribute(hConsole, this->_color);
                this->_stream << thing;
                SetConsoleTextAttribute(hConsole, _default_color);
                return this->_stream;
            }
        }
    };
}

