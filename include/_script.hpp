#pragma once

#include <script.hpp>
#include <debug.hpp>
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace mebius::script {
	extern "C" static int CallStdCallProc(lua_State * L);
	extern "C" static int CallCdeclProc(lua_State * L);
	extern "C" static int CallAddress(lua_State * L);

	class Script::ScriptImpl{
	public:
		ScriptImpl(const char* filename) : L(luaL_newstate()) {
			luaL_openlibs(L);

			char* path = (char*)malloc(strlen(dir) + strlen(filename) + 1);
			if (!path) return;
			strcpy(path, dir);
			strcat(path, filename);

			if (luaL_loadfile(L, path)) return;

			lua_pushcfunction(L, CallCdeclProc);
			lua_setglobal(L, "CallCdeclProc");
			lua_pushcfunction(L, CallStdCallProc);
			lua_setglobal(L, "CallStdCallProc");
			lua_pushcfunction(L, CallAddress);
			lua_setglobal(L, "CallAddress");

			free(path);
			_loaded = true;
		}

		~ScriptImpl() {
			lua_close(L);
		}
		bool execute_impl();
		bool execute_func_impl(const char* func, std::initializer_list<uint32_t> args);
	private:
		static const char* dir;
		bool _loaded = false;
		lua_State* L = nullptr;
	};


}