#include "std_include.hpp"

// + dxlevel 100 required

// commandline args:
// -novid -height 1060 -disable_d3d9_hacks -limitvsconst -softparticlesdefaultoff -disallowhwmorph -no_compressed_verts +sv_cheats 1 +developer 1 +r_ShowViewerArea 1 +cl_showpos 1 +r_PortalTestEnts 0 +portal_ghosts_disable 0 +r_portal_earlyz 0 +r_portal_use_complex_frustums 0 +r_portal_use_pvs_optimization 0 +r_portalstencildisable 1 +r_portal_stencil_depth 0 +portal_draw_ghosting 0 +r_staticprop_lod 0 +r_lod 0 +r_threaded_particles 0 +r_entityclips 0 +cl_brushfastpath 0 +cl_tlucfastpath 0 +cl_modelfastpath 0 +mat_fullbright 1 +mat_queue_mode 0 +mat_softwarelighting 0 +mat_softwareskin 1 +mat_phong 1 +mat_parallaxmap 0 +mat_frame_sync_enable 0 +mat_fastnobump 1 +mat_disable_bloom 1 +mat_dof_enabled 0 +mat_displacementmap 0 +mat_drawflat 0 +mat_normalmaps 0 +mat_normals 0 +r_3dsky 0 +sv_lan 1 +map sp_a1_wakeup
// -novid -disable_d3d9_hacks -limitvsconst -disallowhwmorph -softparticlesdefaultoff -no_compressed_verts +mat_phong 1


// *** Required cvars
// r_PortalTestEnts					:: 0 = needed for anti culling of entities
// portal_ghosts_disable			:: 0 = okay until virtual instances are working (see cportalghost_should_draw)			|| 1 = disable rendering of ghost models
// r_portal_stencil_depth			:: 0 = don't let the game render the scene multiple times - portal vis now handled in 'viewdrawscene_custom_portal_vis'

// *** Useful cvars
// mat_leafvis 1					:: print current leaf and area index to console
// r_ShowViewerArea 1				:: show current area on the HUD
// xo_debug_toggle_node_vis (cmd)	:: toggle debug vis of the leaf/node the player is currently in
// r_drawmodelstatsoverlay 1		:: show model names

// *** Other cvars
// r_novis							:: 1 = disable all visleaf/node checks (renders the entire map - same as 'xo_disable_all_culling' cmdline flag)
// cl_particles_show_bbox 1			:: can be used to see fx names


// engine::Shader_WorldEnd interesting for sky
// Shader_DrawChains:: mat_forcedynamic 1 || mat_drawflat 1 => Shader_DrawChainsDynamic (changes hashes but still unstable)

namespace components
{
	int g_player_current_node = -1;
	int g_player_current_leaf = -1;
	int g_player_current_area = -1;

	namespace api
	{
		bool m_initialized = false;
		remixapi_Interface bridge = {};

		remixapi_MaterialHandle remix_debug_line_materials[3];
		remixapi_MeshHandle remix_debug_line_list[128] = {};
		std::uint32_t remix_debug_line_amount = 0u;
		std::uint64_t remix_debug_last_line_hash = 0u;
		bool remix_debug_node_vis = false; // show/hide debug vis of bsp nodes/leafs

		// forward declaration
		void endscene_cb();

		// called on device->Present
		void on_present_cb()
		{
			// Draw current node/leaf as HUD
			if (api::remix_debug_node_vis && main_module::d3d_font)
			{
				RECT rect;

				if (!map_settings::get_map_name().empty())
				{
					SetRect(&rect, 20, 100, 512, 512);
					main_module::d3d_font->DrawTextA
					(
						nullptr,
						map_settings::get_map_name().c_str(),
						-1,       // text length (-1 = null-terminated)
						&rect,
						DT_NOCLIP,
						D3DCOLOR_XRGB(255, 255, 255)
					);
				}

				if (g_player_current_area != -1)
				{
					SetRect(&rect, 20, 125, 512, 512);
					auto text = utils::va("Area: %d", g_player_current_area);
					main_module::d3d_font->DrawTextA
					(
						nullptr,
						text,
						-1,       // text length (-1 = null-terminated)
						&rect,
						DT_NOCLIP,
						D3DCOLOR_XRGB(255, 255, 255)
					);
				}

				if (g_player_current_leaf != -1)
				{
					SetRect(&rect, 20, 145, 512, 512);
					auto text = utils::va("Leaf: %d", g_player_current_leaf);
					main_module::d3d_font->DrawTextA
					(
						nullptr,
						text,
						-1,       // text length (-1 = null-terminated)
						&rect,
						DT_NOCLIP,
						D3DCOLOR_XRGB(50, 255, 20)
					);
				}

				//if (player_current_node != -1)
				//{
				//	SetRect(&rect, 10, 140, 512, 512);
				//	auto text = utils::va("Node: %d", player_current_node);
				//	main_module::d3d_font->DrawTextA
				//	(
				//		nullptr,
				//		text,
				//		-1,       // text length (-1 = null-terminated)
				//		&rect,
				//		DT_NOCLIP,
				//		D3DCOLOR_XRGB(0, 255, 255)
				//	);
				//}
			}
		}

		// called once from the main_module constructor
		void init()
		{
			const auto status = remixapi::bridge_initRemixApi(&api::bridge);
			if (status == REMIXAPI_ERROR_CODE_SUCCESS)
			{
				m_initialized = true;
				remixapi::bridge_setRemixApiCallbacks(endscene_cb, nullptr, on_present_cb);
			}
		}

		void create_quad(remixapi_HardcodedVertex* v_out, uint32_t* i_out, const float scale)
		{
			if (!v_out || !i_out)
			{
				return;
			}

			auto make_vertex = [&](float x, float y, float z, float u, float v)
			{
				const remixapi_HardcodedVertex vert =
				{
				  .position = { x, y, z },
				  .normal = { 0.0f, 0.0f, -1.0f },
				  .texcoord = { u, v },
				  .color = 0xFFFFFFFF,
				};
				return vert;
			};

			v_out[0] = make_vertex(-1.0f * scale, 1, -1.0f * scale, 0.0f, 0.0f); // bottom left
			v_out[1] = make_vertex(-1.0f * scale, 1, 1.0f * scale, 0.0f, 1.0f); // top left
			v_out[2] = make_vertex(1.0f * scale, 1, -1.0f * scale, 1.0f, 0.0f); // bottom right
			v_out[3] = make_vertex(1.0f * scale, 1, 1.0f * scale, 1.0f, 1.0f); // top right

			i_out[0] = 0; i_out[1] = 1; i_out[2] = 2;
			i_out[3] = 3; i_out[4] = 2; i_out[5] = 1;
		}

		void create_line_quad(remixapi_HardcodedVertex* v_out, uint32_t* i_out, const Vector& p1, const Vector& p2, const float width)
		{
			if (!v_out || !i_out)
			{
				return;
			}

			auto make_vertex = [&](const Vector& pos, float u, float v)
				{
					const remixapi_HardcodedVertex vert =
					{
					  .position = { pos.x, pos.y, pos.z },
					  .normal = { 0.0f, 0.0f, -1.0f },
					  .texcoord = { u, v },
					  .color = 0xFFFFFFFF,
					};
					return vert;
				};

			Vector up = { 0.0f, 0.0f, 1.0f };

			// dir of the line
			Vector dir = p2 - p1;
			dir.Normalize();

			// check if dir is parallel or very close to the up vector
			if (fabs(DotProduct(dir, up)) > 0.999f)
			{
				// if parallel, choose a different up vector
				up = { 1.0f, 0.0f, 0.0f };
			}

			Vector perp = dir.Cross(up); // perpendicular vector to line
			perp.Normalize(); // unit length

			// scale by half width to offset vertices
			const Vector offset = perp * (width * 0.5f);

			v_out[0] = make_vertex(p1 - offset, 0.0f, 0.0f); // bottom left
			v_out[1] = make_vertex(p1 + offset, 0.0f, 1.0f); // top left
			v_out[2] = make_vertex(p2 - offset, 1.0f, 0.0f); // bottom right
			v_out[3] = make_vertex(p2 + offset, 1.0f, 1.0f); // top right

			i_out[0] = 0; i_out[1] = 1; i_out[2] = 2;
			i_out[3] = 3; i_out[4] = 2; i_out[5] = 1;
		}

		void add_debug_line(const Vector& p1, const Vector& p2, const float width, DEBUG_REMIX_LINE_COLOR color)
		{
			if (remix_debug_line_materials[color])
			{
				remix_debug_line_amount++;
				remixapi_HardcodedVertex verts[4] = {};
				uint32_t indices[6] = {};
				api::create_line_quad(verts, indices, p1, p2, width);

				remixapi_MeshInfoSurfaceTriangles triangles =
				{
				  .vertices_values = verts,
				  .vertices_count = ARRAYSIZE(verts),
				  .indices_values = indices,
				  .indices_count = 6,
				  .skinning_hasvalue = FALSE,
				  .skinning_value = {},
				  .material = remix_debug_line_materials[color],
				};

				remixapi_MeshInfo info
				{
					.sType = REMIXAPI_STRUCT_TYPE_MESH_INFO,
					.hash = utils::string_hash64(utils::va("line%d", remix_debug_last_line_hash ? remix_debug_last_line_hash : 1)),
					.surfaces_values = &triangles,
					.surfaces_count = 1,
				};

				api::bridge.CreateMesh(&info, &remix_debug_line_list[remix_debug_line_amount]);
				remix_debug_last_line_hash = reinterpret_cast<std::uint64_t>(remix_debug_line_list[remix_debug_line_amount]);
			}
		}

		// called on device->EndScene
		void endscene_cb()
		{
			if (api::remix_debug_line_amount)
			{
				for (auto l = 1u; l < api::remix_debug_line_amount + 1; l++)
				{
					if (api::remix_debug_line_list[l])
					{
						remixapi_Transform t0 = {};
						t0.matrix[0][0] = 1.0f;
						t0.matrix[1][1] = 1.0f;
						t0.matrix[2][2] = 1.0f;

						const remixapi_InstanceInfo inst =
						{
							.sType = REMIXAPI_STRUCT_TYPE_INSTANCE_INFO,
							.pNext = nullptr,
							.categoryFlags = 0,
							.mesh = api::remix_debug_line_list[l],
							.transform = t0,
							.doubleSided = true
						};

						api::bridge.DrawInstance(&inst);
					}
				}
			}

			// reset first
			for (auto i = 0u; i < 4; i++)
			{
				auto& p = model_render::game_portals[i];
				p.portal = nullptr;
				p.portal_owner = nullptr;
				p.is_linked = false;
			}
		}
	}

	// #
	// #


	/**
	 * Called from CViewRender::RenderView
	 * - Pass world, view, projection to D3D
	 * - Other misc. that runs once per frame
	 */
	void on_renderview()
	{
		auto enginerender = game::get_engine_renderer();
		const auto dev = game::get_d3d_device();

		// setup main camera
		{
			float colView[4][4] = {};
			utils::row_major_to_column_major(enginerender->m_matrixView.m[0], colView[0]);

			float colProj[4][4] = {};
			utils::row_major_to_column_major(enginerender->m_matrixProjection.m[0], colProj[0]);

			dev->SetTransform(D3DTS_WORLD, &game::IDENTITY);
			dev->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(colView));
			dev->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(colProj));

			// set a default material with diffuse set to a warm white
			// so that add light to texture works and does not require rtx.effectLightPlasmaBall (animated)
			D3DMATERIAL9 dmat = {};
			dmat.Diffuse.r = 1.0f;
			dmat.Diffuse.g = 0.8f;
			dmat.Diffuse.b = 0.8f;
			dev->SetMaterial(&dmat);
		}

		if (!game::is_paused()) {
			main_module::framecount++; // used for debug anim
		}


		// ----
		// ----

		choreo_events::on_client_frame();
		api::remix_vars::on_client_frame();
		api::remix_lights::on_client_frame();

		// force cvars per frame just to make sure
		main_module::setup_required_cvars();

		// TODO - find better spot to call this
		map_settings::spawn_markers_once();

		// fog
		if (static bool allow_fog = !flags::has_flag("no_fog"); allow_fog)
		{
			const auto& s = map_settings::get_map_settings();
			if (s.fog_dist > 0.0f)
			{
				const float fog_start = 1.0f; // not useful
				dev->SetRenderState(D3DRS_FOGENABLE, TRUE);
				dev->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
				dev->SetRenderState(D3DRS_FOGSTART, *(DWORD*)&fog_start);
				dev->SetRenderState(D3DRS_FOGEND, *(DWORD*)&s.fog_dist);
				dev->SetRenderState(D3DRS_FOGCOLOR, s.fog_color);
			}
			else
			{
				dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
			}
		}

		// api debug lines
		if (!api::remix_debug_line_materials[0])
		{
			remixapi_MaterialInfo info
			{
				.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO,
				.hash = utils::string_hash64("linemat0"),
				.albedoTexture = L"",
				.normalTexture = L"",
				.tangentTexture = L"",
				.emissiveTexture = L"",
				.emissiveIntensity = 1.0f,
				.emissiveColorConstant = { 1.0f, 0.0f, 0.0f },
			};

			info.albedoTexture = L"";
			info.normalTexture = L"";
			info.tangentTexture = L"";
			info.emissiveTexture = L"";

			remixapi_MaterialInfoOpaqueEXT ext = {};
			{
				ext.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO_OPAQUE_EXT;
				ext.useDrawCallAlphaState = 1;
				ext.opacityConstant = 1.0f;
				ext.roughnessTexture = L"";
				ext.metallicTexture = L"";
				ext.heightTexture = L"";
		}

			info.pNext = &ext;

			api::bridge.CreateMaterial(&info, &api::remix_debug_line_materials[0]);

			info.hash = utils::string_hash64("linemat1");
			info.emissiveColorConstant = { 0.0f, 1.0f, 0.0f };
			api::bridge.CreateMaterial(&info, &api::remix_debug_line_materials[1]);

			info.hash = utils::string_hash64("linemat3");
			info.emissiveColorConstant = { 0.0f, 1.0f, 1.0f };
			api::bridge.CreateMaterial(&info, &api::remix_debug_line_materials[2]);
	}

		// destroy all lines added the prev. frame
		if (api::remix_debug_line_amount)
		{
			for (auto& line : api::remix_debug_line_list)
			{
				if (line)
				{
					api::bridge.DestroyMesh(line);
					line = nullptr;
				}
			}

			api::remix_debug_line_amount = 0;
		}

#if defined(BENCHMARK)
		if (model_render::m_benchmark.enabled && !model_render::m_benchmark.material_name.empty())
		{
			const auto con = GetStdHandle(STD_OUTPUT_HANDLE);

			SetConsoleTextAttribute(con, FOREGROUND_RED);
			printf("[ %.3f ms ]\t vertex format [ 0x%llx ] using material [ %s ] - Longest\n",
				model_render::m_benchmark.ms, model_render::m_benchmark.vertex_format, model_render::m_benchmark.material_name.c_str());

			SetConsoleTextAttribute(con, FOREGROUND_RED | FOREGROUND_INTENSITY);
			printf("[ %.3f ms ]\t frame total \n\n", model_render::m_benchmark.ms_total);
			SetConsoleTextAttribute(con, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

			// bench one frame only
			model_render::m_benchmark.enabled = false;
			model_render::m_benchmark.clear();
		}
#endif

		// needs portal fade-in effect fix:
		// https://github.com/NVIDIAGameWorks/dxvk-remix/pull/83
		if (!api::remix_rayportal::get()->empty())
		{
			api::remix_rayportal::get()->draw_all_pairs();
		}

		api::remix_lights::on_client_frame();
	}

	HOOK_RETN_PLACE_DEF(cviewrenderer_renderview_retn);
	__declspec(naked) void cviewrenderer_renderview_stub()
	{
		__asm
		{
			mov     dword ptr[ebp - 0xC], 0; // og
			
			pushad;
			call	on_renderview;
			popad;

			jmp		cviewrenderer_renderview_retn;
		}
	}


	void cviewrenderer_drawonemonitor_hk()
	{
		auto enginerender = game::get_engine_renderer();

		// old - works
		//const VMatrix* view = call_virtual<12, const VMatrix*>((void*)enginerender);
		//const VMatrix* pProjectionMatrix = view + 1; // Increment the pointer to get to the projectionMatrix

		const auto dev = game::get_d3d_device();

		float colView[4][4] = {};
		utils::row_major_to_column_major(enginerender->m_matrixView.m[0], colView[0]);

		float colProj[4][4] = {};
		utils::row_major_to_column_major(enginerender->m_matrixProjection.m[0], colProj[0]);

		dev->SetTransform(D3DTS_WORLD, &game::IDENTITY);
		dev->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(colView));
		dev->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(colProj));
	}

	HOOK_RETN_PLACE_DEF(cviewrenderer_drawonemonitor_retn);
	__declspec(naked) void cviewrenderer_drawonemonitor_stub()
	{
		__asm
		{
			pushad;
			call	cviewrenderer_drawonemonitor_hk;
			popad;

			// og
			mov     ebx, [ebp - 0xC];
			push    0;
			jmp		cviewrenderer_drawonemonitor_retn;
		}
	}


	
	
#if 0
	void on_cl_init_hk()
	{
		main_module::setup_required_cvars();
	}

	__declspec(naked) void on_cl_init_stub()
	{
		__asm
		{
			pushad;
			call	on_cl_init_hk;
			popad;

			// og
			mov     esp, ebp;
			pop     ebp;
			retn;
		}
	}
#endif

	// #
	// #

	/**
	 * Called from CModelLoader::Map_LoadModel
	 * @param map_name  Name of loading map
	 */
	void on_map_load_hk(const char* map_name)
	{
		// ALL RESETS first because 'on_host_disconnect_hk' is not reliable
		choreo_events::reset_all();

		model_render::vgui_progress_board_scalar = 1.0f;
		model_render::linked_area_portals.clear();

		// -------

		api::remix_vars::on_map_load();
		map_settings::on_map_load(map_name);
		main_module::setup_required_cvars();

		// reset portal vars
		for (auto i = 0u; i < 4; i++)
		{
			auto* p = &model_render::game_portals[i];
			p->portal = nullptr;
			p->open_amount = 0.0f;
			p->portal_owner = nullptr;
			p->is_linked = false;
		}
	}

	HOOK_RETN_PLACE_DEF(on_map_load_stub_retn);
	__declspec(naked) void on_map_load_stub()
	{
		__asm
		{
			pushad;
			push    eax;
			call	on_map_load_hk;
			add		esp, 4;
			popad;

			// og
			lea     ecx, [edi + 0x68];
			xor		edx, edx;
			jmp		on_map_load_stub_retn;

		}
	}


	/**
	 * Called from Host_Disconnect
	 * on: disconnect, restart, killserver, stopdemo ...
	 */
	void on_host_disconnect_hk()
	{
		// #TODO: not called when traversing maps via the elevator ...

		/*map_settings::on_map_exit(); 
		events::s_ent.reset();
		model_render::linked_area_portals.clear();*/
	}

	HOOK_RETN_PLACE_DEF(on_host_disconnect_retn);
	__declspec(naked) void on_host_disconnect_stub()
	{
		__asm
		{
			pushad;
			call	on_host_disconnect_hk;
			popad;

			// og
			mov     ebp, esp;
			sub     esp, 0x10;
			jmp		on_host_disconnect_retn;
		}
	}


	// #
	// #

	// Return nullptr a nullptr to skip rendering of ghost - return ent otherwise
	// - currently used to disable ghosts of chell and the world portalgun so that we can set 'portal_disable_ghosts' to 0
	// - useful if we move a cube through a portal
	// #TODO: fixme when remix virtual instances work?
	C_BaseEntity* cportalghost_should_draw(C_BaseEntity* ent)
	{
		int player_team_num = 0;

		const auto base_player = utils::hook::call<C_BaseEntity*(__cdecl)()>(CLIENT_BASE + USE_OFFSET(0x17B5F0, 0x176460))(); // GetSplitScreenViewPlayer
		if (base_player)
		{
			const auto portal_player = utils::hook::call<void* (__cdecl)(C_BaseEntity*)>(CLIENT_BASE + USE_OFFSET(0x3FDD0, 0x14BF30))(base_player); // ToPortalPlayer
			if (portal_player)
			{
				player_team_num = base_player->m_iTeamNum;
			}
		}

		// teamnum 2 = eggbot
		// teamnum 3 = ballbot

		if (ent && ent->model)
		{
			if (ent->model->radius == 0.0f // chell
				|| (ent->model->radius == 24.6587677f) // portalgun
				|| (ent->model->radius == 74.0151749f && ent->m_iTeamNum == player_team_num) // models/player/ballbot/ballbot.mdl
				|| (ent->model->radius == 82.9798126f && ent->m_iTeamNum == player_team_num) // models/player/eggbot/eggbot.mdl
				|| (ent->model->radius == 76.5784531f) // models/info_character/info_character_player.mdl
				) {
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


	// #
	// #

	void on_set_pixelshader_warning()
	{
#ifdef DEBUG
		int break_me = 0;
#endif
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




	/**
	 * Draw a wireframe box using the remix api
	 * @param center		Center of the cube
	 * @param half_diagonal Half diagonal distance of the box
	 * @param width			Line width
	 * @param color			Line color
	 */
	void main_module::debug_draw_box(const VectorAligned& center, const VectorAligned& half_diagonal, const float width, const api::DEBUG_REMIX_LINE_COLOR& color)
	{
		Vector min, max;
		Vector corners[8];

		// calculate min and max positions based on the center and half diagonal
		min = center - half_diagonal;
		max = center + half_diagonal;

		// get the corners of the cube
		corners[0] = Vector(min.x, min.y, min.z);
		corners[1] = Vector(min.x, min.y, max.z);
		corners[2] = Vector(min.x, max.y, min.z);
		corners[3] = Vector(min.x, max.y, max.z);
		corners[4] = Vector(max.x, min.y, min.z);
		corners[5] = Vector(max.x, min.y, max.z);
		corners[6] = Vector(max.x, max.y, min.z);
		corners[7] = Vector(max.x, max.y, max.z);

		// define the edges
		Vector lines[12][2];
		lines[0][0]  = corners[0];	lines[0][1]  = corners[1]; // Edge 1
		lines[1][0]  = corners[0];	lines[1][1]  = corners[2]; // Edge 2
		lines[2][0]  = corners[0];	lines[2][1]  = corners[4]; // Edge 3
		lines[3][0]  = corners[1];	lines[3][1]  = corners[3]; // Edge 4
		lines[4][0]  = corners[1];	lines[4][1]  = corners[5]; // Edge 5
		lines[5][0]  = corners[2];	lines[5][1]  = corners[3]; // Edge 6
		lines[6][0]  = corners[2];	lines[6][1]  = corners[6]; // Edge 7
		lines[7][0]  = corners[3];	lines[7][1]  = corners[7]; // Edge 8
		lines[8][0]  = corners[4];	lines[8][1]  = corners[5]; // Edge 9
		lines[9][0]  = corners[4];	lines[9][1]  = corners[6]; // Edge 10
		lines[10][0] = corners[5];	lines[10][1] = corners[7]; // Edge 11
		lines[11][0] = corners[6];	lines[11][1] = corners[7]; // Edge 12

		for (auto e = 0u; e < 12; e++) {
			api::add_debug_line(lines[e][0], lines[e][1], width, color);
		}
	}

	/**
	 * Force visibility of a specific node
	 * @param node_index	The node to force vis for
	 * @param player_node	The node the player is currently in
	 */
	void force_node_vis(int node_index, mnode_t* player_node)
	{
		const auto world = game::get_hoststate_worldbrush_data();
		const auto root_node = &world->nodes[0];

		int next_node_index = node_index;
		while (next_node_index >= 0)
		{
			const auto node = &world->nodes[next_node_index];

			// force node vis
			node->visframe = game::get_visframecount();

			// we only need to traverse to the root node or the player node
			if (node == player_node || node == root_node) {
				break;
			}

			next_node_index = &node->parent[0] - root_node;
		}
	}

	/**
	 * Force visibility of a specific leaf
	 * @param leaf_index   The leaf to force vis for
	 * @param player_node  The node the player is currently in
	 */
	void force_leaf_vis(int leaf_index, mnode_t* player_node)
	{
		const auto world = game::get_hoststate_worldbrush_data();
		auto leaf_node = &world->leafs[leaf_index];
		auto parent_node_index = &leaf_node->parent[0] - &world->nodes[0];

		// force leaf vis
		leaf_node->visframe = game::get_visframecount();

		// force nodes
		force_node_vis(parent_node_index, player_node);
	}

	// Called once before 'R_RecursiveWorldNode' is getting called for the first time
	void pre_recursive_world_node()
	{
		const auto world = game::get_hoststate_worldbrush_data();

		// CM_PointLeafnum :: get current leaf
		const auto current_leaf = game::get_leaf_from_position(game::get_current_view_origin());
		//utils::hook::call<int(__cdecl)(const float*)>(ENGINE_BASE + USE_OFFSET(0x159C80, 0x158540))(game::get_current_view_origin());

		// CM_LeafArea :: get current player area
		const auto current_area = utils::hook::call<int(__cdecl)(int leafnum)>(ENGINE_BASE + USE_OFFSET(0x15ACE0, 0x159470))(current_leaf);

		// only run logic when the leaf changes
		const bool leaf_update = current_leaf != g_player_current_leaf;

		// only run logic when the area changes
		const bool area_update = current_leaf != g_player_current_area;

		g_player_current_area = current_area;
		g_player_current_node = -1;
		g_player_current_leaf = -1;

		auto& map_settings = map_settings::get_map_settings();

		// find the bsp node the player is currently in + visualize node / leaf using the remix api
		// - skip if map settings contains no overrides for current map and debug vis is not active
#if 0
		if ((map_settings && map_settings->area_settings.contains(current_area)) || api::remix_debug_node_vis)
		{
			int node_index = 0, leaf_index = 0;
			while (node_index >= 0)
			{
				const auto node = &world->nodes[node_index];

				// check which child we should go to based on the players position
				float dist = DotProduct(g_CurrentViewOrigin, node->plane->normal) - node->plane->dist;

				// if the player is on the front side of the plane
				if (dist > 0)
				{
					auto next_node_index = node->children[0] - &world->nodes[0]; // minus root node to get the index
					if (next_node_index < 0)
					{
						leaf_index = reinterpret_cast<mleaf_t*>(node->children[0]) - &world->leafs[0];
						break;
					}
					node_index = next_node_index; // go to the front child
				}
				else
				{
					auto next_node_index = node->children[1] - &world->nodes[0]; // minus root node to get the index
					if (next_node_index < 0)
					{
						leaf_index = reinterpret_cast<mleaf_t*>(node->children[1]) - &world->leafs[0];
						break;
					}
					node_index = next_node_index; // go to the back child
				}
			}

			// show leaf index as 3D text
			if (leaf_index < world->numleafs)
			{
				player_current_leaf = leaf_index;

				if (api::remix_debug_node_vis)
				{
					const auto curr_leaf = &world->leafs[leaf_index];
					game::debug_add_text_overlay(&curr_leaf->m_vecCenter.x, 0.0f, utils::va("Leaf: %i", leaf_index));
					main_module::debug_draw_box(curr_leaf->m_vecCenter, curr_leaf->m_vecHalfDiagonal, 2.0f, api::DEBUG_REMIX_LINE_COLOR::GREEN);
				}
			}

			/* // not that useful
			// show node index as 3D text
			if (node_index < world->numnodes)
			{
				player_current_node = node_index;

				if (api::remix_debug_node_vis)
				{
					const auto node = &world->nodes[node_index];
					Vector pos_copy = node->m_vecCenter;
					pos_copy.z += 2.0f;

					game::debug_add_text_overlay(&pos_copy.x, 0.0f, utils::va("Node: %i", node_index));
					main_module::debug_draw_box(node->m_vecCenter, node->m_vecHalfDiagonal, 1.0f, api::DEBUG_REMIX_LINE_COLOR::TEAL);
				}
			}
			*/
		}
#endif
		// show leaf index as 3D text
		if (current_leaf < world->numleafs)
		{
			g_player_current_leaf = current_leaf;

			if (api::remix_debug_node_vis)
			{
				const auto curr_leaf = &world->leafs[current_leaf];
				game::debug_add_text_overlay(&curr_leaf->m_vecCenter.x, 0.0f, utils::va("Leaf: %i", current_leaf));
				main_module::debug_draw_box(curr_leaf->m_vecCenter, curr_leaf->m_vecHalfDiagonal, 2.0f, api::DEBUG_REMIX_LINE_COLOR::GREEN);
			}
		}


		// We have to set all nodes from the target leaf to the root node or the node the the player is in to the current visframe
		// Otherwise, 'R_RecursiveWorldNode' will never reach the target leaf

		if (area_update && !map_settings.area_settings.empty())
		{
			if (const auto& t = map_settings.area_settings.find(current_area);
				t != map_settings.area_settings.end())
			{
				for (auto l : t->second)
				{
					if (l < static_cast<std::uint32_t>(world->numleafs)) {
						force_leaf_vis(l, &world->nodes[g_player_current_node]); // force leaf to be visible
					}
				}
			}
		}


		// leaf transitions
		if (leaf_update && !map_settings.leaf_transitions.empty())
		{
			for (auto t = map_settings.leaf_transitions.begin(); t != map_settings.leaf_transitions.end();)
			{
				bool iterpp = false;
				bool trigger_transition = false;

				const bool keep_transition = t->mode >= map_settings::ALWAYS_ON_ENTER;
				const bool trigger_on_enter = t->mode == map_settings::ONCE_ON_ENTER || t->mode == map_settings::ALWAYS_ON_ENTER;
				const bool trigger_on_leave = t->mode == map_settings::ONCE_ON_LEAVE || t->mode == map_settings::ALWAYS_ON_LEAVE;

				if (t->leafs.contains(current_leaf))
				{
					if (!t->_state_enter) // first time we enter the leafset
					{
						if (trigger_on_enter) {
							trigger_transition = true;
						}

						t->_state_enter = true;
					}
				}

				// no longer touching any leaf in this set
				else
				{
					if (t->_state_enter) // player just moved out of the leafset
					{
						if (trigger_on_leave) {
							trigger_transition = true;
						}
					}

					t->_state_enter = false;
				}

				if (trigger_transition)
				{
					bool can_add_transition = true;

					// do not allow the same transition twice
					for (const auto& ip : api::remix_vars::interpolate_stack)
					{
						if (ip.identifier == t->hash)
						{
							can_add_transition = false;
							break;
						}
					}

					if (can_add_transition)
					{
						api::remix_vars::parse_and_apply_conf_with_lerp(
							t->config_name,
							t->hash,
							t->interpolate_type,
							t->duration,
							t->delay_in,
							t->delay_out);

						if (!keep_transition)
						{
							t = map_settings.leaf_transitions.erase(t);
							iterpp = true; // erase returns the next iterator
						}
					}
				}

				if (!iterpp) {
					++t;
				}
			}
		}

		//force_node_vis(1095, &world->nodes[player_node_index], r_visframecount);
		//force_leaf_vis(1123, &world->nodes[player_node_index], r_visframecount);

		// force all nodes in area
		//for (auto i = 0; i < world->numnodes; i++)
		//{
		//	// force leaf to be visible
		//	world->nodes[i].visframe = game::get_visframecount();
		//}

		//for (auto i = 0; i < world->numleafs; i++)
		//{
		//	// force leaf to be visible
		//	world->leafs[i].visframe = game::get_visframecount();
		//}
	}

	HOOK_RETN_PLACE_DEF(pre_recursive_world_node_retn);
	__declspec(naked) void pre_recursive_world_node_stub()
	{
		__asm
		{
			pushad;
			call	pre_recursive_world_node;
			popad;

			// og
			mov     edx, [edx + 0x54];
			mov     ecx, ebx;
			jmp		pre_recursive_world_node_retn;

		}
	}


	// stub in 'R_RecursiveWorldNode' right on 'while (xnode->visframe == r_visframecount ... )'
	int node_visframe_check(const mnode_t* xnode)
	{
		// check if this node would be excluded from rendering
		if (xnode->visframe != game::get_visframecount())
		{
			// sort dimensions to identify height, width, and depth
			float dims[3] = {
				std::fabs(xnode->m_vecHalfDiagonal.x * 2.0f),
				std::fabs(xnode->m_vecHalfDiagonal.y * 2.0f),
				std::fabs(xnode->m_vecHalfDiagonal.z * 2.0f) };

			std::sort(dims, dims + 3);

			const auto width = dims[1];
			const auto height = dims[2];
			const auto depth = dims[0];

			// try to find rectangular cuboids that could match emissive lights
			if (height / width >= 2.0f && depth <= 35.0f) {
				return 1;
			}

			// exlude node
			return 0;
		}

		// node was marked as potentially visible by 'Map_VisSetup'
		return 1;
	}

	HOOK_RETN_PLACE_DEF(while_recursive_world_node_og_retn);
	HOOK_RETN_PLACE_DEF(while_recursive_world_node_skip_retn);
	__declspec(naked) void while_recursive_world_node_stub()
	{
		__asm
		{
			pushad;
			push	ebx;
			call	node_visframe_check;
			add		esp, 4;
			cmp		eax, 1;
			je		SKIP; // jump if eax = 1
			popad;

			jmp		while_recursive_world_node_og_retn;

		SKIP:
			popad;
			jmp		while_recursive_world_node_skip_retn;

		}
	}


	// #
	// Portal anti cull

	// CustomVis will be used when Map_VisSetup is called (which calculates pot. visible nodes and updates their visframe)
	// This can be quite bad if we force a node that is completely on the other side of the node tree because that will make the entire tree visible (R_RecursiveWorldNode will not render the other side otherwise)
	// That is not a problem with the way the game normally renders portals (stencil) as that happens in a completely different renderview so areas can be rendered separately from each other 
	// Since we disabled the stencil'd rendering, we have to render everything in one pass.

	// CViewRender* this
	void viewdrawscene_custom_portal_vis(void* view_renderer, bool bDrew3dSkybox, int nSkyboxVisible, const CViewSetup* view, int nClearFlags, int viewID, bool bDrawViewModel, int baseDrawFlags, [[maybe_unused]] ViewCustomVisibility_t* pCustomVisibility)
	{
		ViewCustomVisibility_t customVisibility = {};
		customVisibility.m_VisData.m_fDistToAreaPortalTolerance = FLT_MAX;

		bool is_using_custom_vis = false;
		bool added_player_view_vis = false;

		// #
		auto portal_vis = [](model_render::game_portal_info_s* p, const CViewSetup* view, ViewCustomVisibility_t* vis, bool* set_view_org)
			{
				if (p->portal && p->portal->m_pLinkedPortal && p->portal->m_fOpenAmount != 0.0f)
				{
					// check if player view was added already
					if (!*set_view_org)
					{
						// add the main scene view first if setting custom portal vis
						vis->m_rgVisOrigins[vis->m_nNumVisOrigins++] = view->origin;
						*set_view_org = true;
					}

					//CPortalRenderable_FlatBasic::AddToVisAsExitPortal(CPortalRenderable_FlatBasic * this, ViewCustomVisibility_t * pCustomVisibility)
					utils::hook::call<void(__fastcall)(void* this_ptr, void* null, ViewCustomVisibility_t*)>(CLIENT_BASE + USE_OFFSET(0x2C2830, 0x2BBDA0))
						(p->portal->m_pLinkedPortal, nullptr, vis);

					return true;
				}

				return false;
			};

		is_using_custom_vis = portal_vis(&model_render::game_portals[0], view, &customVisibility, &added_player_view_vis) ? true : is_using_custom_vis;
		is_using_custom_vis = portal_vis(&model_render::game_portals[1], view, &customVisibility, &added_player_view_vis) ? true : is_using_custom_vis;
		is_using_custom_vis = portal_vis(&model_render::game_portals[2], view, &customVisibility, &added_player_view_vis) ? true : is_using_custom_vis;
		is_using_custom_vis = portal_vis(&model_render::game_portals[3], view, &customVisibility, &added_player_view_vis) ? true : is_using_custom_vis;

		/*
		auto* p = &model_render::game_portals[0];
		//if (model_render::portal1_ptr && model_render::portal1_ptr->m_pLinkedPortal && model_render::portal1_ptr->m_fOpenAmount != 0.0f)
		if (p->portal && p->portal->m_pLinkedPortal && p->portal->m_fOpenAmount != 0.0f)
		{
			// add the main scene view first if setting custom portal vis
			customVisibility.m_rgVisOrigins[customVisibility.m_nNumVisOrigins++] = view->origin;
			added_player_view_vis = true;

			//CPortalRenderable_FlatBasic::AddToVisAsExitPortal(CPortalRenderable_FlatBasic * this, ViewCustomVisibility_t * pCustomVisibility)
			utils::hook::call<void(__fastcall)(void* this_ptr, void* null, ViewCustomVisibility_t*)>(CLIENT_BASE + USE_OFFSET(0x2C2830, 0x2BBDA0))
				(p->portal->m_pLinkedPortal, nullptr, &customVisibility);

			is_using_custom_vis = true;
		}

		//if (model_render::portal2_ptr && model_render::portal2_ptr->m_pLinkedPortal && model_render::portal2_ptr->m_fOpenAmount != 0.0f)
		p = &model_render::game_portals[1];
		if (p->portal && p->portal->m_pLinkedPortal && p->portal->m_fOpenAmount != 0.0f)
		{
			// check if player view was added already
			if (!added_player_view_vis)
			{
				// add the main scene view first if setting custom portal vis
				customVisibility.m_rgVisOrigins[customVisibility.m_nNumVisOrigins++] = view->origin;
				added_player_view_vis = true;
			}

			//CPortalRenderable_FlatBasic::AddToVisAsExitPortal(CPortalRenderable_FlatBasic * this, ViewCustomVisibility_t * pCustomVisibility)
			utils::hook::call<void(__fastcall)(void* this_ptr, void* null, ViewCustomVisibility_t*)>(CLIENT_BASE + USE_OFFSET(0x2C2830, 0x2BBDA0))
				(p->portal->m_pLinkedPortal, nullptr, &customVisibility);

			is_using_custom_vis = true;
		}

		// second pair
		p = &model_render::game_portals[2];
		if (p->portal && p->portal->m_pLinkedPortal && p->portal->m_fOpenAmount != 0.0f)
		{
			// check if player view was added already
			if (!added_player_view_vis)
			{
				// add the main scene view first if setting custom portal vis
				customVisibility.m_rgVisOrigins[customVisibility.m_nNumVisOrigins++] = view->origin;
				added_player_view_vis = true;
			}

			//CPortalRenderable_FlatBasic::AddToVisAsExitPortal(CPortalRenderable_FlatBasic * this, ViewCustomVisibility_t * pCustomVisibility)
			utils::hook::call<void(__fastcall)(void* this_ptr, void* null, ViewCustomVisibility_t*)>(CLIENT_BASE + USE_OFFSET(0x2C2830, 0x2BBDA0))
				(p->portal->m_pLinkedPortal, nullptr, &customVisibility);

			is_using_custom_vis = true;
		}

		p = &model_render::game_portals[3];
		if (p->portal && p->portal->m_pLinkedPortal && p->portal->m_fOpenAmount != 0.0f)
		{
			// check if player view was added already
			if (!added_player_view_vis)
			{
				// add the main scene view first if setting custom portal vis
				customVisibility.m_rgVisOrigins[customVisibility.m_nNumVisOrigins++] = view->origin;
				added_player_view_vis = true;
			}

			//CPortalRenderable_FlatBasic::AddToVisAsExitPortal(CPortalRenderable_FlatBasic * this, ViewCustomVisibility_t * pCustomVisibility)
			utils::hook::call<void(__fastcall)(void* this_ptr, void* null, ViewCustomVisibility_t*)>(CLIENT_BASE + USE_OFFSET(0x2C2830, 0x2BBDA0))
				(p->portal->m_pLinkedPortal, nullptr, &customVisibility);

			is_using_custom_vis = true;
		}
		*/

		// custom vis for area portals that were added in the previous frame (portal views are rendered after the main scene)
		// >> this can be kinda bad as it potentially makes the whole map visible when the area is on the other side of the map and thus
		// setting the whole node tree to the current visframe >> R_RecursiveWorld
#if 0
		for (const auto& p : model_render::linked_area_portals)
		{
			// check if player view was added already
			if (!added_player_view_vis)
			{
				// add the main scene view first if setting custom portal vis
				customVisibility.m_rgVisOrigins[customVisibility.m_nNumVisOrigins++] = view->origin;
				added_player_view_vis = true;
			}

			//CPortalRenderable_FlatBasic::AddToVisAsExitPortal(CPortalRenderable_FlatBasic * this, ViewCustomVisibility_t * pCustomVisibility)
			utils::hook::call<void(__fastcall)(void* this_ptr, void* null, ViewCustomVisibility_t*)>(CLIENT_BASE + USE_OFFSET(0x2C2830, 0x2BBDA0))
				(p, nullptr, &customVisibility);

			is_using_custom_vis = true;
		}
#endif
		// remove all area portals that were added in the previous frame
		model_render::linked_area_portals.clear();  

#if 0
		//if (/*api::allow_api_portals &&*/ api::portal0_mesh && api::portal1_mesh)
		if (!api::rayportal_ctx.empty())
		{
			auto& vis = customVisibility;

			// check if player view was added already
			if (!added_player_view_vis)
			{
				// add the main scene view first if setting custom portal vis
				vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = view->origin;
				//added_player_view_vis = true;
			}

			for (auto it = api::rayportal_ctx.pairs.begin(); it != api::rayportal_ctx.pairs.end(); ++it)
			{
				const auto& p0_corner_points = it->second.get_portal0().get_corner_points();
				vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = p0_corner_points[0];	// 1
				vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = p0_corner_points[1];	// 2
				vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = p0_corner_points[2];	// 3
				vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = p0_corner_points[3];	// 4
				vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = it->second.get_portal0().m_pos; // 5

				const auto& p1_corner_points = it->second.get_portal1().get_corner_points();
				vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = p1_corner_points[0];	// 6
				vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = p1_corner_points[1];	// 7
				vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = p1_corner_points[2];	// 8
				vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = p1_corner_points[3];	// 9
				vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = it->second.get_portal1().m_pos;

				vis.m_iForceViewLeaf = game::get_leaf_from_position(it->second.get_portal1().m_pos);
				vis.m_VisData.m_vecVisOrigin = it->second.get_portal1().m_pos; // last m_rgVisOrigin
				vis.m_VisData.m_fDistToAreaPortalTolerance = 64.0f;
				vis.m_VisData.m_bTrimFrustumToPortalCorners = true;

				vis.m_VisData.m_vPortalOrigin = it->second.get_portal1().m_pos; // vis org 10 : z -= 1
				vis.m_VisData.m_vPortalOrigin.z -= 1; // ?

				vis.m_VisData.m_vPortalForward = it->second.get_portal1().get_normal();
				vis.m_VisData.m_flPortalRadius = 64.4980621f;
				vis.m_VisData.m_vPortalCorners[0] = p1_corner_points[0]; // vis org 6
				vis.m_VisData.m_vPortalCorners[1] = p1_corner_points[1]; // 7
				vis.m_VisData.m_vPortalCorners[2] = p1_corner_points[2]; // 8
				vis.m_VisData.m_vPortalCorners[3] = p1_corner_points[3]; // 9
			}

#if 0
			//portal0 corners?
			vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = { 6110.87109f, 3518.96875f, 1667.12109f };	// 1
			vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = { 6174.87109f, 3518.96875f, 1667.12109f };	// 2
			vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = { 6174.87109f, 3518.96875f, 1555.12109f };	// 3
			vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = { 6110.87109f, 3518.96875f, 1555.12109f };	// 4
			vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = { 6142.87109f, 3518.96875f, 1611.12109f };	// 5 -- center
			vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = { 10460.3789f, 1225.95068f, 344.843750f };	// 6
			vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = { 10451.3281f, 1162.59399f, 344.843750f };	// 7
			vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = { 10451.3281f, 1162.59399f, 232.843750f };	// 8
			vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = { 10460.3789f, 1225.95068f, 232.843750f };	// 9
			vis.m_rgVisOrigins[vis.m_nNumVisOrigins++] = { 10455.8535f, 1194.27234f, 288.843750f };	// 10 -- center

			// last portal data
			vis.m_iForceViewLeaf = 107;
			vis.m_VisData.m_vecVisOrigin = { 10455.8535f, 1194.27234f, 288.843750f }; // last m_rgVisOrigin
			vis.m_VisData.m_fDistToAreaPortalTolerance = 64.0f;
			vis.m_VisData.m_bTrimFrustumToPortalCorners = true;
			vis.m_VisData.m_vPortalOrigin = { 10456.8438f, 1194.13086f, 288.843750f }; // vis org 10 : z -= 1
			vis.m_VisData.m_vPortalForward = { -0.989949524f, 0.141421184f, 0.00000000f };
			vis.m_VisData.m_flPortalRadius = 64.4980621f;
			vis.m_VisData.m_vPortalCorners[0] = { 10460.3789f, 1225.95068f, 344.843750 }; // vis org 6
			vis.m_VisData.m_vPortalCorners[1] = { 10451.3281f, 1162.59399f, 344.843750 }; // 7
			vis.m_VisData.m_vPortalCorners[2] = { 10451.3281f, 1162.59399f, 232.843750 }; // 8
			vis.m_VisData.m_vPortalCorners[3] = { 10460.3789f, 1225.95068f, 232.843750 }; // 9
#endif

			is_using_custom_vis = true;
		}
#endif

		utils::hook::call<void(__fastcall)(void* this_ptr, void* null, bool, int, const CViewSetup*, int, int, bool, int, ViewCustomVisibility_t*)>(CLIENT_BASE + USE_OFFSET(0x1EDAE0, 0x1E84E0))
			(view_renderer, nullptr, bDrew3dSkybox, nSkyboxVisible, view, nClearFlags, viewID, bDrawViewModel, baseDrawFlags, is_using_custom_vis ? &customVisibility : nullptr);
	}

	HOOK_RETN_PLACE_DEF(viewdrawscene_push_args_retn);
	__declspec(naked) void viewdrawscene_push_args_stub()
	{
		__asm
		{
			push	ecx; // CViewRender (this)
			call	viewdrawscene_custom_portal_vis;
			add		esp, 4;

			jmp		viewdrawscene_push_args_retn;
		}
	}


	int override_entity_vis(C_BaseEntity* ent)
	{
		if (ent && ent->model && std::string_view(ent->model->szPathName).contains("mapmarker")) {
			return 1;
		}

		return 0; 
	}

	HOOK_RETN_PLACE_DEF(base_ent_update_vis_draw_retn);
	HOOK_RETN_PLACE_DEF(base_ent_update_vis_skip_retn);
	__declspec(naked) void base_ent_update_vis_stub()
	{
		__asm
		{
			push	ebp;
			pushad;
			push	edi; // C_BaseEntity*
			call	override_entity_vis;
			add		esp, 4;
			cmp		eax, 1;
			jne		SKIP; // jump if eax = 0
			popad;
			pop		ebp;
			mov     byte ptr[ebp - 1], 1 // bDraw
			jmp		base_ent_update_vis_draw_retn;

		SKIP:
			popad;
			pop		ebp;
			// og code without overrides
			cmp     byte ptr[ebp - 1], 0; // bDraw check
			jz		SKIP_OG;
			jmp		base_ent_update_vis_draw_retn;

		SKIP_OG:
			jmp		base_ent_update_vis_skip_retn;
		}
	}


	struct CViewRender
	{
		char pad[0x3E8];
		FadeData_t m_FadeData;
	};

	// cpu_level + 1
	FadeData_t g_aFadeData[] =
	{
		// pxmin  pxmax    width   scale
		{  0.0f,   0.0f,  1280.0f,  1.0f },
		{ 10.0f,  15.0f,   800.0f,  1.0f },
		{  5.0f,  10.0f,  1024.0f,  1.0f },
		{  0.0f,   0.0f,  1280.0f,  0.1f },
		{ 36.0f, 144.0f,   720.0f,  1.0f },
		{  0.0f,   0.0f,  1280.0f,  1.0f },
	};

	void init_fade_data_hk(CViewRender* view_render)
	{
		view_render->m_FadeData = g_aFadeData[3];
	}

	HOOK_RETN_PLACE_DEF(init_fade_data_retn);
	__declspec(naked) void init_fade_data_stub()
	{
		__asm
		{
			push	esi; // CViewRender (this)
			call	init_fade_data_hk;
			add		esp, 4;
			jmp		init_fade_data_retn;
		}
	}


	// #
	// Commands

	ConCommand xo_debug_toggle_node_vis_cmd{};
	void xo_debug_toggle_node_vis_fn()
	{
		api::remix_debug_node_vis = !api::remix_debug_node_vis;
	}

#if defined(BENCHMARK)
	ConCommand xo_debug_toggle_benchmark_cmd{};
	void xo_debug_toggle_benchmark_fn()
	{
		model_render::m_benchmark.enabled = !model_render::m_benchmark.enabled;

		if (model_render::m_benchmark.enabled) {
			system("cls");
		}
	}
#endif

	// #
	// #

	void main_module::setup_required_cvars()
	{
		// #
		// force / uncheat cvars

		//utils::benchmark bench;

		// uncheat lod cvars but don't force them when commandline arg is set
		if (static bool no_lod_forcing = flags::has_flag("xo_disable_lod_forcing");
			no_lod_forcing)
		{
			game::cvar_uncheat("r_staticprop_lod");
			game::cvar_uncheat("r_lod");
			game::cvar_uncheat("r_lod_switch_scale"); // hidden cvar
		}
		else
		{
			game::cvar_uncheat_and_set_int("r_staticprop_lod", 0);
			game::cvar_uncheat_and_set_int("r_lod", 0);
			game::cvar_uncheat_and_set_int("r_lod_switch_scale", 1); // hidden cvar
		}

		game::cvar_uncheat_and_set_int("r_PortalTestEnts", 0);
		game::cvar_uncheat_and_set_int("portal_ghosts_disable", 0);
		game::cvar_uncheat_and_set_int("r_portal_earlyz", 0);
		game::cvar_uncheat_and_set_int("r_portal_use_complex_frustums", 0);
		game::cvar_uncheat_and_set_int("r_portal_use_pvs_optimization", 0);
		game::cvar_uncheat_and_set_int("r_portalstencildisable", 1);
		game::cvar_uncheat_and_set_int("r_portal_stencil_depth", 0);
		game::cvar_uncheat_and_set_int("portal_draw_ghosting", 0);
		
		game::cvar_uncheat_and_set_int("r_threaded_particles", 0);
		game::cvar_uncheat_and_set_int("r_entityclips", 0);
		game::cvar_uncheat_and_set_int("cl_brushfastpath", 0);
		game::cvar_uncheat_and_set_int("cl_tlucfastpath", 0);
		game::cvar_uncheat_and_set_int("cl_modelfastpath", 0);
		game::cvar_uncheat_and_set_int("mat_queue_mode", 0); // does improve performance but breaks rendering
		game::cvar_uncheat_and_set_int("mat_softwarelighting", 0);
		game::cvar_uncheat_and_set_int("mat_parallaxmap", 0);
		game::cvar_uncheat_and_set_int("mat_frame_sync_enable", 0);
		game::cvar_uncheat_and_set_int("mat_dof_enabled", 0);
		game::cvar_uncheat_and_set_int("mat_displacementmap", 0);
		game::cvar_uncheat_and_set_int("mat_drawflat", 0);
		game::cvar_uncheat_and_set_int("mat_normalmaps", 0);
		game::cvar_uncheat_and_set_int("r_3dsky", 0);
		game::cvar_uncheat_and_set_int("r_skybox_draw_last", 0);
		game::cvar_uncheat_and_set_int("r_flashlightrender", 0); // fix emissive "bug" when moon portal opens on finale4 
		game::cvar_uncheat_and_set_int("mat_fullbright", 1);
		game::cvar_uncheat_and_set_int("mat_softwareskin", 1);
		game::cvar_uncheat_and_set_int("mat_phong", 1);
		game::cvar_uncheat_and_set_int("mat_fastnobump", 1);
		game::cvar_uncheat_and_set_int("mat_disable_bloom", 1);

		// graphic settings

		// lvl 0
		game::cvar_uncheat_and_set_int("cl_particle_fallback_base", 3); // 0 = render portalgun viewmodel effects
		game::cvar_uncheat_and_set_int("cl_particle_fallback_multiplier", 2);
		//game::cvar_uncheat_and_set_int("cl_footstep_fx", 0); // does not exist
		game::cvar_uncheat_and_set_int("cl_impacteffects_limit_general", 10);
		game::cvar_uncheat_and_set_int("cl_impacteffects_limit_exit", 3);
		game::cvar_uncheat_and_set_int("cl_impacteffects_limit_water", 2);
		game::cvar_uncheat_and_set_int("mat_depthfeather_enable", 0);
		game::cvar_uncheat_and_set_int("mat_force_vertexfog", 1); // does not exist
		
		game::cvar_uncheat_and_set_int("cl_detaildist", 1024);
		game::cvar_uncheat_and_set_int("cl_detailfade", 400);
		game::cvar_uncheat_and_set_int("r_drawmodeldecals", 1);
		game::cvar_uncheat_and_set_int("r_decalstaticprops", 1);
		game::cvar_uncheat_and_set_int("cl_player_max_decal_count", 32);
		game::cvar_uncheat_and_set_int("r_paintblob_force_single_pass", 1);
		game::cvar_uncheat_and_set_int("r_paintblob_max_number_of_threads", 1);
		game::cvar_uncheat_and_set_float("r_paintblob_highres_cube", 1.0);

		// lvl 3
		game::cvar_uncheat_and_set_int("r_decals", 2048);
		game::cvar_uncheat_and_set_int("r_decal_overlap_count", 3);

		// disable fog
		game::cvar_uncheat_and_set_int("fog_override", 1);
		game::cvar_uncheat_and_set_int("fog_enable", 0);

		//float ms = 0.0f;
		//bench.now(&ms);
		//printf("[ %.3f ms ]\n", ms);
	}

	main_module::main_module()
	{
		{ // init filepath var
			char path[MAX_PATH]; GetModuleFileNameA(nullptr, path, MAX_PATH);
			game::root_path = path; utils::erase_substring(game::root_path, "portal2.exe");
		}
		
		// init remixAPI
		api::init();

		{ // init d3d font
			D3DXFONT_DESC desc =
			{
				18,                  // Height
				0,                   // Width (0 = default)
				FW_NORMAL,           // Weight (FW_BOLD, FW_LIGHT, etc.)
				1,                   // Mip levels
				FALSE,               // Italic
				DEFAULT_CHARSET,     // Charset
				OUT_DEFAULT_PRECIS,  // Output Precision
				CLIP_DEFAULT_PRECIS, // Clipping Precision
				DEFAULT_PITCH,       // Pitch and Family
				TEXT("Arial")        // Typeface
			};

			D3DXCreateFontIndirect(game::get_d3d_device(), &desc, &d3d_font);
		}

		// parse rtx.conf once
		api::remix_vars::parse_rtx_options();

		// init addon textures
		model_render::init_texture_addons();

		// force cvars on init (too late as we init after CL_Init)
		main_module::setup_required_cvars();

#if defined(BENCHMARK)
		game::console();
#endif

		// #
		// commands

		game::con_add_command(&xo_debug_toggle_node_vis_cmd, "xo_debug_toggle_node_vis", xo_debug_toggle_node_vis_fn, "Toggle bsp node/leaf debug visualization using the remix api");

#if defined(BENCHMARK)
		game::con_add_command(&xo_debug_toggle_benchmark_cmd, "xo_debug_toggle_benchmark", xo_debug_toggle_benchmark_fn, "Toggle benchmark printing");
#endif

		// #
		// events

		// CModelLoader::Map_LoadModel :: called on map load
		utils::hook(ENGINE_BASE + USE_OFFSET(0xFD8FC, 0xFCD5C), on_map_load_stub).install()->quick();
		HOOK_RETN_PLACE(on_map_load_stub_retn, ENGINE_BASE + USE_OFFSET(0xFD901, 0xFCD61));

		// Host_Disconnect :: called on map unload
		utils::hook(ENGINE_BASE + USE_OFFSET(0x19A3E1, 0x197DF1), on_host_disconnect_stub).install()->quick();
		HOOK_RETN_PLACE(on_host_disconnect_retn, ENGINE_BASE + USE_OFFSET(0x19A3E6, 0x197DF6));


		// CViewRender::RenderView :: "start" of current frame (after CViewRender::DrawMonitors)
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x1F23C5, 0x1ECDC5), 7);
		utils::hook(CLIENT_BASE + USE_OFFSET(0x1F23C5, 0x1ECDC5), cviewrenderer_renderview_stub).install()->quick();
		HOOK_RETN_PLACE(cviewrenderer_renderview_retn, CLIENT_BASE + USE_OFFSET(0x1F23CC, 0x1ECDCC));

		// CViewRender::DrawOneMonitor
		utils::hook(CLIENT_BASE + USE_OFFSET(0x1EE8F4, 0x1E92F4), cviewrenderer_drawonemonitor_stub).install()->quick();
		HOOK_RETN_PLACE(cviewrenderer_drawonemonitor_retn, CLIENT_BASE + USE_OFFSET(0x1EE8F9, 0x1E92F9));


		// #OFFSET - missing
		// CL_Init :: Unused as asi injection happens after CL_Init
		//utils::hook(ENGINE_BASE + 0x975E7, on_cl_init_stub).install()->quick();

		// #
		// culling

		// same as r_novis 1
		// def. needs 'r_portal_stencil_depth 1' if not enabled
		if (flags::has_flag("xo_disable_all_culling"))
		{
			// R_RecursiveWorldNode :: while (node->visframe == r_visframecount .. ) -> renders the entire map if everything after this is enabled
			utils::hook::nop(ENGINE_BASE + USE_OFFSET(0xE724F, 0xE68EF), 6);
		}

		// stub before calling 'R_RecursiveWorldNode' to override node/leaf vis
		utils::hook(ENGINE_BASE + USE_OFFSET(0xE76CD, 0xE6D6D), pre_recursive_world_node_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(pre_recursive_world_node_retn, ENGINE_BASE + USE_OFFSET(0xE76D2, 0xE6D72));

		// ^ :: xnode->visframe == r_visframecount check - check for rectangular cuboids that could match emissive lights
		utils::hook::nop(ENGINE_BASE + USE_OFFSET(0xE7246, 0xE68E6), 9);
		utils::hook(ENGINE_BASE + USE_OFFSET(0xE7246, 0xE68E6), while_recursive_world_node_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(while_recursive_world_node_og_retn, ENGINE_BASE + USE_OFFSET(0xE73A2, 0xE6A42));
		HOOK_RETN_PLACE(while_recursive_world_node_skip_retn, ENGINE_BASE + USE_OFFSET(0xE7255, 0xE68F5));

		// ^ :: while( ... node->contents < -1 .. ) -> jl to jle
		utils::hook::set<BYTE>(ENGINE_BASE + USE_OFFSET(0xE7258, 0xE68F8), 0x7E);

		// ^ :: while( ... !R_CullNode) - needed for displacements
		utils::hook::nop(ENGINE_BASE + USE_OFFSET(0xE7265, 0xE6905), 6);

		// ^ :: backface check -> je to jl
		utils::hook::nop(ENGINE_BASE + USE_OFFSET(0xE7323, 0xE69C3), 2);

		// ^ :: backface check -> jnz to je
		utils::hook::set<BYTE>(ENGINE_BASE + USE_OFFSET(0xE732D, 0xE69CD), 0x74);

		// R_DrawLeaf :: backface check (emissive lamps) plane normal >= -0.00999f
		utils::hook::nop(ENGINE_BASE + USE_OFFSET(0xE6F23, 0xE65C3), 6);

		// CBrushBatchRender::DrawOpaqueBrushModel :: :: backface check - nop 'if ( bShadowDepth )' to disable culling
		utils::hook::nop(ENGINE_BASE + USE_OFFSET(0x7196E, 0x7156E), 2);

		// CClientLeafSystem::ExtractCulledRenderables :: disable 'engine->CullBox' check to disable entity culling in leafs
		// needs r_PortalTestEnts to be 0 -> je to jmp (0xEB)
		utils::hook::set<BYTE>(CLIENT_BASE + USE_OFFSET(0xE20F5, 0xDE4D5), 0xEB);

		// DrawDisplacementsInLeaf :: nop 'Frustum_t::CullBox' check to disable displacement (terrain) culling in leafs
		utils::hook::nop(ENGINE_BASE + USE_OFFSET(0xE6CE4, 0xE6384), 2);

		// C_VGuiScreen::DrawModel :: vgui screens (world) :: nop C_VGuiScreen::IsBackfacing check
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0xCDD1E, 0xCA14E), 2);

		// CSimpleWorldView::Setup :: nop 'DoesViewPlaneIntersectWater' check
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x1EAC93, 0x1E5693), 2);

		// ^ next instruction :: OR m_DrawFlags with 0x60 instead of 0x30
		utils::hook::set<BYTE>(CLIENT_BASE + USE_OFFSET(0x1EAC95, 0x1E5695) + 6, 0x60);

		// C_Portal_Player::DrawModel :: disable 'C_Portal_Player::ShouldSkipRenderingViewpointPlayerForThisView' check to always render chell
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x27A85B, 0x274FFB), 2);

		utils::hook(CLIENT_BASE + USE_OFFSET(0x282F1C, 0x27D4AC), cportalghost_should_draw_stub).install()->quick();
		HOOK_RETN_PLACE(cportalghost_should_draw_retn, CLIENT_BASE + USE_OFFSET(0x282F21, 0x27D4B1));


		// CShaderManager::SetPixelShader :: disable warning print + place stub so we can break and see what type of shader is failing to load
		utils::hook::nop(RENDERER_BASE + USE_OFFSET(0x2B244, 0x2AAB4), 6); // disable 'Trying to set a pixel shader that failed loading' print
		utils::hook(RENDERER_BASE + USE_OFFSET(0x2B24A, 0x2AABA), set_pixelshader_warning_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(set_pixelshader_warning_retn, RENDERER_BASE + USE_OFFSET(0x2B24F, 0x2AABF));

		// Shader_DrawChains :: disable g_pMaterialSystemConfig->nFullbright == 1 check when rendering painted surfaces (binds the "lightmap" (paint map))
		utils::hook::set<BYTE>(ENGINE_BASE + USE_OFFSET(0xE958D, 0xE8C4D), 0xEB);

		// CBrushBatchRender::DrawOpaqueBrushModel :: ^ same for brushmodels
		utils::hook::nop(ENGINE_BASE + USE_OFFSET(0x7193A, 0x7153A), 2);
		utils::hook::set<BYTE>(ENGINE_BASE + USE_OFFSET(0x71940, 0x71540), 0xEB);


		// Fix map visibility when looking through portals when r_portal_stencil_depth == 0
		// - Map_VisSetup called by CViewRender::ViewDrawScene --> CViewRender::SetupVis :: uses player view and 1 visOrigin if no custom vis is provided
		// - Add player vis and call 'CPortalRenderable_FlatBasic::AddToVisAsExitPortal' for both active portals before rendering the main scene
		utils::hook(CLIENT_BASE + USE_OFFSET(0x1F2504, 0x1ECF04), viewdrawscene_push_args_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(viewdrawscene_push_args_retn, CLIENT_BASE + USE_OFFSET(0x1F2509, 0x1ECF09));


		// C_BaseEntity::UpdateVisibility
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x76406, 0x73076), 10);
		utils::hook(CLIENT_BASE + USE_OFFSET(0x76406, 0x73076), base_ent_update_vis_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(base_ent_update_vis_draw_retn, CLIENT_BASE + USE_OFFSET(0x76410, 0x73080));
		HOOK_RETN_PLACE(base_ent_update_vis_skip_retn, CLIENT_BASE + USE_OFFSET(0x76495, 0x73105));

		// #

		// C_Prop_Portal::CreateAttachedParticles :: disable outer portal particle fx (on high shader/effect settings) (broken anyway)
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x287B1D, 0x281FAD), 2);
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x287B28, 0x281FB8) + 5, 1);
		utils::hook::set<DWORD>(CLIENT_BASE + USE_OFFSET(0x287B28, 0x281FB8), 0x00015CE9); // 0F85 5B01 0000 to E9 5C 01 00 00 + 1 nop


		// #
		// General fixes

		// Fix quicksave crashing - game tries to take a screenshot for the save file but thats not working with remix
		// - this disables the 'RenderView' call in 'CViewRender::WriteSaveGameScreenshotOfSize'
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x1D6669, 0x1D0FC9), 4);
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x1D6675, 0x1D0FD5), 2);
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x1D6679, 0x1D0FD9), 2);

		// force some gpu_level 3 logic
		// C_EnvProjectedTexture::ShouldUpdate :: always return true
		utils::hook::set<WORD>(CLIENT_BASE + USE_OFFSET(0x9E50C, 0x9AF1C), 0x01B0); // 30 C0 -> B0 01

		// CViewRender::InitFadeData :: manually set fade data and not rely on cpu_level
		utils::hook(CLIENT_BASE + USE_OFFSET(0x1E51E3, 0x1DFC33), init_fade_data_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(init_fade_data_retn, CLIENT_BASE + USE_OFFSET(0x1E5209, 0x1DFC59));

		// C_BaseEntity::ShouldDraw :: gpu/cpu level checks for simulated entites
		utils::hook::set<BYTE>(CLIENT_BASE + USE_OFFSET(0x6EE5D, 0x6BC2D), 0xEB);
		utils::hook::set<BYTE>(CLIENT_BASE + USE_OFFSET(0x6EE6F, 0x6BC3F), 0xEB);
		utils::hook::set<BYTE>(CLIENT_BASE + USE_OFFSET(0x6EE86, 0x6BC56), 0xEB);

		// CStaticPropMgr::UpdatePropVisibility :: ignore cpu/gpu level key-value pairs on static props
		utils::hook::set<BYTE>(ENGINE_BASE + USE_OFFSET(0x1F02D0, 0x1ED3F0), 0xEB);
		utils::hook::set<BYTE>(ENGINE_BASE + USE_OFFSET(0x1F02FB, 0x1ED41B), 0xEB);
		utils::hook::nop(ENGINE_BASE + USE_OFFSET(0x1F035F, 0x1ED47F), 2);
	}

	main_module::~main_module()
	{
		// release textures
		components::model_render::init_texture_addons(true);

		// release d3d font
		if (d3d_font) {
			d3d_font->Release();
		}
	}
}
