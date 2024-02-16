#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define LIBRG_IMPLEMENTATION
#include <librg.h>

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <inttypes.h>
#include <Windows.h>
#include <thread>
#include <fstream>
#include <climits>
#include <functional>
#include <librg_ext.h>

//Include hooking lib
#include "MinHook.h"
#include "json.hpp"
//Font wrapper lib
#include "FW1FontWrapper.h"

//Include lua
extern "C" {
	#include "lua/lua.h"
	#include "lua/lauxlib.h"
	#include "lua/lualib.h"
}

HINSTANCE dll_module;
uint64_t base_address = NULL;
librg_ctx_t ctx = { 0 };

// helper macros
#define PRINT_HEX(s, h) printf("%s 0x%" PRIX64 "\n", s, h);
#define RESOLVE_ADDRESS(address) (address + base_address)
#define VEC_PRINT(name, vec) printf("%s {%f, %f, %f}\n", name, vec.x, vec.y, vec.z)
#define VTABLE(offset) *(uint64_t *)this + offset

class Actor;
Actor* local_horse = nullptr;

#include "utils.hpp"
#include "config.hpp"
#include "hooker.hpp"
#include "structs.hpp"
#include "lua_hook.hpp"
#include "game.hpp"
#include "mp.hpp"
#include "graphics.hpp"
#include "lua_harcoded.hpp"


void on_dll_injected();
KeyToggle luaExecute(VK_F5);
KeyToggle ourExecute(VK_F6);

BOOL WINAPI DllMain(HINSTANCE dll_instnace, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(dll_instnace);
		dll_module = dll_instnace;
		on_dll_injected();
	}
	return TRUE;
}

/* 
* Usefull thread for detaching from process
* This allows us to inject same dll into single process multiple times
*/
void detach_thread(PVOID pThreadParameter) {

	printf("[*] DetachThread started !\n");
	while (true) {
		//F4 - Detach library
		if (GetAsyncKeyState(VK_F4)) {
			Sleep(100);

			fclose(stdin);
			fclose(stdout);
			fclose(stderr);

			hooker::free();
			FreeConsole();
			FreeLibraryAndExitThread((HMODULE)dll_module, 0);
		}

		Sleep(10);
	}
}

static int myprint(lua_State* L) {
	
	const char* text = lua_tostring(L, -1);
	printf("[*] %s\n", text);

	return 0;
}

void on_dll_injected() {

	//Allocate new console & reopen stdin, stdout
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	//Get base address of game module for offset address calc
	base_address = (uint64_t)GetModuleHandle("WHGame.dll");

	config_get();
	hooker::init();
	game::install_hooks();

	lua::on_init = []() {
		lua_register(lua::L, "myprint", myprint);
	};

	lua::on_tick = []() {
		
		if (luaExecute) {
			lua::execute(get_file_content("test.lua"));
			printf("executed!\n");
		}

		if (ourExecute) {
			lua::execute(lua_mp_script);
			mp::on_init();
			mp::connect();
		}
		
		mp::on_tick();
	};

	lua::hook();
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&detach_thread, 0, 0, 0);	
}