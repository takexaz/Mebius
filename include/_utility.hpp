#pragma once
#include <Windows.h>
#include <unordered_map>
#include <iostream>

namespace mebius::util {

	const static uint32_t default_entry_point = 0x00494A80;

	const static std::unordered_map<std::string, std::pair<std::string, uint32_t>> MUGEN_HASH_LIST = {
	{"78299bb4c5ddbe8c58093a9b736d7b71", std::make_pair("WinMugen Private Beta", 0x004C38FC)},
	{"04dfd39848a425d5f415d935c143562b", std::make_pair("WinMugen-Hi", 0x004C39D4)},
	{"1bb6d03a27da7b3bd69d1d8ec77145a0", std::make_pair("WinMugen Plus", default_entry_point)},
	};

	std::string calc_md5_self();
	uint32_t detect_mugen(void);
}
