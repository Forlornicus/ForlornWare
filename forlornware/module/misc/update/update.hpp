#pragma once
#include <cstdint>
#include <string>
#include <Windows.h>
#include <iostream>

#include "enchelper/enchelper.hpp"

struct lua_State;

#define rebase(x) x + (uintptr_t)GetModuleHandle(nullptr)
#define rebase_hyperion(x) x + (uintptr_t)GetModuleHandleA("RobloxPlayerBeta.dll")
    
// updated for version-d34359a5577645e2
namespace update
{
    namespace roblox
    {
        const uintptr_t print = rebase(0x1826D00);
        const uintptr_t luad_throw = rebase(0x2AB27D0);
        const uintptr_t get_lua_state = rebase(0xEC65B0);
    }

    namespace lua
    {
        const uintptr_t luao_nilobject = rebase(0x4FA6B18);
        const uintptr_t luau_execute = rebase(0x2AB9080);
        const uintptr_t luah_dummynode = rebase(0x4FA6538);
        const uintptr_t opcode_lookup = rebase(0x55DCD40);
    }

    namespace offsets
    {
        namespace datamodel
        {
            const uintptr_t fake_datamodel = rebase(0x720A138);
            const uintptr_t fake_datamodel_to_datamodel = 0x1C0;
            const uintptr_t script_context = 0x3D0;
            const uintptr_t game_loaded = 0x680;
        }

        namespace script
        {
            const uintptr_t localscript = 0x1B0;
            const uintptr_t modulescript = 0x158;
        }

        namespace instance
        {
            const uintptr_t name = 0x80;
            const uintptr_t children = 0x60;
        }
    }   
}

namespace roblox
{
    using print_func_t = int(__fastcall*)(int64_t, const char*, ...);
    inline print_func_t r_print = reinterpret_cast<print_func_t>(update::roblox::print);

    using decrypt_state_t = int64_t(__fastcall*)(int64_t, uint64_t*, uint64_t*);
    inline decrypt_state_t get_state = reinterpret_cast<decrypt_state_t>(update::roblox::get_lua_state);

    using luad_throw_t = void(__fastcall*)(lua_State*, int);
    inline luad_throw_t luad_throw = reinterpret_cast<luad_throw_t>(update::roblox::luad_throw);
}

#define LUAU_COMMA_SEP ,
#define LUAU_SEMICOLON_SEP ;

#define LUAU_SHUFFLE2(s, a1, a2 ) a1 s a2
#define LUAU_SHUFFLE3(s, a1, a2, a3 ) a1 s a3 s a2
#define LUAU_SHUFFLE4(s, a1, a2, a3, a4 ) a1 s a2 s a3 s a4
#define LUAU_SHUFFLE5(s, a1, a2, a3, a4, a5 ) a2 s a4 s a1 s a3 s a5
#define LUAU_SHUFFLE6(s, a1, a2, a3, a4, a5, a6 ) a3 s a1 s a2 s a5 s a4 s a6
#define LUAU_SHUFFLE7(s, a1, a2, a3, a4, a5, a6, a7 ) a6 s a2 s a7 s a3 s a1 s a4 s a5
#define LUAU_SHUFFLE8(s, a1, a2, a3, a4, a5, a6, a7, a8 ) a8 s a2 s a5 s a1 s a4 s a7 s a3 s a6
#define LUAU_SHUFFLE9(s, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) a2 s a3 s a4 s a1 s a9 s a8 s a5 s a7 s a6

#define TSTRING_HASH_ENC VMValue1
#define PROTO_DEBUGNAME_ENC VMValue1

#define PROTO_MEMBER2_ENC VMValue4
#define CLOSURE_DEBUGNAME_ENC VMValue4

#define PROTO_DEBUGISN_ENC VMValue3
#define CLOSURE_CONT_ENC VMValue3
#define UDATA_META_ENC VMValue3

#define PROTO_TYPEINFO_ENC VMValue2
#define LSTATE_STACKSIZE_ENC VMValue2