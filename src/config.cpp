#include <Mebius.hpp>
#include <_Mebius.hpp>

namespace Mebius::Config {
	bool get_bool_from_key(toml::table tbl, const char* key) {
		auto node = tbl.at_path(key);
		if (node.is_boolean()) {
			return node.ref<bool>();
		}
		return false;
	}

	CF_MEBIUS get_config(void) {
		CF_MEBIUS conf;
		try {
			toml::table tbl = toml::parse_file(Mebius::Config::conf_mebius_path);
			conf.Options.Enable = get_bool_from_key(tbl, "Options.Enable");
			conf.Debug.Console.Enable = get_bool_from_key(tbl, "Debug.Console.Enable");
			conf.Debug.Console.Log = get_bool_from_key(tbl, "Debug.Console.Log");
			conf.Debug.Console.Error = get_bool_from_key(tbl, "Debug.Console.Error");
		}
		catch (const toml::parse_error& err)
		{
			return conf;
		}
		return conf;
	}
}