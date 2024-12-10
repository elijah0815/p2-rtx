#pragma once

#define RENDERER_BASE			game::shaderapidx9_module
#define STUDIORENDER_BASE		game::studiorender_module
#define MATERIALSTYSTEM_BASE	game::materialsystem_module
#define ENGINE_BASE				game::engine_module
#define CLIENT_BASE				game::client_module
#define SERVER_BASE				game::server_module
#define VSTDLIB_BASE			game::vstdlib_module

using namespace components;

namespace glob
{
	extern bool spawned_external_console;
}

namespace game
{
	extern std::vector<std::string> loaded_modules;
	extern std::string root_path;
	extern DWORD shaderapidx9_module;
	extern DWORD studiorender_module;
	extern DWORD materialsystem_module;
	extern DWORD engine_module;
	extern DWORD client_module;
	extern DWORD server_module;
	extern DWORD vstdlib_module;

	extern const D3DXMATRIX IDENTITY;
	extern const D3DXMATRIX TC_TRANSLATE_TO_CENTER;
	extern const D3DXMATRIX TC_TRANSLATE_FROM_CENTER_TO_TOP_LEFT;

	extern void con_add_command(ConCommand* cmd, const char* name, void(__cdecl* callback)(), const char* desc);
	extern void debug_add_text_overlay(const float* pos, float duration, const char* text);

	extern void cbaseentity_remove(void* cbaseentity_ptr);

	extern int get_visframecount();
	extern const char* get_map_name();

	extern void r_flow_through_area(int area, const Vector* vec_vis_origin, const CPortalRect* clip_rect, const VisOverrideData_t* vis_data, float* reflection_water_height);
	inline int* get_visible_areas_num() { return reinterpret_cast<int*>(ENGINE_BASE + USE_OFFSET(0x61ABE0, 0x615468)); }
	inline std::uint16_t* get_visible_areas() { return reinterpret_cast<std::uint16_t*>(ENGINE_BASE + USE_OFFSET(0x61ABE8, 0x615470)); }
	inline bool* get_viewer_in_solid_space() { return reinterpret_cast<bool*>(ENGINE_BASE + USE_OFFSET(0x61C228, 0x616AB0)); }
	inline CPortalRect* get_area_rect() { return reinterpret_cast<CPortalRect*>(ENGINE_BASE + USE_OFFSET(0x61ADE8, 0x615670)); }
	inline Frustum_t* get_area_frustum() { return reinterpret_cast<Frustum_t*>(*(DWORD*)(ENGINE_BASE + USE_OFFSET(0x61C35C, 0x616BE4))); } // + 0x10 to pElements

	extern void frustum_set_planes(Frustum_t* frustum, const VPlane* planes);

	extern bool frustum_cull_box(Frustum_t* frustum, const Vector* mins, const Vector* maxs);
	inline Frustum_t* get_g_frustum() { return reinterpret_cast<Frustum_t*>(ENGINE_BASE + USE_OFFSET(0x615390, 0x60FC20)); }

	extern void cvar_uncheat(const char* name);
	extern void cvar_uncheat_and_set_int(const char* name, const int val);
	extern void cvar_uncheat_and_set_float(const char* name, const float val);

	extern void print_ingame(const char* msg, ...);

	// CRender gRender
	inline components::CRender* get_engine_renderer() { return reinterpret_cast<components::CRender*>(ENGINE_BASE + USE_OFFSET(0x615100, 0x60F880)); }
	inline IDirect3DDevice9* get_d3d_device() { return reinterpret_cast<IDirect3DDevice9*>(*(DWORD*)(RENDERER_BASE + USE_OFFSET(0xDA5D8, 0x179F38))); }
	inline components::IShaderAPIDX8* get_shaderapi() { return reinterpret_cast<components::IShaderAPIDX8*>(*(DWORD*)(RENDERER_BASE + USE_OFFSET(0xD0C74, 0x164C48))); }
	inline components::CShaderAPIDx8* get_cshaderapi() { return reinterpret_cast<components::CShaderAPIDx8*>((RENDERER_BASE + USE_OFFSET(0xD7040, 0x1769A0))); }
	inline components::worldbrushdata_t* get_hoststate_worldbrush_data() { return reinterpret_cast<components::CCommonHostState*>(ENGINE_BASE + USE_OFFSET(0x43F028, 0x439C1C))->worldbrush; }
	inline components::CGlobalVarsBase* get_global_vars() { return reinterpret_cast<components::CGlobalVarsBase*>(*(DWORD*)(CLIENT_BASE + USE_OFFSET(0x92A37C, 0x9220BC))); }
	inline components::CCvar* get_icvar() { return reinterpret_cast<components::CCvar*>((VSTDLIB_BASE + USE_OFFSET(0x315B0, 0x31550))); }

	inline Vector* get_current_view_origin() { return reinterpret_cast<Vector*>(ENGINE_BASE + USE_OFFSET(0x513380, 0x50DB50)); }
	inline Vector* get_current_view_forward() { return reinterpret_cast<Vector*>(ENGINE_BASE + USE_OFFSET(0x4351D0, 0x42FFE4)); }
	inline Vector* get_current_view_right() { return reinterpret_cast<Vector*>(ENGINE_BASE + USE_OFFSET(0x4351DC, 0x42FFF0)); }
	inline Vector* get_current_view_up() { return reinterpret_cast<Vector*>(ENGINE_BASE + USE_OFFSET(0x4351E8, 0x42FFFC)); }

	// note: this might be ILLEGAL when within 'CBaseWorldView::DrawSetup' -> use 'game::saved_view_id' instead
	inline view_id* get_current_view_id() { return reinterpret_cast<view_id*>(CLIENT_BASE + USE_OFFSET(0x937F40, 0x92FB00)); }
	extern view_id saved_view_id;

	inline bool is_puzzlemaker_active()
	{
		struct puzz
		{
			int pad;
			bool m_bShowing;
			bool m_bActive;
			float m_flTransition;
			float m_flZoomScale;
			bool m_bInputEnabled;
			int m_LastUpdateFrame;
			bool m_bIsInLevel;
			bool m_bCanQuitGame;
			int m_nCachedSSSlot;
		};

		const auto p = reinterpret_cast<puzz*>(*(DWORD*)(CLIENT_BASE + USE_OFFSET(0x94EAE8, 0x946438)));
		return p && p->m_bActive;
	}
	// 946438

	// CM_PointLeafnum
	inline int get_leaf_from_position(const Vector& pos) { return utils::hook::call<int(__cdecl)(const float*)>(ENGINE_BASE + USE_OFFSET(0x159C80, 0x158540))(&pos.x); }

	inline bool is_paused()
	{
		// GetBaseLocalClient
		const auto cclientstate_ptr = utils::hook::call<void*(__cdecl)()>(ENGINE_BASE + USE_OFFSET(0x9EAF0, 0x9E7E0))();

		// CClientState::IsPaused
		return utils::hook::call<BOOL(__fastcall)(void* this_ptr, void* null)>(ENGINE_BASE + USE_OFFSET(0xAB850, 0xAB140))(cclientstate_ptr, nullptr);
	}

	/**
	 * Creates an external console
	 */
	inline void console()
	{
		if (!glob::spawned_external_console)
		{
			glob::spawned_external_console = true;
			setvbuf(stdout, nullptr, _IONBF, 0);
			if (AllocConsole())
			{
				FILE* file = nullptr;
				freopen_s(&file, "CONIN$", "r", stdin);
				freopen_s(&file, "CONOUT$", "w", stdout);
				freopen_s(&file, "CONOUT$", "w", stderr);
				SetConsoleTitleA("P2-RTX Debug Console");
			}
		}
	}
}
