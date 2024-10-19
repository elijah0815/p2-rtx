#include "std_include.hpp"

// + dxlevel 100 required

// commandline args:
// -novid -height 1060 -disable_d3d9_hacks -limitvsconst -softparticlesdefaultoff -disallowhwmorph -no_compressed_verts +sv_cheats 1 +r_PortalTestEnts 0 +portal_ghosts_disable 0 +r_portal_earlyz 0 +r_portal_use_complex_frustums 0 +r_portal_use_pvs_optimization 0 +r_portalstencildisable 0 +r_portal_stencil_depth 0 +portal_draw_ghosting 0 +r_staticprop_lod 0 +r_lod 0 +r_threaded_particles 0 +r_entityclips 0 +cl_brushfastpath 0 +cl_tlucfastpath 0 +cl_modelfastpath 0 +mat_fullbright 1 +mat_queue_mode 0 +mat_softwarelighting 0 +mat_softwareskin 1 +mat_phong 1 +mat_parallaxmap 0 +mat_frame_sync_enable 0 +mat_fastnobump 1 +mat_disable_bloom 1 +mat_dof_enabled 0 +mat_displacementmap 0 +mat_drawflat 0 +mat_normalmaps 0 +mat_normals 0

// *** Required cvars
// r_PortalTestEnts					:: 0 = needed for anti culling of entities
// portal_ghosts_disable			:: 0 = okay until virtual instances are working (see cportalghost_should_draw)			|| 1 = disable rendering of ghost models
// r_portal_stencil_depth			:: 0 = don't let the game render the scene multiple times - portal vis now handled in 'viewdrawscene_custom_portal_vis'

// *** Useful cvars
// mat_leafvis 1					:: print current leaf and area index to console
// r_ShowViewerArea 1				:: show current area on the HUD
// xo_debug_toggle_node_vis (cmd)	:: toggle debug vis of the leaf/node the player is currently in

// *** Other cvars
// r_novis							:: 1 = disable all visleaf/node checks (renders the entire map - same as 'xo_disable_all_culling' cmdline flag)
// cl_particles_show_bbox 1			:: can be used to see fx names


// engine::Shader_WorldEnd interesting for sky
// Shader_DrawChains:: mat_forcedynamic 1 || mat_drawflat 1 => Shader_DrawChainsDynamic (changes hashes but still unstable)

namespace components
{
	/*template <std::size_t Index, typename ReturnType, typename... Args>
	__forceinline ReturnType call_virtual(void* instance, Args... args)
	{
		using Fn = ReturnType(__thiscall*)(void*, Args...);

		auto function = (*static_cast<Fn**>(instance))[Index];
		return function(instance, args...);
	}*/

	int player_current_node = -1;
	int player_current_leaf = -1;

	namespace api
	{
		bool m_initialized = false;
		remixapi_Interface bridge = {};

		remixapi_MaterialHandle remix_debug_line_materials[3];
		remixapi_MeshHandle remix_debug_line_list[128] = {};
		std::uint32_t remix_debug_line_amount = 0u;
		std::uint64_t remix_debug_last_line_hash = 0u;
		bool remix_debug_node_vis = false; // show/hide debug vis of bsp nodes/leafs

		remixapi_LightHandle light_handle = nullptr;

		// called on device->EndScene
		void endscene_cb()
		{
			if (remix_debug_line_amount)
			{
				for (auto l = 1u; l < remix_debug_line_amount + 1; l++)
				{
					if (remix_debug_line_list[l])
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
							.mesh = remix_debug_line_list[l],
							.transform = t0,
							.doubleSided = true
						};

						api::bridge.DrawInstance(&inst);
					}
				}
			}
		}

		// called on device->Present
		void on_present_cb()
		{
			// Draw current node/leaf as HUD
			if (api::remix_debug_node_vis && main_module::d3d_font)
			{
				RECT rect;

				if (player_current_node != -1)
				{
					SetRect(&rect, 10, 100, 512, 512);
					auto text = utils::va("Node: %d", player_current_node);
					main_module::d3d_font->DrawTextA
					(
						nullptr,
						text,
						-1,       // text length (-1 = null-terminated)
						&rect,
						DT_NOCLIP,
						D3DCOLOR_XRGB(0, 255, 255)
					);
				}

				if (player_current_leaf != -1)
				{
					SetRect(&rect, 18, 120, 512, 512);
					auto text = utils::va("Leaf: %d", player_current_leaf);
					main_module::d3d_font->DrawTextA
					(
						nullptr,
						text,
						-1,       // text length (-1 = null-terminated)
						&rect,
						DT_NOCLIP,
						D3DCOLOR_XRGB(255, 0, 0)
					);
				}
				
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
					.hash = remix_debug_last_line_hash ? remix_debug_last_line_hash : 1,
					.surfaces_values = &triangles,
					.surfaces_count = 1,
				};

				api::bridge.CreateMesh(&info, &remix_debug_line_list[remix_debug_line_amount]);
				remix_debug_last_line_hash = reinterpret_cast<std::uint64_t>(remix_debug_line_list[remix_debug_line_amount]);
			}
		}

		void create_portal(std::uint8_t index, remixapi_MeshHandle mesh_handle, remixapi_MaterialHandle material_handle)
		{
			if (!material_handle)
			{
				//main_module::bridge.DestroyMaterial(material_handle);
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
					ext.rayPortalIndex = index;
					ext.rotationSpeed = 1.0f;
				}

				info.pNext = &ext;
				api::bridge.CreateMaterial(&info, &material_handle);
			}

			// mesh

			remixapi_HardcodedVertex verts[4] = {};
			uint32_t indices[6] = {};
			create_quad(verts, indices, 50.0f);

			remixapi_MeshInfoSurfaceTriangles triangles =
			{
			  .vertices_values = verts,
			  .vertices_count = ARRAYSIZE(verts),
			  .indices_values = indices,
			  .indices_count = 6,
			  .skinning_hasvalue = FALSE,
			  .skinning_value = {},
			  .material = material_handle,
			};

			remixapi_MeshInfo i =
			{
			  .sType = REMIXAPI_STRUCT_TYPE_MESH_INFO,
			  .hash = 20,
			  .surfaces_values = &triangles,
			  .surfaces_count = 1,
			};

			//main_module::bridge.DestroyMesh(mesh_handle);
			api::bridge.CreateMesh(&i, &mesh_handle);
		}
	}


	

	void once_per_frame_cb()
	{
		remix_vars::on_client_frame();

		if (!api::remix_debug_line_materials[0])
		{
			remixapi_MaterialInfo info
			{
				.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO,
				.hash = 1,
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

			info.hash = 2;
			info.emissiveColorConstant = { 0.0f, 1.0f, 0.0f };
			api::bridge.CreateMaterial(&info, &api::remix_debug_line_materials[1]);

			info.hash = 3;
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

#if 0
		{
			main_module::bridge.DestroyMesh(model_render::portal0_mdl);
			create_portal(0, model_render::portal0_mdl, model_render::portal0_mtl);
		}

		{
			main_module::bridge.DestroyMesh(model_render::portal1_mdl);
			create_portal(1, model_render::portal1_mdl, model_render::portal1_mtl);
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
			main_module::bridge.DrawInstance(&info);
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
			main_module::bridge.DrawInstance(&info);
		}


		if (!light_handle)
		{
			auto ext = remixapi_LightInfoSphereEXT
			{
				.sType = REMIXAPI_STRUCT_TYPE_LIGHT_INFO_SPHERE_EXT,
				.pNext = nullptr,
				.position = remixapi_Float3D {.x = -1550.0f, .y = 1590.0f, .z = -250.0f},
				.radius = 1,
				.shaping_hasvalue = false,
				.shaping_value = {},
			};

			auto info = remixapi_LightInfo
			{
				.sType = REMIXAPI_STRUCT_TYPE_LIGHT_INFO,
				.pNext = &ext,
				.hash = 1234,
				.radiance = remixapi_Float3D {100, 20, 20},
			};
			main_module::bridge.CreateLight(&info, &light_handle);
		}
		else
		{
			main_module::bridge.DrawLightInstance(light_handle);
		}
#endif
	}

	/**
	 * Called from CViewRender::RenderView
	 * - Pass world, view, projection to D3D
	 * - Other misc. that runs once per frame
	 */
	void on_renderview()
	{
		once_per_frame_cb();

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

		main_module::framecount++; // used for debug anim

		// set a default material with diffuse set to a warm white
		// so that add light to texture works and does not require rtx.effectLightPlasmaBall (animated)
		D3DMATERIAL9 dmat = {};
		dmat.Diffuse.r = 1.0f;
		dmat.Diffuse.g = 0.8f;
		dmat.Diffuse.b = 0.8f;
		dev->SetMaterial(&dmat);
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


	// #
	// #

	/**
	 * called from CModelLoader::Map_LoadModel
	 * @param map_name  Name of loading map
	 */
	void on_map_load_hk(const char* map_name)
	{
		remix_vars::on_map_load();
		map_settings::on_map_load(map_name);

		// reset portal vars
		model_render::portal1_ptr = nullptr;
		model_render::portal1_is_linked = false;
		model_render::portal1_open_amount = 0.0f;
		model_render::portal2_ptr = nullptr;
		model_render::portal2_is_linked = false;
		model_render::portal2_open_amount = 0.0f;
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


	// #
	// #

	// Return nullptr a nullptr to skip rendering of ghost - return ent otherwise
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

		for (auto e = 0u; e < 12; e++)
		{
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
			if (node == player_node || node == root_node)
			{
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

		force_node_vis(parent_node_index, player_node);
	}

	// Called once before 'R_RecursiveWorldNode' is getting called for the first time
	void pre_recursive_world_node()
	{
		const auto world = game::get_hoststate_worldbrush_data();
		const auto g_CurrentViewOrigin = reinterpret_cast<float*>(ENGINE_BASE + 0x50DB50);

		// CM_PointLeafnum :: get current leaf
		const auto current_leaf = utils::hook::call<int(__cdecl)(float*)>(ENGINE_BASE + 0x158540)(g_CurrentViewOrigin);

		// CM_LeafArea :: get current player area
		const auto current_area = utils::hook::call<int(__cdecl)(int leafnum)>(ENGINE_BASE + 0x159470)(current_leaf);

		player_current_node = -1;
		player_current_leaf = -1;

		const auto map_settings = map_settings::settings();

		// find the bsp node the player is currently in + visualize node / leaf using the remix api
		// - skip if map settings contains no overrides for current map and debug vis is not active
		if (map_settings->area_settings.contains(current_area) || api::remix_debug_node_vis)
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
					main_module::debug_draw_box(curr_leaf->m_vecCenter, curr_leaf->m_vecHalfDiagonal, 2.0f, api::DEBUG_REMIX_LINE_COLOR::RED);
				}
			}

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
		}


		// We have to set all nodes from the target leaf to the root node or the node the the player is in to the current visframe
		// Otherwise, 'R_RecursiveWorldNode' will never reach the target leaf

#if 1
		if (!map_settings->area_settings.empty())
		{
			if (map_settings->area_settings.contains(current_area))
			{
				auto tweaks = map_settings->area_settings.find(current_area);
				for (auto l : tweaks->second)
				{
					if (l < static_cast<std::uint32_t>(world->numleafs))
					{
						// force leaf to be visible
						force_leaf_vis(l, &world->nodes[player_current_node]);
					}
				}
			}
		}
#endif

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



	// bad to check for vis overrides here as it is called recursivly!
	void r_recursiveworldnode_visframecount()
	{
#if 0
		const auto map_settings = map_settings::settings();
		if (!map_settings->area_settings.empty())
		{
			// search engine.dll for 'Leaf %d, Area %d, Cluster %d\n'
			// get 'CM_PointLeafnum' and 'CM_LeafArea'

			const auto world = game::get_hoststate_worldbrush_data();
			const auto r_visframecount = *reinterpret_cast<int*>(ENGINE_BASE + 0x6A56B4);

			// get 'g_CurrentViewOrigin' by xrefing 'LeafVisBuild'
			const auto g_CurrentViewOrigin = reinterpret_cast<float*>(ENGINE_BASE + 0x50DB50);

			// CM_PointLeafnum :: get current leaf
			const auto current_leaf = utils::hook::call<int(__cdecl)(float*)>(ENGINE_BASE + 0x158540)(g_CurrentViewOrigin);

			// CM_LeafArea :: get current player area
			const auto current_area = utils::hook::call<int(__cdecl)(int leafnum)>(ENGINE_BASE + 0x159470)(current_leaf);

			if (map_settings->area_settings.contains(current_area))
			{
				auto tweaks = map_settings->area_settings.find(current_area);
				for (auto l : tweaks->second)
				{
					if (l < static_cast<std::uint32_t>(world->numleafs)) // check if leaf index is valid
					{
						// force leaf to be visible
						mleaf_t* leaf = &world->leafs[l];
						leaf->visframe = r_visframecount;
					}
				}

				// force all nodes in area
				//for (auto i = 0; i < world->numnodes; i++)
				//{
				//	auto node = &world->nodes[i];
				//	//if (node->area == current_area)
				//	{
				//		// force leaf to be visible
				//		node->visframe = r_visframecount;
				//	}
				//}

				// debug
				//for (auto i = 0; i < world->numleafs; i++)
				//{
				//	// force leaf to be visible
				//	mleaf_t* leaf = &world->leafs[i];
				//	leaf->visframe = r_visframecount;
				//}
			}
		}
#endif
	}

	HOOK_RETN_PLACE_DEF(r_recursiveworldnode_retn);
	__declspec(naked) void r_recursiveworldnode_stub()
	{
		__asm
		{
			pushad;
			call	r_recursiveworldnode_visframecount;
			popad;

			// og
			mov     ebx, edx;
			mov     eax, [ebx];
			push    esi;

			jmp		r_recursiveworldnode_retn;

		}
	}


	// #
	// Portal anti cull

	// CViewRender* this
	void viewdrawscene_custom_portal_vis(void* view_renderer, bool bDrew3dSkybox, int nSkyboxVisible, const CViewSetup* view, int nClearFlags, int viewID, bool bDrawViewModel, int baseDrawFlags, [[maybe_unused]] ViewCustomVisibility_t* pCustomVisibility)
	{
		ViewCustomVisibility_t customVisibility = {};
		customVisibility.m_VisData.m_fDistToAreaPortalTolerance = FLT_MAX;

		bool is_using_custom_vis = false;
		bool added_player_view_vis = false;

		if (model_render::portal1_ptr && model_render::portal1_ptr->m_pLinkedPortal && model_render::portal1_ptr->m_fOpenAmount != 0.0f)
		{
			// add the main scene view first if setting custom portal vis
			customVisibility.m_rgVisOrigins[0] = view->origin;
			customVisibility.m_nNumVisOrigins++;
			added_player_view_vis = true;

			//CPortalRenderable_FlatBasic::AddToVisAsExitPortal(CPortalRenderable_FlatBasic * this, ViewCustomVisibility_t * pCustomVisibility)
			utils::hook::call<void(__fastcall)(void* this_ptr, void* null, ViewCustomVisibility_t*)>(CLIENT_BASE + 0x2BBDA0)
				(model_render::portal1_ptr->m_pLinkedPortal, nullptr, &customVisibility);

			is_using_custom_vis = true;
		}

		if (model_render::portal2_ptr && model_render::portal2_ptr->m_pLinkedPortal && model_render::portal2_ptr->m_fOpenAmount != 0.0f)
		{
			// check if player view was added already
			if (!added_player_view_vis)
			{
				// add the main scene view first if setting custom portal vis
				customVisibility.m_rgVisOrigins[0] = view->origin;
				customVisibility.m_nNumVisOrigins++;
				added_player_view_vis = true;
			}

			//CPortalRenderable_FlatBasic::AddToVisAsExitPortal(CPortalRenderable_FlatBasic * this, ViewCustomVisibility_t * pCustomVisibility)
			utils::hook::call<void(__fastcall)(void* this_ptr, void* null, ViewCustomVisibility_t*)>(CLIENT_BASE + 0x2BBDA0)
				(model_render::portal2_ptr->m_pLinkedPortal, nullptr, &customVisibility);

			is_using_custom_vis = true;
		}

		utils::hook::call<void(__fastcall)(void* this_ptr, void* null, bool, int, const CViewSetup*, int, int, bool, int, ViewCustomVisibility_t*)>(CLIENT_BASE + 0x1E84E0)
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


	// #
	// Commands

	ConCommand xo_debug_toggle_node_vis_cmd{};
	void xo_debug_toggle_node_vis_fn()
	{
		api::remix_debug_node_vis = !api::remix_debug_node_vis;
	}


	// #
	// #

	main_module::main_module()
	{
		api::init();

		// init d3d font
		{
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
		remix_vars::parse_rtx_options();

		// init addon textures
		model_render::init_texture_addons();

		// #
		// #

		// CModelLoader::Map_LoadModel :: event stub
		utils::hook(ENGINE_BASE + 0xFCD5C, on_map_load_stub).install()->quick();
		HOOK_RETN_PLACE(on_map_load_stub_retn, ENGINE_BASE + 0xFCD61);


		utils::hook::nop(CLIENT_BASE + 0x1ECDC5, 7);
		utils::hook(CLIENT_BASE + 0x1ECDC5, cviewrenderer_renderview_stub).install()->quick();
		HOOK_RETN_PLACE(cviewrenderer_renderview_retn, CLIENT_BASE + 0x1ECDCC);


		// #
		// culling

		game::con_add_command(&xo_debug_toggle_node_vis_cmd, "xo_debug_toggle_node_vis", xo_debug_toggle_node_vis_fn, "Toggle bsp node/leaf debug visualization using the remix api");

		// same as r_novis 1
		// def. needs 'r_portal_stencil_depth 1' if not enabled
		if (flags::has_flag("xo_disable_all_culling"))
		{
			// R_RecursiveWorldNode :: while (node->visframe == r_visframecount .. ) -> renders the entire map if everything after this is enabled
			utils::hook::nop(ENGINE_BASE + 0xE68EF, 6);
		}

		// stub before calling 'R_RecursiveWorldNode' to override node/leaf vis
		utils::hook(ENGINE_BASE + 0xE6D6D, pre_recursive_world_node_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(pre_recursive_world_node_retn, ENGINE_BASE + 0xE6D72);

		// stub within r_recursiveworldnode - called A LOT
		//utils::hook(ENGINE_BASE + 0xE68CC, r_recursiveworldnode_stub, HOOK_JUMP).install()->quick();
		//HOOK_RETN_PLACE(r_recursiveworldnode_retn, ENGINE_BASE + 0xE68D1);
		//utils::hook::set<BYTE>(ENGINE_BASE + 0xE68EF + 1, 0x8C); // jne to jl 0x85 -> 0x8C

		utils::hook(ENGINE_BASE + 0xE68C7, r_recursiveworldnode_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(r_recursiveworldnode_retn, ENGINE_BASE + 0xE68CC);

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

		// Shader_DrawChains :: disable g_pMaterialSystemConfig->nFullbright == 1 check when rendering painted surfaces (binds the "lightmap" (paint map))
		utils::hook::set<BYTE>(ENGINE_BASE + 0xE8C4D, 0xEB);

		// CBrushBatchRender::DrawOpaqueBrushModel :: ^ same for brushmodels
		utils::hook::nop(ENGINE_BASE + 0x7153A, 2);
		utils::hook::set<BYTE>(ENGINE_BASE + 0x71540, 0xEB);


		// #
		// #

		// Fix map visibility when looking through portals when r_portal_stencil_depth == 0
		// - Map_VisSetup called by CViewRender::ViewDrawScene --> CViewRender::SetupVis :: uses player view and 1 visOrigin if no custom vis is provided
		// - Add player vis and call 'CPortalRenderable_FlatBasic::AddToVisAsExitPortal' for both active portals before rendering the main scene
		utils::hook(CLIENT_BASE + 0x1ECF04, viewdrawscene_push_args_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(viewdrawscene_push_args_retn, CLIENT_BASE + 0x1ECF09);
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
