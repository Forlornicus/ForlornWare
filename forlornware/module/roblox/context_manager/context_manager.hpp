#pragma once
#include <iostream>
#include <Windows.h>

#include "VM/src/lobject.h"
#include "VM/src/lstate.h"
#include "VM/src/lapi.h"
#include "update.hpp"

class context_manager
{
public:
	static uintptr_t get_datamodel();
	static uintptr_t get_script_context();
	static uintptr_t get_lua_state();

	static void set_proto_capabilities(Proto* p, uintptr_t* c);
	static void set_thread_capabilities(lua_State* l, int lvl, uintptr_t c);
};