#include "hooks.hpp"
#include <vector>
#include <string>
#include <string_view>
#include <unordered_set>
#include <algorithm>
#include <cctype>

inline std::string to_lower(std::string_view s)
{
    std::string r;
    r.reserve(s.size());
    for (unsigned char c : s) r.push_back((char)std::tolower(c));
    return r;
}

static const std::unordered_set<std::string> dangerous_functions =
{
    "openvideosfolder","openscreenshotsfolder","getrobuxbalance","performpurchase",
    "promptbundlepurchase","promptnativepurchase","promptproductpurchase","promptpurchase",
    "promptthirdpartypurchase","publish","getmessageid","openbrowserwindow",
    "requestinternal","executejavascript","togglerecording","takescreenshot",
    "httprequestasync","getlast","sendcommand","getasync","getasyncfullurl",
    "requestasync","makerequest","postasync","httppost","postasyncfullurl",
    "performpurchasev2","promptgamepasspurchase","promptrobloxpurchase",
    "opennativeoverlay","addcorescriptlocal","emithybridevent","returntojavascript",
    "call","openurl","savescriptprofilingdata","getprotocolmethodrequestmessageid",
    "getprotocolmethodresponsemessageid","publishprotocolmethodrequest",
    "publishprotocolmethodresponse","subscribe","subscribetoprotocolmethodrequest",
    "subscribetoprotocolmethodresponse","promptnativepurchasewithlocalplayer",
    "promptcollectiblespurchase","performbulkpurchase","performcancelsubscription",
    "performsubscriptionpurchase","performsubscriptionpurchasev2",
    "preparecollectiblespurchase","promptbulkpurchase","promptcancelsubscription",
    "promptpremiumpurchase","promptsubscriptionpurchase","openwechatauthwindow",
    "requestlimitedasync","run","capturescreenshot","createpostasync",
    "deletecapture","deletecapturesasync","getcapturefilepathasync",
    "savecapturetoexternalstorage","savecapturestoexternalstorageasync",
    "getcaptureuploaddataasync","retrievecaptures","savescreenshotcapture",
    "getcredentialsheaders","getdeviceintegritytoken","getdeviceintegritytokenyield",
    "nopromptcreateoutfit","nopromptdeleteoutfit","nopromptrenameoutfit",
    "nopromptsaveavatar","nopromptsaveavatarthumbnailcustomization","nopromptsetfavorite",
    "nopromptupdateoutfit","performcreateoutfitwithdescription","performrenameoutfit",
    "performsaveavatarwithdescription","performsetfavorite","performupdateoutfit",
    "promptcreateoutfit","promptdeleteoutfit","promptrenameoutfit","promptsaveavatar",
    "promptsetfavorite","promptupdateoutfit","promptimportfile","promptimportfiles",
    "getusersubscriptionpaymenthistoryasync","getusersubscriptiondetailsinternalasync",
    "callfunction","bindcoreactive","reportabuse","reportabusev3","reportchatabuse",
    "load", "getusersubscriptionstatusasync"
};

static const std::unordered_set<std::string> dangerous_services =
{
    "accountservice","testservice","omnirecommendationsservice","messagebusservice",
    "linkingservice","browserservice","opencloudservice","scriptprofilerservice"
};

static __int64 old_namecall;
static __int64 old_index;
static __int64 old_newindex;

static bool is_dangerous_function(const char* name)
{
    return dangerous_functions.count(to_lower(name)) != 0;
}

static bool is_dangerous_service(const char* name)
{
    return dangerous_services.count(to_lower(name)) != 0;
}

auto namecall_hook(lua_State* L) -> int
{
    const auto script_ptr = *(std::uintptr_t*)((std::uintptr_t)(L->userdata) + 0x50);
    if (L->namecall && !script_ptr)
    {
        const char* method = L->namecall->data;
        std::string m = to_lower(method);

        if (m == "getservice" && lua_isstring(L, 2))
        {
            const char* svc = lua_tostring(L, 2);
            if (is_dangerous_service(svc))
            {
                luaL_error(L, "blocked service called");
                return 0;
            }
        }

        if (is_dangerous_function(method))
        {
            luaL_error(L, "blocked method called");
            return 0;
        }

        if (m == "httpget" || m == "httpgetasync")
            return http_library::httpget(L);

        if (m == "getobjects" || m == "getobjectsasync")
            return http_library::getobjects(L);
    }

    return static_cast<int>(((__int64(__fastcall*)(__int64))old_namecall)((__int64)L));
}

auto index_hook(lua_State* L) -> int
{
    auto state = (__int64)L;
    const auto script_ptr = *(std::uintptr_t*)((std::uintptr_t)(L->userdata) + 0x50);
    uintptr_t userdata = *(uintptr_t*)(state + 0x78);
    int level = static_cast<int>(*(uintptr_t*)(userdata + 0x30));

    if (lua_isstring(L, 2) && !script_ptr)
    {
        const char* key = luaL_checkstring(L, 2);
        if (is_dangerous_function(key))
        {
            luaL_error(L, "blocked function called");
            return 0;
        }

        std::string k = to_lower(key);
        if (k == "httpget" || k == "httpgetasync")
        {
            lua_getglobal(L, "httpget");
            return 1;
        }

        if (k == "getobjects" || k == "getobjectsasync")
        {
            lua_getglobal(L, "getobjects");
            return 1;
        }
    }

    return static_cast<int>(((__int64(__fastcall*)(__int64))old_index)((__int64)L));
}

auto newindex_hook(lua_State* L) -> int
{
    auto state = (__int64)L;
    const auto script_ptr = *(std::uintptr_t*)((std::uintptr_t)(L->userdata) + 0x50);
    if (!script_ptr && lua_isuserdata(L, 1) && lua_isstring(L, 2))
    {
        const char* key = luaL_checkstring(L, 2);
        if (is_dangerous_function(key))
        {
            luaL_error(L, "blocked function called");
            return 0;
        }

        uintptr_t userdata = *(uintptr_t*)(state + 0x78);
        int* identity = (int*)(userdata + 0x30);
        int prev = *identity;
        *identity = 8;
        int r = static_cast<int>(((__int64(__fastcall*)(__int64))old_newindex)((__int64)L));
        *identity = prev;
        return r;
    }
    return static_cast<int>(((__int64(__fastcall*)(__int64))old_newindex)((__int64)L));
}

void hooks::initialize(lua_State* L)
{
    lua_getglobal(L, "game");
    lua_getmetatable(L, -1);
    lua_getfield(L, -1, "__namecall");
    Closure* namecall = (Closure*)lua_topointer(L, -1);
    lua_CFunction namecall_f = namecall->c.f;
    old_namecall = (__int64)namecall_f;
    namecall->c.f = namecall_hook;
    lua_settop(L, 0);

    lua_getglobal(L, "game");
    lua_getmetatable(L, -1);
    lua_getfield(L, -1, "__index");
    Closure* index = (Closure*)lua_topointer(L, -1);
    lua_CFunction index_f = index->c.f;
    old_index = (__int64)index_f;
    index->c.f = index_hook;
    lua_settop(L, 0);

    lua_getglobal(L, "game");
    lua_getmetatable(L, -1);
    lua_getfield(L, -1, "__newindex");
    Closure* newindex = (Closure*)lua_topointer(L, -1);
    lua_CFunction newindex_f = newindex->c.f;
    old_newindex = (__int64)newindex_f;
    newindex->c.f = newindex_hook;
    lua_settop(L, 0);
}