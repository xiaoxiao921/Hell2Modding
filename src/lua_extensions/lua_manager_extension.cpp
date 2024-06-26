#include "lua_manager_extension.hpp"

#include "bindings/gui_ext.hpp"
#include "bindings/hades/audio.hpp"
#include "bindings/hades/data.hpp"
#include "bindings/hades/inputs.hpp"
#include "bindings/hades/lz4.hpp"
#include "bindings/lpeg.hpp"
#include "bindings/luasocket/luasocket.hpp"
#include "bindings/paths_ext.hpp"
#include "bindings/tolk/tolk.hpp"
#include "lua_module_ext.hpp"

namespace big::lua_manager_extension
{
	static void delete_everything()
	{
		std::scoped_lock l(g_manager_mutex);

		lua::hades::inputs::vanilla_key_callbacks.clear();

		g_is_lua_state_valid = false;

		g_lua_manager_instance.reset();

		LOG(INFO) << "state is no longer valid!";
	}

	static int the_state_is_going_down(lua_State* L)
	{
		delete_everything();

		return 0;
	}

	void init_lua_manager(sol::state_view& state, sol::table& lua_ext)
	{
		init_lua_state(state, lua_ext);
		init_lua_api(state, lua_ext);
	}

	static int open_debug_lib(lua_State* L)
	{
		luaL_requiref(L, "_rom_debug", luaopen_debug, 1 /*Leaves a copy of the module on the stack.*/);

		// Number of elements on the stack.
		return 1;
	}

	void init_lua_state(sol::state_view& state, sol::table& lua_ext)
	{
		// Register our cleanup functions when the state get destroyed.
		{
			sol::table my_takedown_metatable                           = state.create_table_with();
			my_takedown_metatable[sol::meta_function::garbage_collect] = the_state_is_going_down;
			sol::table my_takedown_table                               = lua_ext.create_named(
                std::format("..{}\xF0\x9F\x8F\xB4 \xF0\x9F\x8F\xB4 \xF0\x9F\x8F\xB4 \xF0\x9F\x8F\xB4 \xF0\x9F\x8F\xB4", rom::g_project_name),
                sol::metatable_key,
                my_takedown_metatable);
		}

		// clang-format off
		state.open_libraries(
			sol::lib::package,
			sol::lib::os,
			sol::lib::debug,
			sol::lib::io);
		// clang-format on

		lua_pushcfunction(state.lua_state(), open_debug_lib);
		lua_setglobal(state.lua_state(), "_rom_open_debug");
	}

	void init_lua_api(sol::state_view& state, sol::table& lua_ext)
	{
		auto on_import_table = lua_ext.create_named("on_import");

		// Lua API: Function
		// Table: on_import
		// Name: pre
		// Param: function: function: signature (string file_name, current_ENV_for_this_import) return nil or _ENV
		// The passed function will be called before the game loads a .lua script from the game's Content/Scripts folder.
		// The _ENV returned (if not nil) by the passed function gives you a way to define the _ENV of this lua script.
		on_import_table.set_function("pre",
		                             [](sol::protected_function f, sol::this_environment env)
		                             {
			                             auto mod = (lua_module_ext*)lua_module::this_from(env);
			                             if (mod)
			                             {
				                             mod->m_data_ext.m_on_pre_import.push_back(f);
			                             }
		                             });

		// Lua API: Function
		// Table: on_import
		// Name: post
		// Param: function: function: signature (string file_name)
		// The passed function will be called after the game loads a .lua script from the game's Content/Scripts folder.
		on_import_table.set_function("post",
		                             [](sol::protected_function f, sol::this_environment env)
		                             {
			                             auto mod = (lua_module_ext*)lua_module::this_from(env);
			                             if (mod)
			                             {
				                             mod->m_data_ext.m_on_post_import.push_back(f);
			                             }
		                             });

		// Let's keep that list sorted the same as the solution file explorer
		lua::hades::audio::bind(lua_ext);
		lua::hades::data::bind(state, lua_ext);
		lua::hades::inputs::bind(state, lua_ext);
		lua::hades::lz4::bind(lua_ext);
		lua::luasocket::bind(lua_ext);
		lua::tolk::bind(lua_ext);
		lua::gui_ext::bind(lua_ext);
		lua::lpeg::bind(lua_ext);
		lua::paths_ext::bind(lua_ext);
	}
} // namespace big::lua_manager_extension
