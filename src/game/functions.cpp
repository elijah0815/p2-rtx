#include "std_include.hpp"

namespace game
{
	std::vector<std::string> loaded_modules;
	DWORD shaderapidx9_module = 0u;
	DWORD studiorender_module = 0u;
	DWORD materialsystem_module = 0u;
	DWORD engine_module = 0u;
	DWORD client_module = 0u;
	DWORD server_module = 0u;

	const D3DXMATRIX IDENTITY =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	const D3DXMATRIX TC_TRANSLATE_TO_CENTER =
	{
		 1.0f,  0.0f, 0.0f, 0.0f,	// identity
		 0.0f,  1.0f, 0.0f, 0.0f,	// identity
		 0.0f,  0.0f, 1.0f, 0.0f,	// identity
		-0.5f, -0.5f, 0.0f, 1.0f,	// translate to center
	};

	const D3DXMATRIX TC_TRANSLATE_FROM_CENTER_TO_TOP_LEFT =
	{
		1.0f, 0.0f, 0.0f, 0.0f,	// identity
		0.0f, 1.0f, 0.0f, 0.0f,	// identity
		0.0f, 0.0f, 1.0f, 0.0f,	// identity
		0.5f, 0.5f, 0.0f, 1.0f,	// translate back to the top left corner
	};

	// adds a simple console command
	void con_add_command(ConCommand* cmd, const char* name, void(__cdecl* callback)(), const char* desc)
	{
		// #OFFSET - done
		// ConCommand *this, const char *pName, void (__cdecl *callback)(), const char *pHelpString, int flags, int (__cdecl *completionFunc)(const char *, char (*)[64]
		utils::hook::call<void(__fastcall)(ConCommand* this_ptr, void* null, const char*, void(__cdecl*)(), const char*, int, int(__cdecl*)(const char*, char(*)[64]))>(CLIENT_BASE + 0x631F00)
			(cmd, nullptr, name, callback, desc, 0x20000, nullptr);
	}

	/**
	 * Calls CDebugOverlay::AddTextOverlay
	 * @param pos		Position of text in 3D Space
	 * @param duration	Duration in which text is visible - use 0.0f for per frame stuff
	 * @param text		The text
	 */
	void debug_add_text_overlay(const float* pos, float duration, const char* text)
	{
		// #OFFSET - done
		utils::hook::call<void(__cdecl)(const float*, float, const char*)>(ENGINE_BASE + 0xC4640)
			(pos, duration, text);
	}

	// remove/destroy a given CBaseEntity
	void cbaseentity_remove(void* cbaseentity_ptr)
	{
		if (cbaseentity_ptr)
		{
			// #OFFSET - done
			// UTIL_Remove
			utils::hook::call<void(__cdecl)(void* cbaseentity)>(SERVER_BASE + 0x283770)(cbaseentity_ptr);
		}
	}

	// #OFFSET - done
	int get_visframecount() {
		return *reinterpret_cast<int*>(ENGINE_BASE + 0x6AAE6C);
	}

	// #OFFSET - done
	const char* get_map_name() {
		return utils::hook::call<const char*(__cdecl)()>(CLIENT_BASE + 0x1F4040)();
	}

}
