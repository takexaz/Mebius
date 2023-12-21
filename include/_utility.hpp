#pragma once
#include <Windows.h>
#include <unordered_map>
#include <iostream>



namespace mebius::util {
	const static std::unordered_map<std::string, std::string> MUGEN_HASH_LIST = {
	{"78299bb4c5ddbe8c58093a9b736d7b71", "WinMugen Private Beta"},
	{"04dfd39848a425d5f415d935c143562b", "WinMugen-Hi"},
	{"1bb6d03a27da7b3bd69d1d8ec77145a0", "WinMugen Plus"},
	};

	std::string calc_md5_self();
	bool checksum(void);
}
