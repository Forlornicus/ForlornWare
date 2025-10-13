#include "http_library.hpp"
#include <cpr/cpr.h>

static const std::unordered_set<std::string> blocked_hosts =
{
    "accountinformation.roblox.com",
    "accountsettings.roblox.com",
    "twostepverification.roblox.com",
    "trades.roblox.com",
    "billing.roblox.com",
    "economy.roblox.com",
    "auth.roblox.com",
    "accountinformation.roproxy.com",
    "accountsettings.roproxy.com",
    "twostepverification.roproxy.com",
    "trades.roproxy.com",
    "billing.roproxy.com",
    "economy.roproxy.com",
    "auth.roproxy.com"
};

inline std::string to_lower(std::string_view s)
{
    std::string r;
    r.reserve(s.size());
    for (unsigned char c : s) r.push_back((char)std::tolower(c));
    return r;
}

inline std::string get_host(const std::string& url)
{
    auto scheme_pos = url.find("://");
    if (scheme_pos == std::string::npos) return {};
    auto host_begin = scheme_pos + 3;
    auto host_end = url.find('/', host_begin);
    if (host_end == std::string::npos) host_end = url.size();
    return url.substr(host_begin, host_end - host_begin);
}

inline bool is_blocked_url(const std::string& url)
{
    std::string host = to_lower(get_host(url));
    return blocked_hosts.count(host) != 0;
}

int http_library::getobjects(lua_State* L)
{
    lua_getglobal(L, "game");
    lua_getfield(L, -1, "GetService");
    lua_pushvalue(L, -2);
    lua_pushstring(L, "InsertService");
    lua_call(L, 2, 1);

    lua_getfield(L, -1, "LoadLocalAsset");
    lua_pushvalue(L, -2);
    lua_pushstring(L, lua_tostring(L, 2));
    lua_call(L, 2, 1);

    lua_newtable(L);
    lua_pushvalue(L, -2);
    lua_rawseti(L, -2, 1);
    return 1;
}

int http_library::httpget(lua_State* L)
{
    std::string url;
    if (!lua_isstring(L, 1))
    {
        luaL_checkstring(L, 2);
        url = lua_tostring(L, 2);
    }
    else
    {
        url = lua_tostring(L, 1);
    }

    if (url.find("http://") != 0 && url.find("https://") != 0)
    {
        luaL_argerror(L, 1, "Invalid protocol (expected 'http://' or 'https://')");
        return 0;
    }

    if (is_blocked_url(url))
    {
        luaL_error(L, "blocked roblox api domain");
        return 0;
    }

    std::optional<std::string> job_id;
    lua_getglobal(L, "game");
    lua_getfield(L, -1, "JobId");
    if (lua_isstring(L, -1))
        job_id = lua_tostring(L, -1);
    lua_pop(L, 2);

    uintptr_t place_id = 0;
    lua_getglobal(L, "game");
    lua_getfield(L, -1, "PlaceId");
    if (lua_isnumber(L, -1))
        place_id = static_cast<uintptr_t>(lua_tointeger(L, -1));
    lua_pop(L, 2);

    return yielder::yield_execution(L, [url, job_id, place_id]() -> yielder::yield_return {
        return [url, job_id, place_id](lua_State* L) -> int {
            std::string session_id_header;
            if (job_id.has_value())
                session_id_header = "{\"GameId\":\"" + job_id.value() + "\",\"PlaceId\":\"" + std::to_string(place_id) + "\"}";
            else
                session_id_header = "{\"GameId\":\"empty value\",\"PlaceId\":\"empty value\"}";

            cpr::Response r = cpr::Get(
                cpr::Url{ url },
                cpr::Header{ {"Roblox-Session-Id", session_id_header} }
            );

            if (r.error)
            {
                lua_pushstring(L, ("HttpGet failed: " + r.error.message).c_str());
                return 1;
            }

            if (r.text.empty())
            {
                lua_pushstring(L, "HttpGet failed: empty response");
                return 1;
            }

            lua_pushlstring(L, r.text.data(), r.text.size());
            return 1;
            };
        });
}

void http_library::initialize(lua_State* L)
{
    register_env_functions(L,
        {
            {"httpget", httpget},
            {"getobjects", getobjects},
            {nullptr, nullptr}
        });
}