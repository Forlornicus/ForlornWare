#pragma once
#include "Windows.h"
#include "iostream"
#include "vector"
#include <string>
#include <vector>
#include <algorithm>
#include <map>      
#include <cstdlib>   
#include <stdexcept> 
#include <ctime>    
#include <cctype>    
#include <random>    
#include <sstream>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#include "update.hpp"
#include "misc/globals.hpp"
#include "../roblox/context_manager/context_manager.hpp"
#include "yielder/yielder.hpp"
#include "hooks/hooks.hpp"

#include "VM/include/lua.h"
#include "VM/include/lualib.h"
#include "VM/src/lcommon.h"
#include "VM/src/lstring.h"
#include "VM/src/lfunc.h"
#include "VM/src/lmem.h"
#include "VM/src/lgc.h"
#include "VM/src/ltable.h"
#include "VM/src/lobject.h"
#include "VM/src/lstate.h"
#include "VM/src/lapi.h"
#include "VM/src/ldo.h"

#include "zstd/include/zstd/zstd.h"
#include "zstd/include/zstd/xxhash.h"
#include "Compiler/include/Luau/Compiler.h"
#include "Compiler/include/luacode.h"
#include "Common/include/Luau/BytecodeUtils.h"
#include "Compiler/include/Luau/BytecodeBuilder.h"

#include "cpr/cpr.h"
#include "curl/curl.h"

// environment
#include "libraries/closures/closure_library.hpp"
#include "libraries/http/http_library.hpp"
#include "libraries/script/script_library.hpp"

#pragma comment(lib, "secur32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

inline void register_env_functions(lua_State* l, std::initializer_list<luaL_Reg> functions) {
    lua_pushvalue(l, LUA_GLOBALSINDEX);
    luaL_register(l, nullptr, functions.begin());
    lua_pop(l, 1);
}

inline void register_env_members(lua_State* l, std::initializer_list<luaL_Reg> functions, const std::string& global_name) {
    luaL_register(l, global_name.c_str(), functions.begin());
}

inline void register_to_global(lua_State* l, std::initializer_list<luaL_Reg> functions, const std::string& global_name) {
    lua_getglobal(l, global_name.c_str());
    if (lua_istable(l, -1)) {
        lua_setreadonly(l, -1, false);
        luaL_register(l, nullptr, functions.begin());
        lua_setreadonly(l, -1, true);
    }
    lua_pop(l, 1);
}

class environment
{
public:
	static void initialize(lua_State* l);
};