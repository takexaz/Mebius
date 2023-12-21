#include <Mebius.hpp>
#include <_Mebius.hpp>

namespace mebius::config {
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
			toml::table tbl = toml::parse_file(mebius::config::conf_mebius_path);
			conf.Options.Enable = get_bool_from_key(tbl, "Options.Enable");
			conf.Options.BypassCheckSum = get_bool_from_key(tbl, "Options.BypassCheckSum");

			// コンソール出力
			conf.Debug.Console.Enable = get_bool_from_key(tbl, "Debug.Console.Enable");
			conf.Debug.Console.Default = get_bool_from_key(tbl, "Debug.Console.Default");
			conf.Debug.Console.Error = get_bool_from_key(tbl, "Debug.Console.Error");
		}
		catch (const toml::parse_error& err)
		{
			return conf;
		}
		return conf;
	}
}