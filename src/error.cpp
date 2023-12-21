#include <error.hpp>
#include <Windows.h>
#include <format>

[[noreturn]] void mebius::ShowErrorDialog(const char* message) {
	//d WriteLog(std::vformat("ERROR: {}", std::make_format_args(message)));
	MessageBoxA(NULL, message, "Error by Mebius", MB_OK | MB_ICONERROR);
	exit(1);
}
