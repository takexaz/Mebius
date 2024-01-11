#pragma once

#include <debug.hpp>
#include <config.hpp>
#include <toml.hpp>

namespace mebius::config {
	class Config::ConfigImpl {
	public:
		ConfigImpl(const char* path) : _tbl(toml::parse_file(path)) {}

		template <typename T>
		bool get_value_from_key_impl(const char* key, T& ptr) noexcept {
			try {
				auto node = _tbl.at_path(key);
				if (node.is<T>()) {
					ptr = node.ref<T>();
					return true;
				}
				PLOGE << "[" << key << "] type mismatch.";
				return false;
			}
			catch (const toml::parse_error& err)
			{
				PLOGE << err.what();
				return false;
			}
		}
	private:
		toml::table _tbl;
	};
}