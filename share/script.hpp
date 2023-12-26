#pragma once

#include <debug.hpp>

namespace mebius::script {
	class Script
	{
	public:
		Script(const char* filename);
		~Script();
		bool execute();
		bool execute_func(const char* func, std::initializer_list<uint32_t> args);
	private:
		class ScriptImpl;
		ScriptImpl* _pImpl;
	};
}