#include "std_include.hpp"

#define GET_MODULE_HANDLE(HANDLE_OUT, NAME, T) \
	while (!(HANDLE_OUT)) { \
		if ((HANDLE_OUT) = (DWORD)GetModuleHandleA(NAME); !(HANDLE_OUT)) { \
			Sleep(100); (T) += 100u; \
			if ((T) >= 30000) {	\
				goto INIT_FAIL; \
			} \
		} \
	}

DWORD WINAPI find_window_loop(LPVOID)
{
	std::uint32_t _time = 0;
	HWND main_window = nullptr;
	MH_STATUS MH_INIT_STATUS = MH_UNKNOWN;

	// wait for window creation
	while (!main_window)
	{
		// get main window hwnd
		if (!main_window)
		{
			main_window = FindWindowA(nullptr, "Portal 2");
		}

		Sleep(100); _time += 100;
		if (_time >= 30000) { goto INIT_FAIL; }
	}

	GET_MODULE_HANDLE(game::shaderapidx9_module, "shaderapidx9.dll", _time);
	GET_MODULE_HANDLE(game::studiorender_module, "StudioRender.dll", _time);
	GET_MODULE_HANDLE(game::materialsystem_module, "MaterialSystem.dll", _time);
	GET_MODULE_HANDLE(game::engine_module, "engine.dll", _time);
	GET_MODULE_HANDLE(game::client_module, "client.dll", _time);
	GET_MODULE_HANDLE(game::server_module, "server.dll", _time);

	if (MH_INIT_STATUS = MH_Initialize(); MH_INIT_STATUS != MH_STATUS::MH_OK)
	{
		goto INIT_FAIL;
	}

#ifdef DEBUG
	Beep(523, 100);
#endif
		
#ifdef GIT_DESCRIBE
	SetWindowTextA(main_window, utils::va("Portal 2 - RTX - %s", GIT_DESCRIBE));
#else
	SetWindowTextA(main_window, "Portal 2 - RTX");
#endif

	components::loader::initialize();
	return TRUE;

INIT_FAIL:
	XASSERT(MH_INIT_STATUS != MH_STATUS::MH_OK);
	Beep(300, 100); Sleep(100); Beep(200, 100); Sleep(100); Beep(100, 100);
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD  ul_reason_for_call, LPVOID /*lpReserved*/)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		CreateThread(nullptr, 0, find_window_loop, nullptr, 0, nullptr);
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		components::loader::uninitialize();
	}

	return TRUE;
}
