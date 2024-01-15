#pragma once

#include <debug.hpp>
#include <config.hpp>
#include <toml.hpp>

namespace mebius::config {
	class Config::ConfigImpl {
	private:
		std::optional<toml::table> _tbl;
	public:
		ConfigImpl(const char* path) 
		{
			try {
				_tbl = toml::parse_file(path);
			}
			catch (const toml::parse_error& err)
			{
				_tbl = std::nullopt;
			}
		}

		bool is_loaded(void) {
			return _tbl.has_value();
		}

		template <typename T>
		std::optional<T> get_value(const char* key) noexcept {
			if (_tbl.has_value()) {
				return _tbl.value().at_path(key).value<T>();
			}
			return std::nullopt;
		}
	};
}