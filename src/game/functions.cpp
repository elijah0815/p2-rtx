#include "std_include.hpp"

namespace glob
{
	bool spawned_external_console = false;
}

namespace game
{
	std::vector<std::string> loaded_modules;
	std::string root_path;
	DWORD shaderapidx9_module = 0u;
	DWORD studiorender_module = 0u;
	DWORD materialsystem_module = 0u;
	DWORD engine_module = 0u;
	DWORD client_module = 0u;
	DWORD server_module = 0u;
	DWORD vstdlib_module = 0u;

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
		// ConCommand *this, const char *pName, void (__cdecl *callback)(), const char *pHelpString, int flags, int (__cdecl *completionFunc)(const char *, char (*)[64]
		utils::hook::call<void(__fastcall)(ConCommand* this_ptr, void* null, const char*, void(__cdecl*)(), const char*, int, int(__cdecl*)(const char*, char(*)[64]))>(CLIENT_BASE + USE_OFFSET(0x631F00, 0x6298D0))
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
		utils::hook::call<void(__cdecl)(const float*, float, const char*)>(ENGINE_BASE + USE_OFFSET(0xC4640, 0xC3FE0))
			(pos, duration, text);
	}

	// remove/destroy a given CBaseEntity
	void cbaseentity_remove(void* cbaseentity_ptr)
	{
		if (cbaseentity_ptr)
		{
			// UTIL_Remove
			utils::hook::call<void(__cdecl)(void* cbaseentity)>(SERVER_BASE + USE_OFFSET(0x283770, 0x27D690))(cbaseentity_ptr);
		}
	}

	int get_visframecount() {
		return *reinterpret_cast<int*>(ENGINE_BASE + USE_OFFSET(0x6AAE6C, 0x6A56B4));
	}

	const char* get_map_name() {
		return utils::hook::call<const char*(__cdecl)()>(CLIENT_BASE + USE_OFFSET(0x1F4040, 0x1EEEE0))();
	}

	void cvar_uncheat(const char* name)
	{
		if (const auto ivar = game::get_icvar(); ivar)
		{
			if (auto var = ivar->vftable->FindVar(ivar, name); var) {
				var->m_nFlags &= ~0x4000;
			}
		}
	}

	void cvar_uncheat_and_set_int(const char* name, const int val)
	{
		if (const auto ivar = game::get_icvar(); ivar)
		{
			if (auto var = ivar->vftable->FindVar(ivar, name); var)
			{
				var->vtbl->SetValue_Int(var, val);
				var->m_nFlags &= ~0x4000;
			}
		}
	}

	void cvar_uncheat_and_set_float(const char* name, const float val)
	{
		if (const auto ivar = game::get_icvar(); ivar)
		{
			if (auto var = ivar->vftable->FindVar(ivar, name); var)
			{
				var->vtbl->SetValue_Float(var, val);
				var->m_nFlags &= ~0x4000;
			}
		}
	}
}
