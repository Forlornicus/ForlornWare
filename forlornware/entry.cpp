#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>

#include "misc/teleport_handler/tp_handler.hpp"
#include "misc/communication/com.hpp"

void load() {
    teleport_handler::initialize();

    script_server server;
    if (!server.initialize(2304)) {
        roblox::r_print(0, "failed bc you are gay");
        return;
    } 

    while (true) {
        std::string script = server.receive_script();
        if (!script.empty()) {
            task_scheduler::send_script(script);
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

BOOL APIENTRY DllMain(HMODULE mod, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        // only uncomment this if the module is crashing after 30 secs (means your injector is pretty ass)
        // global_functions::patch_control_flow_guard(mod);

        std::thread(load).detach();
    }
    return TRUE;
}

// forlorn was here now im in your closet....
