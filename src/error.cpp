#include <error.hpp>
#include <debug.hpp>
#include <Windows.h>
#include <format>

[[noreturn]] void mebius::ShowErrorDialog(const char* message) {
	mebius::debug::Logger err(std::cerr, FOREGROUND_PINK);
	err << std::format("ERROR: {}", message);
	MessageBoxA(NULL, message, "Error by Mebius", MB_OK | MB_ICONERROR);
	exit(1);
}
