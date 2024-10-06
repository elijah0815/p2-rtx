#include "std_include.hpp"

// -novid -disable_d3d9_hacks -limitvsconst -disallowhwmorph -no_compressed_verts -maxdxlevel 90 -dxlevel 90 +sv_cheats 1 +developer 1 +cl_brushfastpath 0 +cl_modelfastpath 0 +r_ShowViewerArea 1 +mat_fullbright 1  +mat_queue_mode 0 +mat_softwarelighting 1 +mat_softwareskin 1 +mat_phong 1 +mat_parallaxmap 0 +mat_frame_sync_enable 0 +mat_fastnobump 1 +mat_disable_bloom 1 +mat_dof_enabled 0 +mat_displacementmap 0 +mat_drawflat 1 +mat_normalmaps 0 +mat_normals 0 +sv_lan 1 +map devtest

// dxlevel 100 required
// -novid -disable_d3d9_hacks -limitvsconst -softparticlesdefaultoff -disallowhwmorph -no_compressed_verts +sv_cheats 1 +developer 1 +r_ShowViewerArea 1 +cl_showpos 1 +r_PortalTestEnts 0 +portal_ghosts_disable 0 +r_portal_earlyz 0 +r_portal_use_complex_frustums 0 +r_portal_use_pvs_optimization 0 +r_portalstencildisable 0 +portal_stencil_depth 1 +portal_draw_ghosting 0 +r_staticprop_lod 0 +r_lod 0 +r_threaded_particles 0 +r_entityclips 0 +cl_brushfastpath 0 +cl_tlucfastpath 0 +cl_modelfastpath 0 +mat_fullbright 1 +mat_queue_mode 0 +mat_softwarelighting 0 +mat_softwareskin 1 +mat_phong 1 +mat_parallaxmap 0 +mat_frame_sync_enable 0 +mat_fastnobump 1 +mat_disable_bloom 1 +mat_dof_enabled 0 +mat_displacementmap 0 +mat_drawflat 1 +mat_normalmaps 0 +mat_normals 0 +sv_lan 1 +map sp_a2_bridge_intro

// r_PortalTestEnts			:: 0 = needed for anti culling of entities
// portal_ghosts_disable	:: 0 = okay until virtual instances are working (see cportalghost_should_draw)			|| 1 = disable rendering of ghost models
// r_portal_stencil_depth	:: 0 = okay as long most/all culling is disabled (may still produce missing surfaces)	|| 1 = might lower performance but should fix invisible surfaces


// cl_particles_show_bbox 1 can be used to see fx names
// +map sp_a1_intro2

// engine::Shader_WorldEnd interesting for sky

// Shader_DrawChains:: mat_forcedynamic 1 || mat_drawflat 1 => Shader_DrawChainsDynamic (changes hashes but still unstable)

namespace components
{
	template <std::size_t Index, typename ReturnType, typename... Args>
	__forceinline ReturnType call_virtual(void* instance, Args... args)
	{
		using Fn = ReturnType(__thiscall*)(void*, Args...);

		auto function = (*reinterpret_cast<Fn**>(instance))[Index];
		return function(instance, args...);
	}

	void rowMajorToColumnMajor(const float* rowMajor, float* columnMajor)
	{
		// Transpose the matrix by swapping the rows and columns
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				columnMajor[j * 4 + i] = rowMajor[i * 4 + j];
			}
		}
	}

	remixapi_LightHandle _handle = nullptr;


	void create_quad(remixapi_HardcodedVertex* v_out, uint32_t* i_out, const float scale)
	{
		if (!v_out || !i_out)
		{
			return;
		}

		auto makeVertex = [&](float x, float y, float z, float u, float v) {
			const remixapi_HardcodedVertex vert =
			{
			  .position = {x,y,z},
			  .normal = {0,0,-1},
			  .texcoord = { u, v },
			  .color = 0xFFFFFFFF,
			};
			return vert;
			};

		v_out[0] = makeVertex(-1.0f * scale, 1, -1.0f * scale, 0.0f, 0.0f); // b l
		v_out[1] = makeVertex(-1.0f * scale, 1, 1.0f * scale, 0.0f, 1.0f); // t l
		v_out[2] = makeVertex(1.0f * scale, 1, -1.0f * scale, 1.0f, 0.0f); // b r
		v_out[3] = makeVertex(1.0f * scale, 1, 1.0f * scale, 1.0f, 1.0f); // t r

		i_out[0] = 0;
		i_out[1] = 1;
		i_out[2] = 2;
		i_out[3] = 3;
		i_out[4] = 2;
		i_out[5] = 1;
	}

	void create_portal0()
	{
		if (!model_render::portal0_mtl)
		{
			//main_module::bridge.DestroyMaterial(portal0_mtl);
			remixapi_MaterialInfo info = {};
			{
				info.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO;
				info.hash = 10;
				info.emissiveIntensity = 0.0f;
				info.emissiveColorConstant = { 1.0f, 0.0f, 0.0f };
				info.albedoTexture = L"";
				info.normalTexture = L"";
				info.tangentTexture = L"";
				info.emissiveTexture = L"";

				info.spriteSheetFps = 5;
				info.spriteSheetCol = 1;
				info.spriteSheetRow = 1;
				info.filterMode = 1u;
				info.wrapModeU = 1u;
				info.wrapModeV = 1u;
			}

			remixapi_MaterialInfoPortalEXT ext = {};
			{
				ext.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO_PORTAL_EXT;
				ext.rayPortalIndex = 0;
				ext.rotationSpeed = 1.0f;
			}

			info.pNext = &ext;
			main_module::bridge.CreateMaterial(&info, &model_render::portal0_mtl);
		}

		// mesh

		remixapi_HardcodedVertex verts[4] = {};
		uint32_t indices[6] = {};
		create_quad(verts, indices, 50.0f);

		remixapi_MeshInfoSurfaceTriangles triangles = {
		  .vertices_values = verts,
		  .vertices_count = ARRAYSIZE(verts),
		  .indices_values = indices,
		  .indices_count = 6,
		  .skinning_hasvalue = FALSE,
		  .skinning_value = {},
		  .material = model_render::portal0_mtl,
		};

		remixapi_MeshInfo i = {
		  .sType = REMIXAPI_STRUCT_TYPE_MESH_INFO,
		  .hash = 20,
		  .surfaces_values = &triangles,
		  .surfaces_count = 1,
		};

		//main_module::bridge.DestroyMesh(model_render::portal0_mdl);
		main_module::bridge.CreateMesh(&i, &model_render::portal0_mdl);
	}

	void create_portal1()
	{
		if (!model_render::portal1_mtl)
		{
			//main_module::bridge.DestroyMaterial(portal0_mtl);
			remixapi_MaterialInfo info = {};
			{
				info.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO;
				info.hash = 30;
				info.emissiveIntensity = 0.0f;
				info.emissiveColorConstant = { 0.0f, 0.0f, 1.0f };
				info.albedoTexture = L"";
				info.normalTexture = L"";
				info.tangentTexture = L"";
				info.emissiveTexture = L"";

				info.spriteSheetFps = 5;
				info.spriteSheetCol = 1;
				info.spriteSheetRow = 1;
				info.filterMode = 1u;
				info.wrapModeU = 1u;
				info.wrapModeV = 1u;
			}

			remixapi_MaterialInfoPortalEXT ext = {};
			{
				ext.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO_PORTAL_EXT;
				ext.rayPortalIndex = 1;
				ext.rotationSpeed = 1.0f;
			}

			info.pNext = &ext;
			main_module::bridge.CreateMaterial(&info, &model_render::portal1_mtl);
		}

		// mesh

		remixapi_HardcodedVertex verts[4] = {};
		uint32_t indices[6] = {};
		create_quad(verts, indices, 50.0f);

		remixapi_MeshInfoSurfaceTriangles triangles = {
		  .vertices_values = verts,
		  .vertices_count = ARRAYSIZE(verts),
		  .indices_values = indices,
		  .indices_count = 6,
		  .skinning_hasvalue = FALSE,
		  .skinning_value = {},
		  .material = model_render::portal1_mtl,
		};

		remixapi_MeshInfo i = {
		  .sType = REMIXAPI_STRUCT_TYPE_MESH_INFO,
		  .hash = 40,
		  .surfaces_values = &triangles,
		  .surfaces_count = 1,
		};

		//main_module::bridge.DestroyMesh(model_render::portal1_mdl);
		main_module::bridge.CreateMesh(&i, &model_render::portal1_mdl);
	}

	void endscene_cb()
	{
		model_render::portal1_render_count = 0;
		model_render::portal2_render_count = 0;
#if 0
		{
			main_module::bridge.DestroyMesh(model_render::portal0_mdl);
			create_portal0();
		}

		{
			main_module::bridge.DestroyMesh(model_render::portal1_mdl);
			create_portal1();
		}

		if (model_render::portal0_mdl)
		{
			//remixapi_Transform t0 = {};
			main_module::portal0.matrix[0][0] = 1.0f;
			main_module::portal0.matrix[1][1] = 1.0f;
			main_module::portal0.matrix[2][2] = 1.0f;

			//main_module::portal0.matrix[0][3] = -1550.0f;
			//main_module::portal0.matrix[1][3] = 1715.0f;
			//main_module::portal0.matrix[2][3] = -255.0f;

			const remixapi_InstanceInfo info =
			{
				.sType = REMIXAPI_STRUCT_TYPE_MESH_INFO,
				.pNext = nullptr,
				.categoryFlags = 0,
				.mesh = model_render::portal0_mdl,
				.transform = main_module::portal0,
				.doubleSided = false
			};

			auto x = main_module::bridge.DrawInstance(&info);
			int z = 0;
		}

		if (model_render::portal1_mdl)
		{
			//remixapi_Transform t0 = {};
			main_module::portal1.matrix[0][0] = 1.0f;
			main_module::portal1.matrix[1][1] = 1.0f;
			main_module::portal1.matrix[2][2] = 1.0f;

			//main_module::portal1.matrix[0][3] = -1550.0f;
			//main_module::portal1.matrix[1][3] = 1590.0f;
			//main_module::portal1.matrix[2][3] = -255.0f;

			const remixapi_InstanceInfo info =
			{
				.sType = REMIXAPI_STRUCT_TYPE_MESH_INFO,
				.pNext = nullptr,
				.categoryFlags = 0,
				.mesh = model_render::portal1_mdl,
				.transform = main_module::portal1,
				.doubleSided = false
			};

			auto x = main_module::bridge.DrawInstance(&info);
			int z = 0;
		}
#endif

		if (!_handle)
		{
			auto ext = remixapi_LightInfoSphereEXT{
			.sType = REMIXAPI_STRUCT_TYPE_LIGHT_INFO_SPHERE_EXT,
			.pNext = nullptr,
			.position = remixapi_Float3D {.x = -1550.0f, .y = 1590.0f, .z = -250.0f},
			.radius = 1,
			.shaping_hasvalue = false,
			.shaping_value = {},
			};

			auto info = remixapi_LightInfo{
				.sType = REMIXAPI_STRUCT_TYPE_LIGHT_INFO,
				.pNext = &ext,
				.hash = 1234,
				.radiance = remixapi_Float3D {100, 20, 20},
			};

			main_module::bridge.CreateLight(&info, &_handle);
		}
		else
		{
			main_module::bridge.DrawLightInstance(_handle);
		}
	}

	// CViewRender::RenderView
	void xx(/*void* renderer*/)
	{
		if (static bool init_api = false; !init_api)
		{
			init_api = true;

			const auto status = remixapi::bridge_initRemixApi(&main_module::bridge);
			if (status == REMIXAPI_ERROR_CODE_SUCCESS)
			{
				main_module::m_initialized = true;
				//remixapi::bridge_setRemixApiCallbacks(nullptr, &endscene_cb, nullptr);
			}

			// init addon textures
			model_render::init_texture_addons();
		}

		endscene_cb();

		//void* enginerender = reinterpret_cast<void*>(ENGINE_BASE + 0x60F880);
		auto enginerender = game::get_engine_renderer();

		// old - works
		//const VMatrix* view = call_virtual<12, const VMatrix*>((void*)enginerender);
		//const VMatrix* pProjectionMatrix = view + 1; // Increment the pointer to get to the projectionMatrix

		//IDirect3DDevice9* dev = reinterpret_cast<IDirect3DDevice9*>(*(DWORD*)(RENDERER_BASE + 0x179F38));
		const auto dev = game::get_d3d_device();

		float colView[4][4] = {};
		rowMajorToColumnMajor(enginerender->m_matrixView.m[0], colView[0]);

		float colProj[4][4] = {};
		rowMajorToColumnMajor(enginerender->m_matrixProjection.m[0], colProj[0]);

		dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));
		dev->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(colView));
		dev->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(colProj));

		main_module::framecount++; // used for debug anim

		// reset this once every frame
		model_render::rendered_first_sky_surface = false;

		// set a default material with diffuse set to a warm white
		// so that add light to texture works and does not require rtx.effectLightPlasmaBall (animated)
		D3DMATERIAL9 dmat = {};
		dmat.Diffuse.r = 1.0f;
		dmat.Diffuse.g = 0.8f;
		dmat.Diffuse.b = 0.8f;
		dev->SetMaterial(&dmat);
	}

	HOOK_RETN_PLACE_DEF(yyy_retn);
	__declspec(naked) void yyy_stub()
	{
		__asm
		{
			call    edx; // og

			pushad;
			call	xx;
			popad;

			mov     ebx, [ebp + 8];
			jmp		yyy_retn;
		}
	}

#ifdef XXX
	void xx(void* renderer)
	{
		void* enginerender = reinterpret_cast<void*>(ENGINE_BASE + 0x60F880);

		const VMatrix* view = call_virtual<12, const VMatrix*>(enginerender);

		// Increment the pointer to get to the projectionMatrix
		const VMatrix* pProjectionMatrix = view + 1;

		//static DWORD* backEndDataOut_ptr = (DWORD*)(0x174F970);  // backendEndDataOut pointer
		//const auto out = reinterpret_cast<game::GfxBackEndData*>(*game::backEndDataOut_ptr);
		IDirect3DDevice9* dev = reinterpret_cast<IDirect3DDevice9*>(*(DWORD*)(RENDERER_BASE + 0x179F38));

		float colView[4][4] = {};
		rowMajorToColumnMajor(view[0][0], colView[0]);

		float colProj[4][4] = {};
		rowMajorToColumnMajor(pProjectionMatrix[0][0], colProj[0]);

		dev->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(colView));
		dev->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(colProj));

	}

	HOOK_RETN_PLACE_DEF(xxx_retn);
	__declspec(naked) void xxx_stub()
	{
		__asm
		{
			pushad;
			push	ecx;
			call	xx;
			add		esp, 4;
			popad;

			mov     edx, [ecx];
			mov     eax, [edx + 0x24];
			jmp		xxx_retn;
		}
	}
#endif

	/*void calc_player_view()
	{
		int x = 1;
	}

	HOOK_RETN_PLACE_DEF(pl_view_stub_retn);
	__declspec(naked) void calc_player_view_stub()
	{
		__asm
		{
			pushad;
			call	calc_player_view;
			popad;

			mov     edx, [esi];
			xorps   xmm0, xmm0;
			jmp		pl_view_stub_retn;
		}
	}*/

	// return nullptr a nullptr to skip rendering of ghost - return ent otherwise
	// - currently used to disable ghosts of chell and the world portalgun so that we can set 'portal_disable_ghosts' to 0
	// - useful if we move a cube through a portal
	// #TODO: fixme when remix virtual instances work?
	C_BaseEntity* cportalghost_should_draw(C_BaseEntity* ent)
	{
		if (ent && ent->model)
		{
			//const auto mdl_name = std::string_view(ent->model->szPathName);
			//if (mdl_name.contains("chell") || mdl_name.contains("portalgun"))
			if (ent->model->radius == 0.0f || ent->model->radius == 24.6587677f)
			{
				return nullptr; 
			}
		}

		return ent;
	}

	HOOK_RETN_PLACE_DEF(cportalghost_should_draw_retn);
	__declspec(naked) void cportalghost_should_draw_stub()
	{
		__asm
		{
			movss   dword ptr[esp], xmm0; // og

			mov		ecx, eax; // save og entity

			pushad;
			push	eax; // C_BaseEntity
			call	cportalghost_should_draw;
			add		esp, 4;

			test	eax, eax;
			jz		SKIP;
			popad;

			mov		eax, ecx; // restore og entity
			jmp		cportalghost_should_draw_retn; // continue normally

		SKIP:
			popad;
			add		esp, 4; // ecx was pushed 1 instr. before the hook
			xor		al, al;
			pop     esi;
			mov     esp, ebp;
			pop     ebp;
			retn;
		}
	}



	void on_set_pixelshader_warning()
	{
		int break_me = 0;
	}

	HOOK_RETN_PLACE_DEF(set_pixelshader_warning_retn);
	__declspec(naked) void set_pixelshader_warning_stub()
	{
		__asm
		{
			// og
			add     esp, 4;
			pop		edi;
			pop		esi;

			pushad;
			call	on_set_pixelshader_warning;
			popad;

			jmp		set_pixelshader_warning_retn;

		}
	}


	main_module::main_module()
	{
		/*HOOK_RETN_PLACE(pl_view_stub_retn, CLIENT_BASE + 0x4DA02);
		utils::hook(CLIENT_BASE + 0x4D9FD, calc_player_view_stub).install()->quick();*/

		// works but no gunmodel and broken stuff
#ifdef XXX
		utils::hook(ENGINE_BASE + 0xE6CF6, xxx_stub).install()->quick();
		HOOK_RETN_PLACE(xxx_retn, ENGINE_BASE + 0xE6CFB);
#endif

		utils::hook(CLIENT_BASE + 0x1ECB85, yyy_stub).install()->quick();
		HOOK_RETN_PLACE(yyy_retn, CLIENT_BASE + 0x1ECB8A);



		// anti cull:
		// 0x1D125D -> mov     byte ptr [edi+330h], 0 to 1 (cviewrenderer->m_bForceNoVis)
		// engine.dll + 0xE64E0 -> jmp - freeze
		// engine.dll + 0xE65DA -> nop 2

		//utils::hook::set<BYTE>(ENGINE_BASE + 0xE68F8, 0xEB);
		//utils::hook::nop(ENGINE_BASE + 0xE69C3, 2);

		// R_RecursiveWorldNode :: while (node->visframe == r_visframecount .. ) -> renders the entire map if everything after this is enabled
		utils::hook::nop(ENGINE_BASE + 0xE68EF, 6);

		// ^ :: while( ... node->contents < -1 .. ) -> jl to jle
		utils::hook::set<BYTE>(ENGINE_BASE + 0xE68F8, 0x7E);

		// ^ :: while( ... !R_CullNode) - needed for displacements
		utils::hook::nop(ENGINE_BASE + 0xE6905, 6);

		// ^ :: backface check -> je to jl
		// utils::hook::set<BYTE>(ENGINE_BASE + 0xE69C3, 0x7C); // this culls some visible surfs?
		utils::hook::nop(ENGINE_BASE + 0xE69C3, 2);

		// ^ :: backface check -> jnz to je
		utils::hook::set<BYTE>(ENGINE_BASE + 0xE69CD, 0x74);

		// R_DrawLeaf :: backface check (emissive lamps) plane normal >= -0.00999f
		utils::hook::nop(ENGINE_BASE + 0xE65C3, 6);

		// CClientLeafSystem::ExtractCulledRenderables :: disable 'engine->CullBox' check to disable entity culling in leafs
		// needs r_PortalTestEnts to be 0 -> je to jmp (0xEB)
		utils::hook::set<BYTE>(CLIENT_BASE + 0xDE4D5, 0xEB);

		// DrawDisplacementsInLeaf :: nop 'Frustum_t::CullBox' check to disable displacement (terrain) culling in leafs
		utils::hook::nop(ENGINE_BASE + 0xE6384, 2);

		// C_VGuiScreen::DrawModel :: vgui screens (world) :: nop C_VGuiScreen::IsBackfacing check
		utils::hook::nop(CLIENT_BASE + 0xCA14E, 2);


		// CSimpleWorldView::Setup :: nop 'DoesViewPlaneIntersectWater' check
		utils::hook::nop(CLIENT_BASE + 0x1E5693, 2);

		// ^ next instruction :: OR m_DrawFlags with 0x60 instead of 0x30
		utils::hook::set<BYTE>(CLIENT_BASE + 0x1E5695 + 6, 0x60);


		// C_Portal_Player::DrawModel :: disable 'C_Portal_Player::ShouldSkipRenderingViewpointPlayerForThisView' check to always render chell
		utils::hook::nop(CLIENT_BASE + 0x274FFB, 2);

		utils::hook(CLIENT_BASE + 0x27D4AC, cportalghost_should_draw_stub).install()->quick();
		HOOK_RETN_PLACE(cportalghost_should_draw_retn, CLIENT_BASE + 0x27D4B1);


		// CShaderManager::SetPixelShader :: disable warning print + place stub so we can break and see what type of shader is failing to load
		utils::hook::nop(RENDERER_BASE + 0x2AAB4, 6); // disable 'Trying to set a pixel shader that failed loading' print
		utils::hook(RENDERER_BASE + 0x2AABA, set_pixelshader_warning_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(set_pixelshader_warning_retn, RENDERER_BASE + 0x2AABF);
	}
}
