#pragma once

#include <debug.hpp>
#include <config.hpp>
#include <toml++/toml.hpp>

namespace mebius::config {
	class Config::ConfigImpl {
	public:
		ConfigImpl(const char* path) : _tbl(toml::parse_file(path)) {}

		template <typename T>
		bool get_value_from_key_impl(const char* key, T& ptr) noexcept {
			mebius::debug::Logger cerr(std::cout, FOREGROUND_PINK);
			try {
				auto node = _tbl.at_path(key);
				if (node.is<T>()) {
					ptr = node.ref<T>();
					return true;
				}
				cerr << std::format("[{}] is not [{}]", key, typeid(T).name()) << std::endl;
				return false;
			}
			catch (const toml::parse_error& err)
			{
				cerr << err.what() << std::endl;
				return false;
			}
		}
	private:
		toml::table _tbl;
	};
}