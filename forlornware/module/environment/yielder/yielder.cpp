#include "yielder.hpp"

struct task_data
{
	lua_State* state;
	std::function<yielder::yield_return()> generator;
	PTP_WORK work;
};

void thread_worker(task_data* data)
{
	try
	{
		auto yield_result = data->generator();
		int result_count = yield_result(data->state);

		lua_State* thread_ctx = lua_newthread(data->state);

		lua_getglobal(thread_ctx, "task");
		lua_getfield(thread_ctx, -1, "defer");
		lua_pushthread(data->state);
		lua_xmove(data->state, thread_ctx, 1);
		lua_pop(data->state, 1);

		for (int i = result_count; i >= 1; --i)
		{
			lua_pushvalue(data->state, -i);
			lua_xmove(data->state, thread_ctx, 1);
		}

		lua_pcall(thread_ctx, result_count + 1, 0, 0);
		lua_settop(thread_ctx, 0);
	}
	catch (...)
	{
	}

	delete data;
}

int yielder::yield_execution(lua_State* L, const std::function<yield_return()>& generator)
{
	lua_pushthread(L);
	lua_ref(L, -1);
	lua_pop(L, 1);

	auto* task = new task_data{ L, generator, nullptr };

	std::thread(thread_worker, task).detach();

	L->base = L->top;
	L->status = LUA_YIELD;
	L->ci->flags |= 1;
	return -1;
}