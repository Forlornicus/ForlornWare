#pragma once
#include <cstdint>
#include <lua.h>
#include <Windows.h>
#include <queue>
#include <psapi.h>

#include "Compiler/include/luacode.h"
#include "luau/BytecodeBuilder.h"
#include "luau/BytecodeUtils.h"
#include "luau/Compiler.h"

#include "zstd/include/zstd/zstd.h"

inline uintptr_t max_caps = 0xEFFFFFFFFFFFFFFF;

class bytecode_encoder : public Luau::BytecodeEncoder {
    inline void encode(uint32_t* data, size_t count) override {
        for (auto i = 0u; i < count;) {
            uint8_t op = LUAU_INSN_OP(data[i]);
            const auto opLength = Luau::getOpLength(static_cast<LuauOpcode>(op));
            const auto lookupTable = reinterpret_cast<BYTE*>(update::lua::opcode_lookup);
            uint8_t newOp = op * 227;
            newOp = lookupTable[newOp];
            data[i] = (newOp) | (data[i] & ~0xff);
            i += opLength;
        }
    }
};

inline bytecode_encoder encoder;

namespace global_functions
{
    inline std::string compile_script(const std::string& omegahacker) {
        static const char* mutable_globals[] = {
            "Game", "Workspace", "game", "plugin", "script", "shared", "workspace",
            "_G", "_ENV", nullptr
        };

        Luau::CompileOptions options;
        options.debugLevel = 1;
        options.optimizationLevel = 1;
        options.mutableGlobals = mutable_globals;
        options.vectorLib = "Vector3";
        options.vectorCtor = "new";
        options.vectorType = "Vector3";

        return Luau::compile(omegahacker, options, {}, &encoder);
    }

    inline std::string decompress_bytecode(const std::string& c) {
        uint8_t h[4]; memcpy(h, c.data(), 4);
        for (int i = 0; i < 4; i++) h[i] = (h[i] ^ "RSB1"[i]) - i * 41;
        std::vector<uint8_t> v(c.begin(), c.end());
        for (size_t i = 0; i < v.size(); i++) v[i] ^= h[i % 4] + i * 41;
        int len; memcpy(&len, v.data() + 4, 4);
        std::string out(len, 0);
        return ZSTD_decompress(out.data(), len, v.data() + 8, v.size() - 8) == len ? out : "";
    }

    inline std::string read_bytecode(uintptr_t addr) {
        uintptr_t str = addr + 0x10;
        size_t len = *(size_t*)(str + 0x10);
        size_t cap = *(size_t*)(str + 0x18);
        uintptr_t data_ptr = (cap > 0x0f) ? *(uintptr_t*)(str + 0x00) : str;
        return std::string(reinterpret_cast<const char*>(data_ptr), len);
    }
}

namespace globals {
    inline lua_State* roblox_state;
    inline lua_State* forlorn_state;
    inline bool loaded = false;

    inline std::queue<std::string> teleport_queue = {};
    inline std::queue<std::string> execution_queue = {};
}