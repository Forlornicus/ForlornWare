#include "execution.hpp"

void execution::execute_script(lua_State* l, const std::string& script) {
    if (script.empty())
        return;

    int original_top = lua_gettop(l);
    lua_State* thread = lua_newthread(l);
    lua_pop(l, 1);
    luaL_sandboxthread(thread);

    auto bytecode = global_functions::compile_script(script);
    if (luau_load(thread, "@ForlornWare", bytecode.c_str(), bytecode.size(), 0) != LUA_OK) {
        if (const char* err = lua_tostring(thread, -1))
            roblox::r_print(0, "%s", err);
        lua_pop(thread, 1);
        return;
    }

    if (auto closure = (Closure*)(lua_topointer(thread, -1)); closure && closure->l.p)
        context_manager::set_proto_capabilities(closure->l.p, &max_caps);

    lua_getglobal(l, "task");
    lua_getfield(l, -1, "defer");
    lua_remove(l, -2);
    lua_xmove(thread, l, 1);

    if (lua_pcall(l, 1, 0, 0) != LUA_OK) {
        if (const char* err = lua_tostring(l, -1))
            roblox::r_print(0, "%s", err);
        lua_pop(l, 1);
    }

    lua_settop(thread, 0);
    lua_settop(l, original_top);
}
