#pragma once

#include <iostream>
#include <memory>
#include <optional>

#ifdef MEBIUS_EXPORT
#define MEBIUSAPI __declspec(dllexport)
#else
#define MEBIUSAPI __declspec(dllimport)
#endif

namespace mebius::config {
	class MEBIUSAPI Config
	{
	public:
		Config(const char * path);
		~Config();

		std::optional<std::string_view> get_string_view(const char* key) noexcept;
		std::optional<std::string> get_string(const char* key) noexcept;
		std::optional<int64_t> get_int(const char* key) noexcept;
		std::optional<double> get_double(const char* key) noexcept;
		std::optional<bool> get_bool(const char* key) noexcept;

		bool is_loaded(void) noexcept;
	private:
		class ConfigImpl;
		ConfigImpl* _pImpl;
	};
}