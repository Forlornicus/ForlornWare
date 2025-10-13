#pragma once
#include "../environment.hpp"
#include "../libraries/http/http_library.hpp"

class hooks
{
public:
	static void initialize(lua_State* L);
};