#pragma once
#include <toml++/toml.hpp>

namespace Mebius::Config {
	const static char* conf_mebius_path = "mods\\mebius.toml";
	static toml::table conf_mebius;
}