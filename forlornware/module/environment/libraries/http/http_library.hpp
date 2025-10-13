#pragma once
#include "../../environment.hpp"
#include <unordered_set>

class http_library
{
public:
	static void initialize(lua_State* L);

	// functions so we can register them with the hooks
	static int httpget(lua_State* L);
	static int getobjects(lua_State* L);
};