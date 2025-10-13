#include "tp_handler.hpp"
#include <thread>
#include <chrono>

bool is_game_loaded() {
    uintptr_t datamodel = context_manager::get_datamodel();
    if (!datamodel)
        return false;

    int64_t loaded_flag = 0;
    __try {
        loaded_flag = *(int64_t*)(datamodel + update::offsets::datamodel::game_loaded);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }

    return loaded_flag == 31;
}

void teleport_loop() {
    uintptr_t last_model = 0;

    while (true) {
        uintptr_t current_model = context_manager::get_datamodel();
        if (!current_model) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        uintptr_t model_instance = current_model;

        int attempts = 0;
        constexpr int max_attempts = 50;

        while (!is_game_loaded() && attempts++ < max_attempts) {
            current_model = context_manager::get_datamodel();
            if (!current_model)
                break;
            model_instance = current_model;
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }

        if (!is_game_loaded()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        try {
            task_scheduler::initialize_scheduler();
            last_model = current_model;
        }
        catch (...) {
            last_model = 0;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }

        while (is_game_loaded()) {
            globals::loaded = true;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        globals::loaded = false;
        last_model = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}

void teleport_handler::initialize() {
    std::thread(teleport_loop).detach();
}