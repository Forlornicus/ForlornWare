#include "script_library.hpp"

int identifyexecutor(lua_State* L)
{
	lua_pushstring(L, "ForlornWare");
	lua_pushstring(L, "1.0.0");
	return 2;
}

int getgenv(lua_State* L)
{
	lua_pushvalue(L, LUA_ENVIRONINDEX);
	return 1;
}

int getscriptbytecode(lua_State* L)
{
	if (lua_type(L, 1) != LUA_TUSERDATA) { lua_pushnil(L); return 1; }

	uintptr_t script = *(uintptr_t*)lua_touserdata(L, 1);
	if (!script) 
	{ 
		lua_pushnil(L); 
		return 1; 
	}

	lua_getfield(L, 1, "ClassName");
	const char* name = lua_tostring(L, -1);
	lua_pop(L, 1);

	uintptr_t addr = name && strcmp(name, "ModuleScript") == 0
		? *(uintptr_t*)(script + update::offsets::script::modulescript)
		: *(uintptr_t*)(script + update::offsets::script::localscript);

	std::string bytecode = global_functions::read_bytecode(addr);
	std::string code = addr ? global_functions::decompress_bytecode(bytecode) : "";
	if (code.empty()) 
	{ 
		lua_pushnil(L); 
		return 1; 
	}

	lua_pushlstring(L, code.data(), code.size());
	return 1;
}

void script_library::initialize(lua_State* L)
{
	register_env_functions(L,
		{
			{"identifyexecutor", identifyexecutor},
			{"getgenv", getgenv},

			{"getscriptbytecode", getscriptbytecode},
			{nullptr, nullptr}
		});
}