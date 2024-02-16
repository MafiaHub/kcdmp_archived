#pragma once
namespace lua
{
    lua_State* L = nullptr;
	std::function<void()> on_tick = nullptr;
	std::function<void()> on_init = nullptr;

	inline void execute(const std::string & lua_buffer);

    namespace hooks
    {
		/************************************************************************/
		/* CScriptSystem__Update_t												*/
		/************************************************************************/
		typedef __int64(__fastcall *CScriptSystem__Update_t)(uint64_t _this);
		CScriptSystem__Update_t CScriptSystem__Update = nullptr;
		__int64 CScriptSystem__Update__hook(uint64_t _this)
		{
			if (on_tick != nullptr)
				on_tick();

			return CScriptSystem__Update(_this);
		}

        /************************************************************************/
        /* Lua Load Buffer impl													*/
        /************************************************************************/
        typedef int32_t(__cdecl *luaL_loadbuffer_t)(lua_State *L, char *buff, size_t size, char *name);
        luaL_loadbuffer_t pluaL_loadbuffer = nullptr;

        int32_t luaL_loadbuffer_hook(lua_State *L, char *buff, size_t size, char *name)
        {
            return pluaL_loadbuffer(L, buff, size, name);
        }

        /************************************************************************/
        /* Lua pcall implementation                                             */
        /************************************************************************/
        typedef int32_t(__cdecl *lua_pcall_t)(lua_State *L, int32_t nargs, int32_t nresults, int32_t errfunc);
        lua_pcall_t plua_pcall = nullptr;

        int32_t lua_pcall_hook(lua_State *L_, int32_t nargs, int32_t nresults, int32_t errfunc)
        {
			int32_t return_val = plua_pcall(L_, nargs, nresults, errfunc);
            if (L == nullptr)
            {
                printf("[LUA] Hooked !\n");
                L = L_;  

				if (on_init != nullptr)
					on_init();
            }

			return return_val;
        }
    }
	
    /* Hook Lua */
    inline void hook()
    {
        hooker::hook(reinterpret_cast<PVOID>(RESOLVE_ADDRESS(0x55FAF4)), &hooks::lua_pcall_hook, reinterpret_cast<PVOID*>(&hooks::plua_pcall));
        hooker::hook(reinterpret_cast<PVOID>(RESOLVE_ADDRESS(0x972ACC)), &hooks::luaL_loadbuffer_hook, reinterpret_cast<PVOID*>(&hooks::pluaL_loadbuffer));
		hooker::hook(reinterpret_cast<PVOID>(RESOLVE_ADDRESS(0x55A364)), &hooks::CScriptSystem__Update__hook, reinterpret_cast<PVOID*>(&hooks::CScriptSystem__Update));
	}

    /*Execute lua */
    inline void execute(const std::string & lua_buffer)
    {
		luaL_dostring(L, lua_buffer.c_str());
		printf("%s\n", lua_tostring(L, -1));
    }

	float getNumberField(lua_State* L, const char* key) {

		lua_pushstring(L, key);
		lua_gettable(L, -2);

		float result = lua_tonumber(L, -1);
		lua_pop(L, 1);
		return result;
	}

	zpl_vec3_t getVectorField() {
		return {
			getNumberField(L, "x"),
			getNumberField(L, "y"),
			getNumberField(L, "z")
		};
	}

	std::string getStringField(const char* key) {

		lua_pushstring(L, key);
		lua_gettable(L, -2);

		std::string result = lua_tostring(L, -1);
		lua_pop(L, 1);
		return result;
	}
}
