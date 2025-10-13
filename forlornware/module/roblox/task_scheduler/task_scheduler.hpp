#pragma once
#include "Windows.h"
#include "iostream"
#include "vector"
#include "thread"

#include "lualib.h"
#include "VM/src/lobject.h"
#include "VM/src/lstate.h"
#include "VM/src/lapi.h"

#include "update.hpp"
#include "../../execution/execution.hpp"
#include "../context_manager/context_manager.hpp"
#include "misc/globals.hpp"

#include "environment/environment.hpp"

class task_scheduler
{
public:
	static void send_script(const std::string& script);
	static void initialize_scheduler();
};