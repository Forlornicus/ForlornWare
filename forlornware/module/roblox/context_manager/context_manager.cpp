#include "context_manager.hpp"

void context_manager::set_proto_capabilities(Proto* proto, uintptr_t* c) {
    if (!proto) return;
    proto->userdata = c;
    for (int i = 0; i < proto->sizep; ++i)
        set_proto_capabilities(proto->p[i], c);
}

void context_manager::set_thread_capabilities(lua_State* l, int lvl, uintptr_t c) {
    *(uintptr_t*)((uintptr_t)(l->userdata) + 0x48) = c;
    *(int*)((uintptr_t)(l->userdata) + 0x30) = lvl;
}

uintptr_t context_manager::get_datamodel() {
    uintptr_t fake_datamodel = *(uintptr_t*)(update::offsets::datamodel::fake_datamodel);
    return *(uintptr_t*)(fake_datamodel + update::offsets::datamodel::fake_datamodel_to_datamodel);
}

uintptr_t context_manager::get_script_context() {
    uintptr_t children_pointer = *(uintptr_t*)(get_datamodel() + update::offsets::instance::children);
    uintptr_t script_context = *(uintptr_t*)(*(uintptr_t*)(children_pointer)+update::offsets::datamodel::script_context);
    return script_context;
}

uintptr_t context_manager::get_lua_state() {
    uint64_t a2 = 0, a3 = 0;
    return roblox::get_state(get_script_context(), &a2, &a3);
}