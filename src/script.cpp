#include <_script.hpp>

#include <vector>


namespace mebius::script {
	Script::Script(const char* filename) : _pImpl(new ScriptImpl(filename)) {}
	Script::~Script() { delete _pImpl; }
	const char* Script::ScriptImpl::dir = "scripts\\";

	bool Script::execute() {
		return _pImpl->execute_impl();
	}
	bool Script::execute_func(const char* func, std::initializer_list<uint32_t> args) {
		return _pImpl->execute_func_impl(func, args);
	}

	bool Script::ScriptImpl::execute_impl() {
		if (lua_pcall(L, 0, 0, 0)) {
			return false;
		}
		return true;
	}

	bool Script::ScriptImpl::execute_func_impl(const char* func, std::initializer_list<uint32_t> args) {
		lua_getglobal(L, func);
		for (uint32_t arg : args) {
			lua_pushinteger(L, arg);
		}

		if (lua_pcall(L, args.size(), 1, 0)) return false;
		return lua_toboolean(L, -1);
	}


	extern "C" static int CallStdCallProc(lua_State * L)
	{
		uint32_t num = lua_gettop(L) - 2; // Second argument

		if (!lua_isinteger(L, 1)) {
			lua_pushboolean(L, false);
			lua_pushinteger(L, -1);
			return 2; // Count of returned values
		}
		const char* ModuleName = lua_tostring(L, 1); // First argument
		HMODULE ModuleHandle = GetModuleHandleA(ModuleName);
		if (!ModuleHandle) {
			lua_pushboolean(L, false);
			lua_pushinteger(L, -1);
			return 2; // Count of returned values
		}


		if (lua_isinteger(L, 2)) {
			lua_pushboolean(L, false);
			lua_pushinteger(L, -1);
			return 2; // Count of returned values
		}
		const char* ProcName = lua_tostring(L, 2); // First argument
		FARPROC ProcAddress = GetProcAddress(ModuleHandle, ProcName);
		if (!ProcAddress) {
			lua_pushboolean(L, false);
			lua_pushinteger(L, -1);
			return 2; // Count of returned values
		}

		uint32_t* args = (uint32_t*)malloc(num * sizeof(uint32_t));
		if (!args) {
			lua_pushboolean(L, false);
			lua_pushinteger(L, -1);
			return 2; // Count of returned values
		}
		for (int i = 0; i < num; i++) {
			if (!lua_isinteger(L, 3 + i)) {
				args[i] = (uint32_t)lua_tostring(L, 3 + i);
			}
			else {
				args[i] = lua_tointeger(L, 3 + i);
			}
		}

		uint32_t ret = 0xFFFFFFFF;
		__asm {
			MOV EAX, args
			MOV ECX, num
			L_PUSH_LOOP :
			TEST ECX, ECX
				JE L_PUSH_LOOP_END
				DEC ECX
				PUSH[EAX + ECX * 4]
				JMP L_PUSH_LOOP
				L_PUSH_LOOP_END :
			CALL ProcAddress
				MOV ret, EAX
		}

		free(args);
		lua_pushboolean(L, true);
		lua_pushinteger(L, ret);
		return 2; // Count of returned values
	}

	extern "C" static int CallCdeclProc(lua_State * L)
	{
		uint32_t num = lua_gettop(L) - 2; // Second argument

		if (lua_isinteger(L, 1)) {
			lua_pushboolean(L, false);
			lua_pushinteger(L, -1);
			return 2; // Count of returned values
		}
		const char* ModuleName = lua_tostring(L, 1); // First argument
		HMODULE ModuleHandle = GetModuleHandleA(ModuleName);
		if (!ModuleHandle) {
			lua_pushboolean(L, false);
			lua_pushinteger(L, -1);
			return 2; // Count of returned values
		}


		if (lua_isinteger(L, 2)) {
			lua_pushboolean(L, false);
			lua_pushinteger(L, -1);
			return 2; // Count of returned values
		}
		const char* ProcName = lua_tostring(L, 2); // First argument
		FARPROC ProcAddress = GetProcAddress(ModuleHandle, ProcName);
		if (!ProcAddress) {
			lua_pushboolean(L, false);
			lua_pushinteger(L, -1);
			return 2; // Count of returned values
		}

		uint32_t* args = (uint32_t*)malloc(num * sizeof(uint32_t));
		if (!args) {
			lua_pushboolean(L, false);
			lua_pushinteger(L, -1);
			return 2; // Count of returned values
		}
		for (int i = 0; i < num; i++) {
			if (!lua_isinteger(L, 3 + i)) {
				args[i] = (uint32_t)lua_tostring(L, 3 + i);
			}
			else {
				args[i] = lua_tointeger(L, 3 + i);
			}
		}

		int ret = 0xFFFFFFFF;
		__asm {
			MOV EAX, args
			MOV ECX, num
			L_PUSH_LOOP :
			TEST ECX, ECX
				JE L_PUSH_LOOP_END
				DEC ECX
				PUSH[EAX + ECX * 4]
				JMP L_PUSH_LOOP
				L_PUSH_LOOP_END :
			CALL ProcAddress
				MOV ret, EAX
				MOV ECX, num
				L_POP_LOOP :
			TEST ECX, ECX
				JE L_POP_LOOP_END
				DEC ECX
				POP EAX
				JMP L_POP_LOOP
				L_POP_LOOP_END :
		}

		free(args);

		lua_pushboolean(L, true);
		lua_pushinteger(L, ret);
		return 2; // Count of returned values
	}

	extern "C" static int CallAddress(lua_State * L)
	{
		uint32_t num = lua_gettop(L) - 1;
		if (num < 1) {
			lua_pushboolean(L, false);
			lua_pushinteger(L, -1);
			return 2; // Count of returned values
		}

		if (!lua_isinteger(L, 1)) {
			lua_pushboolean(L, false);
			lua_pushinteger(L, -1);
			return 2; // Count of returned values
		}
		uint32_t Function = lua_tointeger(L, 1); // First argument

		uint32_t* args = (uint32_t*)malloc(num * sizeof(uint32_t));
		if (!args) {
			lua_pushboolean(L, false);
			lua_pushinteger(L, -1);
			return 2; // Count of returned values
		}
		for (int i = 0; i < num; i++) {
			if (!lua_isinteger(L, 2 + i)) {
				args[i] = (uint32_t)lua_tostring(L, 2 + i);
			}
			else {
				args[i] = lua_tointeger(L, 2 + i);
			}
		}

		int ret = 0xFFFFFFFF;
		__asm {
			MOV EAX, args
			MOV ECX, num
			L_PUSH_LOOP :
			TEST ECX, ECX
				JE L_PUSH_LOOP_END
				DEC ECX
				PUSH[EAX + ECX * 4]
				JMP L_PUSH_LOOP
				L_PUSH_LOOP_END :
			CALL Function
				MOV ret, EAX
				MOV ECX, num
				L_POP_LOOP :
			TEST ECX, ECX
				JE L_POP_LOOP_END
				DEC ECX
				POP EAX
				JMP L_POP_LOOP
				L_POP_LOOP_END :
		}

		free(args);

		lua_pushboolean(L, true);
		lua_pushinteger(L, ret);
		return 2; // Count of returned values
	}
}
