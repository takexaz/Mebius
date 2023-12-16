#pragma once
#include <Windows.h>
#include <Wincrypt.h>
#include <vector>
#include <iostream>



namespace Mebius::Utility {
	const static std::vector<std::string> MUGEN_HASH_LIST = {
	"78299bb4c5ddbe8c58093a9b736d7b71", // Private Beta Version
	"c702e03ca0b396f2a5acb89fb69fd0e6", // Private Beta Version Decompiled
	};
	std::string calc_md5_self();
	bool is_mugen(std::string md5);
	void check_mugen(void);
}