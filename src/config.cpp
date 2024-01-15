#include <_config.hpp>
#include <optional>

namespace mebius::config {
	Config::Config(const char* path) : _pImpl(new ConfigImpl(path)) {}
	Config::~Config() { delete _pImpl; }

	std::optional<std::string_view> Config::get_string_view(const char* key) noexcept
	{
		return Config::_pImpl->get_value<std::string_view>(key);
	}
	std::optional<std::string> Config::get_string(const char* key) noexcept
	{
		return Config::_pImpl->get_value<std::string>(key);
	}
	std::optional<int64_t> Config::get_int(const char* key) noexcept {
		return Config::_pImpl->get_value<int64_t>(key);
	}
	std::optional<double> Config::get_double(const char* key) noexcept {
		return Config::_pImpl->get_value<double>(key);
	}
	std::optional<bool> Config::get_bool(const char* key) noexcept {
		return Config::_pImpl->get_value<bool>(key);
	}
	bool Config::is_loaded(void) noexcept {
		return Config::_pImpl->is_loaded();
	}
}