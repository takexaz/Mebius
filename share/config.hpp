#pragma once

#include <iostream>
#include <memory>

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

		bool get_value_from_key(const char* key, std::string& ptr) noexcept;
		bool get_value_from_key(const char* key, int64_t& ptr) noexcept;
		bool get_value_from_key(const char* key, double& ptr) noexcept;
		bool get_value_from_key(const char* key, bool& ptr) noexcept;

	private:
		class ConfigImpl;
		ConfigImpl* _pImpl;
	};
}