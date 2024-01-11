#include <error.hpp>
#include <debug.hpp>
#include <Windows.h>
#include <format>

[[noreturn]] void mebius::ShowErrorDialog(const char* message) {
	PLOGE << message;
	MessageBoxA(NULL, message, "Error by Mebius", MB_OK | MB_ICONERROR);
	exit(1);
}
