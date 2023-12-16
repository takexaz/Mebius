#include <Mebius.hpp>
#include <_Mebius.hpp>

namespace Mebius::Debug {
    HANDLE Logger::_hConsole = nullptr;
    char16_t Logger::_default_color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
}

namespace Mebius::Debug {
    Console* Console::_instance = nullptr;
}