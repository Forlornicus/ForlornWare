#include "environment.hpp"

void environment::initialize(lua_State* L)
{
    closure_library::initialize(L);
    script_library::initialize(L);
    http_library::initialize(L);

	hooks::initialize(L);

    luaL_sandboxthread(L);

    lua_newtable(L);
    lua_setglobal(L, "_G");

    lua_newtable(L);
    lua_setglobal(L, "shared");
}