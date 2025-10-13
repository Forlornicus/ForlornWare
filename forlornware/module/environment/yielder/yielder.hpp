#pragma once
#include "../environment.hpp"
#include <thread>
#include "functional"

class yielder
{
public:
	using yield_return = std::function<int(lua_State* L)>;

	static int yield_execution(lua_State* L, const std::function<yield_return()>& generator);
};