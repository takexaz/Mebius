#pragma once
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
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
        static std::ofstream _log;
        char16_t _color = FOREGROUND_WHITE;

        static HANDLE get_console_handle(void) {
            if (!_hConsole) _hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            return _hConsole;
        }

    public:
        Logger(std::ostream& _stream, char16_t color) : _stream(_stream) {
            if (!_log.is_open()) {
                std::ofstream ofs("mebius.log");
                ofs << "\n\n";
                ofs << "--------------------------------\n";
                ofs << "MEBIUS ver 2023.12.24 status log\n";
                ofs << "--------------------------------\n";
                ofs.flush();
                ofs.close();
                _log.open("mebius.log", std::ios::app);
            }
            set_color(color);
        }

        void set_color(char16_t color) {
            this->_color = color;
        }

        static void set_default_color(char16_t color) {
            _default_color = color;
        }

        template <typename T>
        std::ostream& operator<<(const T& thing) {
            // ファイル出力
            {
                std::stringstream buf;
                buf << thing;
                _log << buf.str() << '\n';
                _log.flush();
            }

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

