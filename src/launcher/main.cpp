#include <iostream>
#include <windows.h>
#include <string>
#include <filesystem>
#include <version.hpp> // git version

# define ENDL "\n"

bool find_window_by_process_id(const DWORD proc_id)
{
	HWND hwnd = FindWindowA(nullptr, nullptr); // start with the first window
	while (hwnd != nullptr)
	{
		DWORD pid;
		GetWindowThreadProcessId(hwnd, &pid);

		if (pid == proc_id) {
			return true;
		}

		hwnd = GetNextWindow(hwnd, GW_HWNDNEXT); // move to the next window
	}

	return false;
} 

int wmain(int argc, wchar_t* argv[])
{
	std::filesystem::path game_path = std::filesystem::current_path();
	std::filesystem::path dll_path = game_path;
	dll_path.append("p2-rtx.dll");
	game_path.append("portal2.exe");

	if (!exists(game_path))
	{
		std::cout << "[Main] Could not find 'portal2.exe'. Path was: " << game_path.generic_string().c_str() << ENDL;
		system("pause");
		return -1;
	}

	if (!exists(dll_path))
	{
		std::cout << "[Main] Could not find 'p2-rtx.dll'. Path was: " << dll_path.generic_string().c_str() << ENDL;
		system("pause");
		return -1;
	}

	STARTUPINFOW si = { sizeof(STARTUPINFOW) };
	PROCESS_INFORMATION pi = {};

	// first arg has to be the executable name
	std::wstring command_line = L"portal2.exe -novid -disable_d3d9_hacks -limitvsconst -softparticlesdefaultoff -disallowhwmorph -no_compressed_verts +mat_phong 1";

	// get launcher arguments
	for (int i = 1; i < argc; ++i) // skip the launcher name
	{
		command_line += L" ";
		command_line += argv[i];
	}

	// create game process in a suspended state
	if (!CreateProcessW(game_path.c_str(), &command_line[0], nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi))
	{
		std::cout << "[Main] !CreateProcessW - Failed to launch portal2.exe" << ENDL;
		std::cout << "[Main] Paths:" << ENDL;
		std::cout << "[Main] > Game: " << game_path.generic_string().c_str() << ENDL;
		std::cout << "[Main] > DLL: " << dll_path.generic_string().c_str() << ENDL;
		system("pause");
		return -1;
	}

	bool error = false;

	{	// inject dll
		const auto path = dll_path.generic_string();

		if (LPVOID remote_memory = VirtualAllocEx(pi.hProcess, nullptr, path.size() + 1, MEM_COMMIT, PAGE_READWRITE);
			remote_memory)
		{
			// write the dll path into the allocated memory
			if (WriteProcessMemory(pi.hProcess, remote_memory, path.c_str(), path.size() + 1, nullptr))
			{
				// get the address of LoadLibraryA
				if (const auto loadlib_addr = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
					loadlib_addr)
				{
					// create a remote thread to call LoadLibraryA with the DLL path
					if (const auto thread = CreateRemoteThread(pi.hProcess, nullptr, 0, loadlib_addr, remote_memory, 0, nullptr);
						thread)
					{
						ResumeThread(pi.hThread);

						// Check if we want to debug the child process
						// Can not use WaitForObject as that would cause a deadlock with child process debugging
						if (IsDebuggerPresent())
						{
							Sleep(50);
							std::uint32_t time = 0;

							bool process_spawned = false;
							while (!process_spawned)
							{
								process_spawned = find_window_by_process_id(pi.dwProcessId);
								if (!process_spawned)
								{
									Sleep(50);
									time += 50;

									if (time >= 30000)
									{
										error = true;
										std::cout << "[Debug] Failed to find spawned process!" << ENDL;
										break;
									}
								}
							}
						}
						else { // normal startup
							WaitForSingleObject(thread, INFINITE);
						}

						CloseHandle(thread);
					}
					else
					{
						error = true;
						std::cout << "[Inject] !thread" << ENDL;
						std::cout << "[Inject] Path was: " << path.c_str() << ENDL;
					}
				}
				else
				{
					error = true;
					std::cout << "[Inject] !loadlib_addr" << ENDL;
					std::cout << "[Inject] Path was: " << path.c_str() << ENDL;
				}
			}
			else
			{
				error = true;
				std::cout << "[Inject] !WriteProcessMemory" << ENDL;
				std::cout << "[Inject] Path was: " << path.c_str() << ENDL;
			}

			VirtualFreeEx(pi.hProcess, remote_memory, 0, MEM_RELEASE);
		}
		else
		{
			error = true;
			std::cout << "[Inject] !remote_memory" << ENDL;
			std::cout << "[Inject] Path was: " << path.c_str() << ENDL;
		}
	}

	// clean-up
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	if (error) {
		system("pause");
	}
	
	return 0;
}