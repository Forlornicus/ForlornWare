#pragma once
#include "Windows.h"
#include "iostream"
#include "vector"

#include "update.hpp"
#include "misc/globals.hpp"

#include "luacode.h"
#include "luau/BytecodeBuilder.h"
#include "luau/BytecodeUtils.h"
#include "luau/Compiler.h"

#include "VM/src/lobject.h"
#include "VM/src/lstate.h"
#include "VM/src/lapi.h"

#include "lz4/include/lz4.h"
#include "zstd/include/zstd/xxhash.h"
#include "zstd/include/zstd/zstd.h"

#include "roblox/task_scheduler/task_scheduler.hpp"
#include "lualib.h"

struct lua_State;

class execution
{
public:
	static void execute_script(lua_State* l, const std::string& script);
};