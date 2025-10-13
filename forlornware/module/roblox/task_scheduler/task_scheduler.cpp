#include "task_scheduler.hpp"
#include <sstream>
#include <queue>
#include <functional>
#include <optional>

int hook_scheduler(lua_State* L) {
    if (!globals::execution_queue.empty()) {
        auto script = std::move(globals::execution_queue.front());
        globals::execution_queue.pop();
        execution::execute_script(globals::forlorn_state, std::move(script));
    }
    return 0;
}

void setup_queue(lua_State* L)
{
    lua_getglobal(L, "game");
    lua_getfield(L, -1, "GetService");
    lua_pushvalue(L, -2);

    lua_pushstring(L, "RunService");
    lua_pcall(L, 2, 1, 0);

    lua_getfield(L, -1, "RenderStepped");
    lua_getfield(L, -1, "Connect");
    lua_pushvalue(L, -2);

    lua_pushcclosure(L, hook_scheduler, nullptr, 0);
    lua_pcall(L, 2, 0, 0);
    lua_pop(L, 2);
}

void task_scheduler::initialize_scheduler() {
    globals::roblox_state = (lua_State*)context_manager::get_lua_state();
    globals::forlorn_state = lua_newthread(globals::roblox_state);

    context_manager::set_thread_capabilities(globals::forlorn_state, 8, max_caps);
    environment::initialize(globals::forlorn_state);

    setup_queue(globals::forlorn_state);

    task_scheduler::send_script(R"--(
        printidentity()
        print("ForlornWare has loaded!")
    )--");
}

void task_scheduler::send_script(const std::string& script) {
    if (!script.empty()) {
        globals::execution_queue.push(script);
    }
}