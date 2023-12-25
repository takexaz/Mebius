#include <_config.hpp>

namespace mebius::config {
	Config::Config(const char* path) : _pImpl(new ConfigImpl(path)) {}
	Config::~Config() { delete _pImpl; }

	bool Config::get_value_from_key(const char* key, std::string& ptr) noexcept {
		return Config::_pImpl->get_value_from_key_impl<std::string>(key, ptr);
	}
	bool Config::get_value_from_key(const char* key, int64_t& ptr) noexcept {
		return Config::_pImpl->get_value_from_key_impl<int64_t>(key, ptr);
	}
	bool Config::get_value_from_key(const char* key, double& ptr) noexcept {
		return Config::_pImpl->get_value_from_key_impl<double>(key, ptr);
	}
	bool Config::get_value_from_key(const char* key, bool& ptr) noexcept {
		return Config::_pImpl->get_value_from_key_impl<bool>(key, ptr);
	}
}