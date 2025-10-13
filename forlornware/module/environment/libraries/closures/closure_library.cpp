#include "closure_library.hpp"

// the newcclosure and shit does not support yielding add it yourself!

static std::unordered_map<Closure*, Closure*> executor_closures;
static std::unordered_map<Closure*, Closure*> original_functions;

int loadstring(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TSTRING);

    const char* source = lua_tostring(L, 1);
    const char* chunk_name = luaL_optstring(L, 2, "ForlornWare");

    const std::string& bytecode = global_functions::compile_script(source);

    if (luau_load(L, chunk_name, bytecode.data(), bytecode.size(), 0) != LUA_OK)
    {
        lua_pushnil(L);
        lua_pushvalue(L, -2);
        return 2;
    }

    if (Closure* func = lua_toclosure(L, -1))
    {
        if (func->l.p)
            context_manager::set_proto_capabilities(func->l.p, &max_caps);
    }

    lua_setsafeenv(L, LUA_GLOBALSINDEX, false);
    return 1;
}

int hookfunction(lua_State* L)
{
    lua_normalisestack(L, 2);
    luaL_checktype(L, 1, LUA_TFUNCTION);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    Closure* original = lua_toclosure(L, 1);
    Closure* replacement = lua_toclosure(L, 2);

    if (!original || !replacement)
        luaL_error(L, "invalid closure");

    if (original->isC)
        lua_clonecfunction(L, 1);
    else
        lua_clonefunction(L, 1);

    Closure* cloned = clvalue(index2addr(L, -1));
    original_functions[original] = cloned;
    lua_pop(L, 1);

    if (original && replacement)
    {
        if (original->isC && replacement->isC)
            luaF_replacecclosure(L, original, replacement);
        else if (!original->isC && !replacement->isC)
            luaF_replacelclosure(L, original, replacement);
        /*else if (original->isC && !replacement->isC) // C->L
        {
            lua_pushvalue(L, 2);
            lua_pushcclosure(L, lclosure_wrapper, nullptr, 1);
            Closure* wrapped = lua_toclosure(L, -1);
            if (wrapped)
                luaF_replacecclosure(L, original, wrapped);
            lua_pop(L, 1);
        }*/
        else if (!original->isC && replacement->isC) // L->C
        {
            lua_newtable(L);
            lua_newtable(L);
            lua_pushvalue(L, LUA_GLOBALSINDEX);
            lua_setfield(L, -2, "__index");
            lua_setmetatable(L, -2);
            lua_pushvalue(L, 2);
            lua_setfield(L, -2, "tramp");

            const char* src = "return tramp(...)";
            const std::string& bc = global_functions::compile_script(src);
            if (luau_load(L, "@forlornware", bc.data(), bc.size(), 0) != LUA_OK)
            {
                lua_pop(L, 2);
                luaL_error(L, "failed to compile");
            }

            Closure* thunk = lua_toclosure(L, -1);
            if (thunk)
            {
                thunk->env = hvalue(luaA_toobject(L, -2));
                luaC_threadbarrier(L);
                if (thunk->l.p)
                    context_manager::set_proto_capabilities(thunk->l.p, &max_caps);

                luaF_replacelclosure(L, original, thunk);
            }
            lua_pop(L, 2);
        }
        else
            luaL_error(L, "hooking type not supported");
    }

    setclvalue(L, L->top, cloned);
    L->top += 1;

    return 1;
}

int restorefunction(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TFUNCTION);
    Closure* function = lua_toclosure(L, 1);
    if (!function)
        luaL_error(L, "invalid function");

    auto it = original_functions.find(function);
    if (it == original_functions.end())
        luaL_error(L, "closure is not hooked");

    Closure* original = it->second;

    if (original && function)
    {
        if (function->isC)
            luaF_replacecclosure(L, function, original);
        else
            luaF_replacelclosure(L, function, original);
    }

    original_functions.erase(function);
    return 0;
}

int clonefunction(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TFUNCTION);
    Closure* f = clvalue(index2addr(L, 1));
    if (!f) luaL_error(L, "idk huh?");

    f->isC ? lua_clonecfunction(L, 1) : lua_clonefunction(L, 1);
    return 1;
}

int iscclosure(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TFUNCTION);
    lua_pushboolean(L, clvalue(index2addr(L, 1))->isC);
    return 1;
}

int islclosure(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TFUNCTION);
    lua_pushboolean(L, !clvalue(index2addr(L, 1))->isC);
    return 1;
}

//int newcclosure(lua_State* L)
//{
//    luaL_checktype(L, 1, LUA_TFUNCTION);
//    Closure* f = clvalue(index2addr(L, 1));
//
//    if (!f) 
//        luaL_error(L, "invalid closure");
//
//    lua_pushvalue(L, 1);
//    if (!f->isC) 
//        lua_pushcclosure(L, lclosure_wrapper, nullptr, 1);
//
//    return 1;
//}

int isexecutorclosure(lua_State* L)
{
    if (lua_type(L, 1) != LUA_TFUNCTION) { lua_pushboolean(L, 0); return 1; }

    Closure* f = lua_toclosure(L, 1);
    bool ex = !f->isC ? (f->l.p && f->l.p->linedefined != 0) : luaF_getclosures().count(f) > 0;

    lua_pushboolean(L, ex);
    return 1;
}

void closure_library::initialize(lua_State* L)
{
	register_env_functions(L,
		{
			{"loadstring", loadstring},

            {"hookfunction", hookfunction},
            {"replaceclosure", hookfunction},
            {"restorefunction", restorefunction},

            {"clonefunction", clonefunction},

            {"iscclosure", iscclosure},
            {"islclosure", islclosure},

            {"isexecutorclosure", isexecutorclosure},
            {"checkclosure", isexecutorclosure},
            {"isourclosure", isexecutorclosure},

            //{"newcclosure", newcclosure},
			{nullptr, nullptr}
		}); 
}