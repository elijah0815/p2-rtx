#pragma once

#define RENDERER_BASE			game::shaderapidx9_module
#define STUDIORENDER_BASE		game::studiorender_module
#define MATERIALSTYSTEM_BASE	game::materialsystem_module
#define ENGINE_BASE				game::engine_module
#define CLIENT_BASE				game::client_module
#define SERVER_BASE				game::server_module
#define VSTDLIB_BASE			game::vstdlib_module

using namespace components;

namespace game
{
	extern std::vector<std::string> loaded_modules;
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

	extern const char* get_map_name();
	extern int get_visframecount();

	extern void cvar_uncheat_and_set_int(const char* name, const int val);
	extern void cvar_uncheat_and_set_float(const char* name, const float val);

	// CRender gRender
	inline components::CRender* get_engine_renderer() { return reinterpret_cast<components::CRender*>(ENGINE_BASE + 0x615100); } // #OFFSET - done
	inline IDirect3DDevice9* get_d3d_device() { return reinterpret_cast<IDirect3DDevice9*>(*(DWORD*)(RENDERER_BASE + 0xDA5D8)); } // #OFFSET - done
	inline components::IShaderAPIDX8* get_shaderapi() { return reinterpret_cast<components::IShaderAPIDX8*>(*(DWORD*)(RENDERER_BASE + 0xD0C74)); } // #OFFSET - done
	inline components::CShaderAPIDx8* get_cshaderapi() { return reinterpret_cast<components::CShaderAPIDx8*>((RENDERER_BASE + 0xD7040)); } // #OFFSET - done
	inline components::worldbrushdata_t* get_hoststate_worldbrush_data() { return reinterpret_cast<components::CCommonHostState*>(ENGINE_BASE + 0x43F028)->worldbrush; } // #OFFSET - done
	inline components::CGlobalVarsBase* get_global_vars() { return reinterpret_cast<components::CGlobalVarsBase*>(*(DWORD*)(CLIENT_BASE + 0x92A37C)); } // #OFFSET - done
	//inline components::CAutoInitBasicPropPortalDrawingMaterials* base_portal_mats() { return reinterpret_cast<components::CAutoInitBasicPropPortalDrawingMaterials*>(CLIENT_BASE + 0x9FE710); } // #OFFSET - UNUSED
	inline components::CCvar* get_icvar() { return reinterpret_cast<components::CCvar*>((VSTDLIB_BASE + 0x315B0)); } // #OFFSET - done 

}
