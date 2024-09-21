#include "std_include.hpp"

DWORD WINAPI find_window_loop(LPVOID)
{
	std::uint32_t _time = 0;
	HWND main_window = nullptr;

	// wait for window creation
	while (!main_window)
	{
		// get main window hwnd
		if (!main_window)
		{
			main_window = FindWindowA(nullptr, "Portal 2");
		}

		Sleep(100); _time += 100;

		if (_time >= 30000)
		{
			Beep(300, 100); Sleep(100); Beep(200, 100); Sleep(100); Beep(100, 100);
			return TRUE;
		}
	}

	// get render module (base address)
	if (!game::shaderapidx9)
	{
		game::shaderapidx9 = (DWORD)GetModuleHandleA("shaderapidx9.dll"); // RendDX9_r
	}

	Beep(523, 100);
	//Sleep(1000);

#ifdef GIT_DESCRIBE
	SetWindowTextA(main_window, utils::va("Portal 2 - RTX - %s", GIT_DESCRIBE));
#else
	SetWindowTextA(main_window, "Portal 2 - RTX");
#endif

	components::loader::initialize();
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
