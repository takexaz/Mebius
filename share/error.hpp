#pragma once

#include <stdexcept>
#include <string>

#ifdef MEBIUS_EXPORT
#define MEBIUSAPI __declspec(dllexport)
#else
#define MEBIUSAPI __declspec(dllimport)
#endif

namespace mebius {
	class MebiusError : public std::runtime_error {
	public:
		explicit MebiusError(const std::string& message) : runtime_error(message) {};
		explicit MebiusError(const char* message) : runtime_error(message) {};
	};

	extern "C" MEBIUSAPI void ShowErrorDialog(const char* message);
}
