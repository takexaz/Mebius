#include <Mebius.hpp>
#include <_Mebius.hpp>

namespace mebius::debug {
    HANDLE Logger::_hConsole = nullptr;
    char16_t Logger::_default_color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    std::ofstream Logger::_log;
}

namespace mebius::debug {
    Console* Console::_instance = nullptr;
}