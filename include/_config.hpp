#pragma once
#include <toml++/toml.hpp>

namespace mebius::config {
	const static char* conf_mebius_path = "mods\\mebius.toml";


	struct CF_MEBIUS {

		struct CF_OPTIONS {
			bool Enable = false;
			bool BypassCheckSum = false;
		};
		CF_OPTIONS Options;

		struct CF_DEBUG {

			struct CF_CONSOLE {
				bool Enable = false;
				bool Log = false;
				bool Error = false;
			};
			CF_CONSOLE Console;

		};
		CF_DEBUG Debug;
	};


	bool get_bool_from_key(toml::table tbl, const char* key);
	CF_MEBIUS get_config(void);
}