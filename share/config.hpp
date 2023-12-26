#pragma once

#include <iostream>
#include <memory>

namespace mebius::config {
	class Config
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