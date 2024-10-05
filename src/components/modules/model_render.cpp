#include "std_include.hpp"

// surface dual render (displacement with blending) gets invisible when skinned objects are moved?
// eg terrain gets invisible if cube is picked up?

namespace components
{
	namespace ff_model
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		IDirect3DBaseTexture9* s_texture;
	}

	//IDirect3DVertexShader9* og_model_shader = nullptr;
	bool is_portalgun_viewmodel = false;

	bool has_materialvar(IMaterialInternal* cmat, const char* var_name, IMaterialVar** out_var = nullptr)
	{
		bool found = false;
		auto var = cmat->vftable->FindVar(cmat, nullptr, var_name, &found, false);

		if (out_var)
		{
			*out_var = var;
		}

		return found;
	}

	// returns true if exists
	bool add_nocull_materialvar(IMaterialInternal* cmat)
	{
		bool found = false;
		auto cullvar = cmat->vftable->FindVar(cmat, nullptr, "$nocull", &found, false);
		//auto varname = cullvar->vftable->GetName(cullvar);

		if (!found)
		{
			utils::function<IMaterialVar* (IMaterialInternal* pMaterial, const char* pKey, int val)> IMaterialVar_Create = MATERIALSTYSTEM_BASE + 0x1A2F0;
			auto var = IMaterialVar_Create(cmat, "$nocull", 1);

			cmat->vftable->AddMaterialVar(cmat, nullptr, var);
			cullvar = cmat->vftable->FindVar(cmat, nullptr, "$nocull", &found, false);
		}

		return found;
	}


	D3DCOLORVALUE g_old_light_to_texture_color = {};

	// only supports 1 saved state for now (should be enough)
	// values also influence radiance (can be larger than 1)
	void add_light_to_texture_color_edit(const float& r, const float& g, const float& b, const float scalar = 1.0f)
	{
		const auto dev = game::get_d3d_device();

		D3DMATERIAL9 temp_mat = {};
		dev->GetMaterial(&temp_mat);

		// save prev. color
		g_old_light_to_texture_color = temp_mat.Diffuse;

		temp_mat.Diffuse = { .r = r * scalar, .g = g * scalar, .b = b * scalar };
		dev->SetMaterial(&temp_mat);
	}

	// restore color
	void add_light_to_texture_color_restore()
	{
		const D3DMATERIAL9 temp_mat = 
		{
			.Diffuse = {.r = g_old_light_to_texture_color.r, .g = g_old_light_to_texture_color.g, .b = g_old_light_to_texture_color.b }
		};

		game::get_d3d_device()->SetMaterial(&temp_mat);
	}


	void __fastcall tbl_hk::model_renderer::DrawModelExecute::Detour(void* ecx, void* edx, void* oo, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		const auto dev = game::get_d3d_device();
		dev->GetVertexShader(&ff_model::s_shader);
		dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));

		D3DMATRIX saved_view = {};
		D3DMATRIX saved_proj = {};


		// MODELFLAG_MATERIALPROXY | MODELFLAG_STUDIOHDR_AMBIENT_BOOST
		if (pInfo.flags == 0x80000011 || pInfo.flags == 0x11)
		{
			VMatrix mat = {};
			mat.m[0][0] = pInfo.pModelToWorld->m_flMatVal[0][0];
			mat.m[1][0] = pInfo.pModelToWorld->m_flMatVal[0][1];
			mat.m[2][0] = pInfo.pModelToWorld->m_flMatVal[0][2];

			mat.m[0][1] = pInfo.pModelToWorld->m_flMatVal[1][0];
			mat.m[1][1] = pInfo.pModelToWorld->m_flMatVal[1][1];
			mat.m[2][1] = pInfo.pModelToWorld->m_flMatVal[1][2];

			mat.m[0][2] = pInfo.pModelToWorld->m_flMatVal[2][0];
			mat.m[1][2] = pInfo.pModelToWorld->m_flMatVal[2][1];
			mat.m[2][2] = pInfo.pModelToWorld->m_flMatVal[2][2];

			mat.m[3][0] = pInfo.pModelToWorld->m_flMatVal[0][3];
			mat.m[3][1] = pInfo.pModelToWorld->m_flMatVal[1][3];
			mat.m[3][2] = pInfo.pModelToWorld->m_flMatVal[2][3];
			mat.m[3][3] = game::identity[3][3];

			dev->SetTransform(D3DTS_WORLD, reinterpret_cast<D3DMATRIX*>(&mat.m)); 
		}
		else if (pInfo.pModel->radius == 37.3153992f) // models/weapons/v_portalgun.mdl
		{
			dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));

			is_portalgun_viewmodel = true;
			if (auto shaderapi = game::get_shaderapi(); shaderapi)
			{
				BufferedState_t buffer_state = {};
				shaderapi->vtbl->GetBufferedState(shaderapi, nullptr, &buffer_state);

				dev->GetTransform(D3DTS_VIEW, &saved_view);
				dev->GetTransform(D3DTS_PROJECTION, &saved_proj);

				dev->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[1]));
				dev->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[2]));
			}
		}

		tbl_hk::model_renderer::table.original<FN>(Index)(ecx, edx, oo, state, pInfo, pCustomBoneToWorld);

		if (is_portalgun_viewmodel)
		{
			dev->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&saved_view));
			dev->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&saved_proj));
		}

		is_portalgun_viewmodel = false;
		dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));
		dev->SetFVF(NULL);

		if (ff_model::s_shader)
		{
			dev->SetVertexShader(ff_model::s_shader);
			ff_model::s_shader = nullptr;
		}

		if (ff_model::s_texture)
		{
			dev->SetTexture(0, ff_model::s_texture);
			ff_model::s_texture = nullptr;
		}
	}

	// #
	// #

	namespace ff_worldmodel
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		D3DMATRIX s_world_mtx = {};
	}

	namespace ff_beamtransport
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		D3DMATRIX s_world_mtx = {};
		D3DMATRIX s_tc_transform = {};
		DWORD s_tc_stage = NULL;
	}

	namespace ff_laserplatform
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		D3DMATRIX s_world_mtx = {};
		D3DMATRIX s_tc_transform = {};
		DWORD s_tc_stage = NULL;
	}

	namespace ff_laser
	{
		IDirect3DVertexShader9* s_shader = nullptr;
	}

	namespace ff_portalfx_01
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		IDirect3DBaseTexture9* s_texture;
		IDirect3DBaseTexture9* s_texture2;
	}

	namespace ff_portalfx_02
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		IDirect3DBaseTexture9* s_texture;
	}

	namespace ff_portalfx_03
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		IDirect3DBaseTexture9* s_texture1;
		IDirect3DBaseTexture9* s_texture2;
	}

	namespace ff_portalfx_04
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		IDirect3DBaseTexture9* s_texture;
	}

	namespace ff_terrain
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		IDirect3DBaseTexture9* s_texture;
	}

	namespace ff_glass_shards
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		IDirect3DBaseTexture9* s_texture;
	}

	namespace ff_more_models
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		IDirect3DBaseTexture9* s_texture;
	}

	namespace ff_vgui
	{
		IDirect3DVertexShader9* s_shader01 = nullptr;
		IDirect3DVertexShader9* s_shader02 = nullptr;
		IDirect3DVertexShader9* s_shader03 = nullptr;
		IDirect3DVertexShader9* s_shader04 = nullptr;
	}

	IDirect3DVertexShader9* saved_shader_unk = nullptr;
	D3DMATRIX saved_world_mtx_unk = {};

	IDirect3DVertexShader9* og_bmodel_shader = nullptr;
	Vector model_org_offset = {};

	

	C_BaseEntity* current_render_ent = nullptr;

	namespace tex_addons
	{
		LPDIRECT3DTEXTURE9 portal_mask;
		LPDIRECT3DTEXTURE9 portal_blue;
		LPDIRECT3DTEXTURE9 portal_orange;
		LPDIRECT3DTEXTURE9 glass_shards;
		LPDIRECT3DTEXTURE9 glass_window_refract;
		LPDIRECT3DTEXTURE9 black_shader;
		LPDIRECT3DTEXTURE9 sky_gray_ft;
		LPDIRECT3DTEXTURE9 sky_gray_bk;
		LPDIRECT3DTEXTURE9 sky_gray_lf;
		LPDIRECT3DTEXTURE9 sky_gray_rt;
		LPDIRECT3DTEXTURE9 sky_gray_up;
		LPDIRECT3DTEXTURE9 sky_gray_dn;
	}

	

	int do_not_render_next_mesh = false;
	bool render_second_pass_with_basetexture2 = false;

	namespace render_next_mesh
	{
		bool with_high_gamma = false;
		bool as_sky = false;

		void reset()
		{
			with_high_gamma = false;
			as_sky = false;
		}
	}

	namespace ff_water
	{
		bool was_water = false;
		IDirect3DVertexShader9* s_shader = nullptr;
		IDirect3DBaseTexture9* s_texture;
	}

	bool render_with_new_stride = false;
	std::uint32_t new_stride = 0u;

	std::uint64_t tick_on_first_no_render = 0;

	// #TODO call from somewhere appropriate
	void model_render::init_texture_addons()
	{
		const auto dev = game::get_d3d_device();

		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_mask.png", &tex_addons::portal_mask);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_blue.png", &tex_addons::portal_blue);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_orange.png", &tex_addons::portal_orange);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\glass_shards.png", &tex_addons::glass_shards);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\glass_window_refract.png", &tex_addons::glass_window_refract);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\black_shader.png", &tex_addons::black_shader);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\graycloud_ft.jpg", &tex_addons::sky_gray_ft);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\graycloud_bk.jpg", &tex_addons::sky_gray_bk);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\graycloud_lf.jpg", &tex_addons::sky_gray_lf);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\graycloud_rt.jpg", &tex_addons::sky_gray_rt);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\graycloud_up.jpg", &tex_addons::sky_gray_up);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\graycloud_dn.jpg", &tex_addons::sky_gray_dn);
	}

	void cmeshdx8_renderpass_pre_draw(CMeshDX8* mesh)
	{
		const auto dev = game::get_d3d_device();

		IDirect3DVertexBuffer9* b = nullptr;
		UINT ofs = 0, stride = 0;
		dev->GetStreamSource(0, &b, &ofs, &stride);

		//Vector* model_org = reinterpret_cast<Vector*>(ENGINE_BASE + 0x50DA90);
		const auto model_to_world_mtx = reinterpret_cast<VMatrix*>(ENGINE_BASE + 0x637158);

		//do_not_render_next_mesh = true;

		VMatrix mtx = {};
		mtx.m[0][0] = model_to_world_mtx->m[0][0];
		mtx.m[1][0] = model_to_world_mtx->m[0][1];
		mtx.m[2][0] = model_to_world_mtx->m[0][2];

		mtx.m[0][1] = model_to_world_mtx->m[1][0];
		mtx.m[1][1] = model_to_world_mtx->m[1][1];
		mtx.m[2][1] = model_to_world_mtx->m[1][2];

		mtx.m[0][2] = model_to_world_mtx->m[2][0];
		mtx.m[1][2] = model_to_world_mtx->m[2][1];
		mtx.m[2][2] = model_to_world_mtx->m[2][2];

		mtx.m[3][0] = model_to_world_mtx->m[0][3];
		mtx.m[3][1] = model_to_world_mtx->m[1][3];
		mtx.m[3][2] = model_to_world_mtx->m[2][3];
		mtx.m[3][3] = game::identity[3][3];

		IMaterialInternal* current_material = nullptr;
		std::string_view current_material_name;
		std::string_view current_shader_name;
		const auto shaderapi = game::get_shaderapi();

		BufferedState_t buffer_state = {};
		shaderapi->vtbl->GetBufferedState(shaderapi, nullptr, &buffer_state);

		if (current_material = shaderapi->vtbl->GetBoundMaterial(shaderapi, nullptr); current_material)
		{
			current_material_name = current_material->vftable->GetName(current_material);
			current_shader_name = current_material->vftable->GetShaderName(current_material);

#if 1
			/*if (current_material_name.contains("slime"))
			{
				int x = 1;
			}*/

			// added format check
			if (mesh->m_VertexFormat == 0x2480033 || mesh->m_VertexFormat == 0x80033)
			{
				if (current_shader_name.contains("Water"))
				{
					IMaterialVar* var = nullptr;
					if (has_materialvar(current_material, "$basetexture", &var))
					{
						// if material has NO defined basetexture
						if (var && !var->vftable->IsDefined(var))
						{
							// check if it has a defined bottommaterial
							var = nullptr;
							const auto has_bottom_mat = has_materialvar(current_material, "$bottommaterial", &var);

							if (!has_bottom_mat /*&& var && var->vftable->IsDefined(var)*/)
							{
								// do not render water surfaces that have no bottom material (this is the surface below the water)
								// could just check $abovewater I guess? lmao

								// we only need one surface
								do_not_render_next_mesh = true;
							}

							// put the normalmap into texture slot 0
							else
							{
								//  BindTexture( SHADER_SAMPLER2, TEXTURE_BINDFLAGS_NONE, NORMALMAP, BUMPFRAME );
								IDirect3DBaseTexture9* tex = shaderapi->vtbl->GetD3DTexture(shaderapi, nullptr, buffer_state.m_BoundTexture[2]);
								if (tex)
								{
									// save og texture
									dev->GetTexture(0, &ff_water::s_texture);
									dev->SetTexture(0, tex);
									ff_water::was_water = true;
								}
							}

							//if (has_materialvar(cmat, "$bottommaterial", &var))
							//{
							//	if (var)
							//	{
							//		// do not render the current surface if there is a bottom material
							//		// we only need one surface
							//		if (var->vftable->IsDefined(var))
							//		{
							//			do_not_render_next_mesh = true;
							//		}
							//		// if there is no bottom material put the normalmap into texture slot 0
							//		else
							//		{
							//			//  BindTexture( SHADER_SAMPLER2, TEXTURE_BINDFLAGS_NONE, NORMALMAP, BUMPFRAME );
							//			IDirect3DBaseTexture9* tex = shaderapi->vtbl->GetD3DTexture(shaderapi, nullptr, buffer_state.m_BoundTexture[2]);
							//			if (tex)
							//			{
							//				dev->SetTexture(0, tex);
							//			}
							//		}
							//	}
						}
						// material has defined a $basetexture
						else
						{
							//  sampler 10
							IDirect3DBaseTexture9* tex = shaderapi->vtbl->GetD3DTexture(shaderapi, nullptr, buffer_state.m_BoundTexture[10]);
							if (tex)
							{
								// save og texture
								dev->GetTexture(0, &ff_water::s_texture);
								dev->SetTexture(0, tex);
								ff_water::was_water = true;
							}
						}
					}
				}
			}
#endif

			//if (current_material_name.contains("water"))
			//{
			//	//do_not_render_next_mesh = true;
			//	int x = 1;
			//}

			//if (current_material_name.contains("toxicslime002a_beneath"))
			//{
			//	//do_not_render_next_mesh = true;
			//	int x = 1;
			//}
		}

		if (og_bmodel_shader && mesh->m_VertexFormat == 0x2480033)
		{
			//do_not_render_next_mesh = true;

			dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));

			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX7);
			dev->SetVertexShader(nullptr);
		}

		// player model - gun - grabable stuff - portal button - portal door - stairs
		else if (ff_model::s_shader && mesh->m_VertexFormat == 0xa0003)
		{
			//do_not_render_next_mesh = true;
		
			if (current_material_name.contains("props_destruction/glass_"))
			{
				//do_not_render_next_mesh = true;
				if (tex_addons::glass_shards)
				{
					// this can cause some issues with other glass textures?!
					// a prob. because: models/props_destruction/glass_fracture_a_inner
					dev->GetTexture(0, &ff_model::s_texture); // save and restore tex after render
					dev->SetTexture(0, tex_addons::glass_shards);
				}
			}
			/*else if (cname.contains("chell"))
			{
				do_not_render_next_mesh = true;
			}*/

			//if (!is_portalgun_viewmodel)
			{
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
			}

			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX6);
			dev->SetVertexShader(nullptr); // vertexformat 0x00000000000a0003 

			//mtx[3][2] = sinf((float)main_module::framecount * 0.05f) * 10.0f;
		}

		// this also renders the glass? infront of white panel lamps
		// also renders some foliage (2nd level - emissive)
		else if (ff_model::s_shader) // should be stride 30
		{
			// 0xa0103
			// do_not_render_next_mesh = true;

			// replace all refract shaders with wireframe
			if (current_shader_name.contains("Refract_DX90"))
			{
				// I think we are simply missing basetex0 here
				current_material->vftable->SetShader(current_material, "Wireframe");
			}

			if (current_material_name.contains("glasswindow_refract"))
			{
				if (tex_addons::glass_window_refract)
				{
					dev->GetTexture(0, &ff_model::s_texture);
					dev->SetTexture(0, tex_addons::glass_window_refract);
				}
			}

			if (current_shader_name.contains("Black"))
			{
				dev->SetTexture(0, tex_addons::black_shader);
			}

			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3);
			dev->SetVertexShader(nullptr); // vertexformat 0x00000000000a0003
		}
		else
		{
			bool was_portal_related = false;

			// r_portal_stencil_depth 0 heavy influence
			if (current_material_name.contains("portal_stencil"))
			{
				was_portal_related = true; // prevent all other else's
				do_not_render_next_mesh = true;
			}
			else if (current_material_name.contains("portalstaticoverlay_1"))
			{
				if (tex_addons::portal_mask)
				{
					dev->GetTexture(1, &ff_portalfx_03::s_texture2);
					dev->SetTexture(1, tex_addons::portal_mask);
				}

				if (tex_addons::portal_blue)
				{
					dev->GetTexture(0, &ff_portalfx_03::s_texture1);
					dev->SetTexture(0, tex_addons::portal_blue);
				}

				// replace with wireframe (makes life much easier)
				if (current_shader_name != "Wireframe_DX9")
				{
					current_material->vftable->SetShader(current_material, "Wireframe");
				}
				
				//do_not_render_next_mesh = true;
				was_portal_related = true;
				model_render::portal1_render_count++;
			}
			else if (current_material_name.contains("portalstaticoverlay_2"))
			{
				{
					if (tex_addons::portal_mask)
					{
						dev->GetTexture(1, &ff_portalfx_03::s_texture2);
						dev->SetTexture(1, tex_addons::portal_mask);
					}

					if (tex_addons::portal_orange)
					{
						dev->GetTexture(0, &ff_portalfx_03::s_texture1);
						dev->SetTexture(0, tex_addons::portal_orange);
					}
				}
				
				// replace with wireframe (makes life much easier)
				if (current_shader_name != "Wireframe_DX9")
				{
					current_material->vftable->SetShader(current_material, "Wireframe");
				}

				//do_not_render_next_mesh = true;
				was_portal_related = true;
				model_render::portal2_render_count++;
			}

			if (was_portal_related) 
			{
				// through wall overlays
				if (mesh->m_VertexFormat == 0xa0007)
				{
					int break_me = 1;
					//do_not_render_next_mesh = true;
					//dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX5 | D3DFVF_TEXCOORDSIZE1(4)); // 68 - 4 as last tc is one float
					//dev->GetVertexShader(&ff_portalfx_01::s_shader);
					//dev->SetVertexShader(nullptr);
					//dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));
				}

				// draws portal stencil hole
				if (mesh->m_VertexFormat == 0x4a0003)
				{
					//do_not_render_next_mesh = true;

					dev->GetVertexShader(&ff_portalfx_03::s_shader);
					dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64
					dev->SetVertexShader(nullptr);
					dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
				}

				// if set to wireframe mode 
				if (mesh->m_VertexFormat == 0x80003)
				{
					//do_not_render_next_mesh = true;
					dev->GetVertexShader(&ff_portalfx_03::s_shader);
					dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));
					dev->SetVertexShader(nullptr);
					dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
				}

				// fizzle
				if (mesh->m_VertexFormat == 0xa0003)
				{
					int break_me = 1; 
				}
			}
					
				
			

			if (was_portal_related)
			{
				int break_me = 0;
			}

			// world geo - floor / walls --- "LightmappedGeneric"
			// this renders water but not the $bottommaterial
			else if (mesh->m_VertexFormat == 0x2480033)
			{
				//do_not_render_next_mesh = true;

				// tc @ 24
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3);
				
				if (ff_water::was_water)
				{
					dev->GetVertexShader(&ff_water::s_shader);
				}
				else
				{
					dev->GetVertexShader(&ff_worldmodel::s_shader);
				}

				dev->SetVertexShader(nullptr);
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));

#if 0			// can be used to look into the vertex buffer to figure out the layout
				{
					IDirect3DVertexBuffer9* buff = nullptr;
					UINT t_stride = 0u, t_offset = 0u;
					dev->GetStreamSource(0, &buff, &t_offset, &t_stride);

					// looking at the data, the actual stride seems to be 48?

					void* buffer_data;
					if (buff) 
					{
						if (const auto hr = buff->Lock(0, t_stride * 10u, &buffer_data, D3DLOCK_READONLY); hr >= 0)
						{
							buff->Unlock(); // break here
						}
					}
				}
#endif
			}
			// transporting beams
			else if (mesh->m_VertexFormat == 0x80005) // stride 0x20
			{
				//do_not_render_next_mesh = true;

				dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2);
				dev->GetVertexShader(&ff_beamtransport::s_shader); 
				dev->SetVertexShader(nullptr);

				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));

				dev->GetTransform(D3DTS_TEXTURE0, &ff_beamtransport::s_tc_transform);
				//dev->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, &beamtransport::s_tc_stage);

				// tc scroll
				D3DXMATRIX ret = ff_beamtransport::s_tc_transform;
				ret(3, 1) = (float)main_module::framecount * 0.0015f;

				dev->SetTransform(D3DTS_TEXTURE0, &ret);
				dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			}

			// laser platforms + DebugTextureView
			// renders a small quad at 0 0 0 ?
			else if (mesh->m_VertexFormat == 0x80001)
			{
				//do_not_render_next_mesh = true;

				// SolidEnergy_dx9
				if (current_shader_name.starts_with("SolidEn"))
				{
					// tc @ 12
					dev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(1)); // missing 4 bytes at the end here - fixed with tc2 size 3?
					dev->GetVertexShader(&ff_laserplatform::s_shader);
					dev->SetVertexShader(nullptr);

					dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));
					dev->GetTransform(D3DTS_TEXTURE0, &ff_laserplatform::s_tc_transform);
					//dev->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, &laserplatform::s_tc_stage);

					// tc scroll
					D3DXMATRIX ret = ff_laserplatform::s_tc_transform;
					ret(3, 1) = (float)main_module::framecount * 0.01f;

					dev->SetTransform(D3DTS_TEXTURE0, &ret);
					dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
				}
				else
				{
					do_not_render_next_mesh = true;
				}
			}

			// lasers - indicator dots - some of the white light stripes
			// this also renders the the wireframe portal if hook enabled to render wireframe for portals
			// also renders white stuff behind sliding doors
			// treeleaf shader
			// + skybox
			else if (mesh->m_VertexFormat == 0x80003)
			{
				//do_not_render_next_mesh = true;

				if (current_material_name.contains("light_panel_"))
				{
					add_nocull_materialvar(current_material);
				}
				// TODO - create actual portals for this?
				// requires portal stencil depth of at least 1
				else if (current_material_name.contains("decals/portalstencildecal"))
				{
					//do_not_render_next_mesh = true;

					// #TODO set unique texture
					dev->SetTexture(0, tex_addons::portal_mask);
				}
				// unique textures for the white sky so they can be marked
				else if (current_material_name.contains("sky"))
				{
					if (current_material_name.contains("_white"))
					{
						if (current_material_name.contains("eft")) // sky_whiteft
						{
							dev->SetTexture(0, tex_addons::sky_gray_ft);
						}
						else if (current_material_name.contains("ebk"))
						{
							dev->SetTexture(0, tex_addons::sky_gray_bk);
						}
						else if (current_material_name.contains("elf"))
						{
							dev->SetTexture(0, tex_addons::sky_gray_lf);
						}
						else if (current_material_name.contains("ert"))
						{
							dev->SetTexture(0, tex_addons::sky_gray_rt);
						}
						else if (current_material_name.contains("eup"))
						{
							dev->SetTexture(0, tex_addons::sky_gray_up);
						}
						else if (current_material_name.contains("edn"))
						{
							dev->SetTexture(0, tex_addons::sky_gray_dn);
						}

						render_next_mesh::as_sky = true;
					}
				}

				dev->GetVertexShader(&ff_laser::s_shader);

				// noticed some normal issues on vgui_indicator's .. disable normals for now?
				dev->SetFVF(D3DFVF_XYZB3 /*| D3DFVF_NORMAL*/ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));
				dev->SetVertexShader(nullptr);

				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[0]));
				//dev->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[1]));
				//dev->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[2]));
			}

			// portal_draw_ghosting 0 disables this
			// this normally shows portals through walls
			else if (mesh->m_VertexFormat == 0xa0007) // portal fx
			{
				//do_not_render_next_mesh = true;

				// tc at 16 + 12 
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX5 | D3DFVF_TEXCOORDSIZE1(4)); // 68 - 4 as last tc is one float
				dev->GetVertexShader(&ff_portalfx_01::s_shader);

				dev->SetVertexShader(nullptr);
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity)); // 0x6c0005 influences this one here???
			}

			// related to props like portalgun, pickup-ables
			// does not have a visibile texture set? (setting one manually makes it show up)
			// renders decals/simpleshadow
			else if (mesh->m_VertexFormat == 0x6c0005)
			{
				do_not_render_next_mesh = true;

				// stride 48
				dev->SetFVF(D3DFVF_XYZB1 | D3DFVF_TEX5);
				dev->GetVertexShader(&saved_shader_unk);
				dev->SetVertexShader(nullptr);

				// this disables rendering - why not just not render
				//float mtx[4][4] = {};
				//dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
			}

			// white overlay on portals? - getting tex1 and setting it onto tex0 shows
			// a texture (prob. lmap) that matches the tiny portals
			// does not look like its responsible for the tiny portals tho


			// this would draw the portal1 and portal2 mesh (but we already do that way above)
			// can still be used on some levels (eg sp_a2_bridge_intro)
			// portal refract texture
			else if (mesh->m_VertexFormat == 0x4a0003)
			{
				do_not_render_next_mesh = true;

				dev->GetVertexShader(&ff_portalfx_03::s_shader); 
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64
				dev->SetVertexShader(nullptr);
			}

			// fizzle texture that breaks everything and is normally ignored?
			// engine/shadowbuild
			else if (mesh->m_VertexFormat == 0xa0003)
			{
				do_not_render_next_mesh = true;

				// stride 0x30
				dev->GetVertexShader(&ff_portalfx_02::s_shader);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX5 | D3DFVF_TEXCOORDSIZE1(4));

				// def. render using FF as the shader is causing heavy frametime drops
				dev->SetVertexShader(nullptr);

				// 0 scale gets rid of it (disable to make it show up)
				float null_mtx[4][4] = {};
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&null_mtx));
			}

			// HUD
			else if (mesh->m_VertexFormat == 0x80007) 
			{
				//do_not_render_next_mesh = true;
				
				// always render UI and world ui with high gamma?
				render_next_mesh::with_high_gamma = true;

				if (current_material_name.contains("vgui__fontpage"))
				{
					// get rid of all world-rendered text as its using the same glyph as HUD elements?!
					if (buffer_state.m_Transform[0].m[3][0] != 0.0f)
					{
						do_not_render_next_mesh = true;
					}
				}
				/*else if (current_material_name.contains("vgui_white"))
				{
					int x = 1;
				}*/
				else if (current_material_name.contains("vgui_coop_progress_board")
					  || current_material_name.contains("p2_lightboard_vgui")
					  || current_material_name.contains("elevator_video_overlay"))
				{
					//do_not_render_next_mesh = true;
					dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[0]));

					// no need to set fvf here?
					//dev->SetFVF(D3DFVF_XYZB3 | D3DFVF_TEX4);

					dev->GetVertexShader(&ff_vgui::s_shader01);
					dev->SetVertexShader(nullptr);
				}

				// video on intro3
				else if (current_material_name.contains("elevator_video_lines"))
				{
					dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[0]));

					//IDirect3DBaseTexture9* vid = nullptr;
					//dev->GetTexture(1, &vid);
					//IDirect3DBaseTexture9* vid = shaderapi->vtbl->GetD3DTexture(shaderapi, nullptr, state.m_BoundTexture[0]);
					//dev->SetTexture(0, vid);

					dev->GetVertexShader(&ff_vgui::s_shader01);
					dev->SetVertexShader(nullptr);
				}
				/*else if (current_material_name.contains("loading_screens"))
				{
					render_next_mesh::with_high_gamma = true;
				}
				else if (sname.starts_with("console/"))
				{
					render_next_mesh::with_high_gamma = true;
				}*/
			}

			// on portal open - spark fx (center)
			// also portal clearing gate (blue sweeping beam)
			// + portal gun pickup effect
			// can be rendered but also requires vertexshader + position
			else if (mesh->m_VertexFormat == 0x924900005) // stride 0x70 - 112
			{
				//do_not_render_next_mesh = true;

				/*float constant[4] = {};  
				dev->GetPixelShaderConstantF(5, constant, 1);

				float constant2[4] = {};
				dev->GetPixelShaderConstantF(6, constant2, 1);*/

				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[0]));
				dev->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[1]));
				dev->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[2]));

				//dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3); // 80
				//dev->GetVertexShader(&ff_vgui::s_shader04);
				//dev->SetVertexShader(nullptr);

				//dev->GetTexture(0, &ff_water::s_texture); 
				//dev->SetTexture(0, tex_addons::portal_mask);
				//ff_water::was_water = true;
				//render_next_mesh::with_high_gamma = false;

#if 0			// can be used to look into the vertex buffer to figure out the layout
				{
					IDirect3DVertexBuffer9* buff = nullptr;
					UINT t_stride = 0u, t_offset = 0u;
					dev->GetStreamSource(0, &buff, &t_offset, &t_stride);

					void* buffer_data;
					if (buff)
					{
						if (const auto hr = buff->Lock(0, 48u * 100u, &buffer_data, D3DLOCK_READONLY); hr >= 0)
						{
							buff->Unlock(); // break here
						}
					}
				}
#endif
			}

			// ? bed
			else if (mesh->m_VertexFormat == 0xa0103)  
			{
				//do_not_render_next_mesh = true;
				// tc @ 24
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64
				dev->GetVertexShader(&ff_more_models::s_shader);
				dev->SetVertexShader(nullptr);
			}

			// terrain - "WorldVertexTransition"
			else if (mesh->m_VertexFormat == 0x480007)
			{
				//do_not_render_next_mesh = true;

				if (current_shader_name.contains("WorldVertexTransition_DX9"))
				{
					render_second_pass_with_basetexture2 = true;
				}

				// m_BoundTexture[7]  = first blend colormap
				// m_BoundTexture[12] = second blend colormap
				
				// if envmap		:: VERTEX_TANGENT_S | VERTEX_TANGENT_T | VERTEX_NORMAL is set
				// if basetex2		:: vertex color is set
				// if bumpmap		:: tc count = 3 ... else 2

				// texcoord0 : base texcoord
				// texcoord1 : lightmap texcoord
				// texcoord2 : lightmap texcoord offset
				
#if 0			// can be used to look into the vertex buffer to figure out the layout
				{
					IDirect3DVertexBuffer9* buff = nullptr;
					UINT t_stride = 0u, t_offset = 0u;
					dev->GetStreamSource(0, &buff, &t_offset, &t_stride);

					void* buffer_data;
					if (buff)
					{
						if (const auto hr = buff->Lock(0, 48u * 100u, &buffer_data, D3DLOCK_READONLY); hr >= 0)
						{
							buff->Unlock(); // break here
						}
					}
				}
#endif
				// not doing this and picking up a skinned model (eg. cube) will break displacement rendering???
				dev->SetTransform(D3DTS_WORLD, &buffer_state.m_Transform[0]);

				// tc @ 28
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE1(2));
				dev->GetVertexShader(&ff_terrain::s_shader);
				dev->SetVertexShader(nullptr); 
			}

			// hanging cables - requires vertex shader - verts not modified on the cpu
			else if (mesh->m_VertexFormat == 0x24900005)
			{
				do_not_render_next_mesh = true; // they can freak out sometimes so just ignore them for now
				
#if 0
				// do not set fvf
				//dev->SetFVF(D3DFVF_XYZB5 | D3DFVF_NORMAL | D3DFVF_TEX2); // tc @ 28
				dev->GetVertexShader(&ff_terrain::s_shader);
				dev->SetPixelShader(nullptr);
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[0]));
#endif
			}

			// SpriteCard shader
			// on portal open
			// portal gun pickup effect (pusling lights (not the beam))
			// other particles like smoke - wakeup scene ring - water splash
			else if (mesh->m_VertexFormat == 0x114900005) // stride 96 
			{
				//do_not_render_next_mesh = true;

#if 0			// can be used to look into the vertex buffer to figure out the layout
				{
					IDirect3DVertexBuffer9* buff = nullptr;
					UINT t_stride = 0u, t_offset = 0u;
					dev->GetStreamSource(0, &buff, &t_offset, &t_stride);

					void* buffer_data;
					if (buff)
					{
						if (const auto hr = buff->Lock(0, t_stride * 2, &buffer_data, D3DLOCK_READONLY); hr >= 0)
						{
							buff->Unlock(); // break here
						}
					}

					//dev->SetStreamSource(0, buff, t_offset, 48*4); // times 48*4 is almost good lmao
					//render_with_new_stride = true;
				}
#endif

				//dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[0]));  
				//dev->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[1]));
				//dev->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[2]));

				//dev->GetVertexShader(&ff_portalfx_04::s_shader); 
				//dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3); // vertex fmt looks like: pos - normal - 3x texcoord (float2) = 48 byte
				//dev->SetVertexShader(nullptr);

				//dev->SetTexture(0, tex_addons::portal_mask);
			}

			// on portal open - blob in the middle (impact)
			else if (mesh->m_VertexFormat == 0x80037) // TODO - test with buffer_state transforms 
			{
				//do_not_render_next_mesh = true;
				// this needs a position as it spawns on 0 0 0
				// stride 0x40

				int break_me = 0;

				//dev->GetVertexShader(&ff_portalfx_04::s_shader);
				//dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX5);
				//dev->SetVertexShader(nullptr);

				// nope
				/*if (current_render_ent)
				{
					float mtx[4][4] = {};
					mtx[0][0] = game::identity[0][0];
					mtx[0][1] = game::identity[0][1];
					mtx[0][2] = game::identity[0][2];
					mtx[0][3] = game::identity[0][3];

					mtx[1][0] = game::identity[1][0];
					mtx[1][1] = game::identity[1][1];
					mtx[1][2] = game::identity[1][2];
					mtx[1][3] = game::identity[1][3];

					mtx[2][0] = game::identity[2][0];
					mtx[2][1] = game::identity[2][1];
					mtx[2][2] = game::identity[2][2];
					mtx[2][3] = game::identity[2][3];

					mtx[3][0] = current_render_ent->m_vecAbsOrigin.x;
					mtx[3][1] = current_render_ent->m_vecAbsOrigin.y;
					mtx[3][2] = current_render_ent->m_vecAbsOrigin.z;
					mtx[3][3] = game::identity[3][3];
					dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
				}*/

				//dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));
				
			}

			// on portal open - outer ring
			else if (mesh->m_VertexFormat == 0x1b924900005) // stride 0x90 - 144
			{
				//do_not_render_next_mesh = true;
			}

			// portal clearing gate
			// renders water $bottommaterial
			else if (mesh->m_VertexFormat == 0x80033) //stride = 0x40 
			{
				//do_not_render_next_mesh = true;

				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[0]));

				// tc @ 24
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64
				dev->GetVertexShader(&ff_vgui::s_shader04);
				dev->SetVertexShader(nullptr);
			}

			// decals
			else if (mesh->m_VertexFormat == 0x2480037)  // stride 0x50 - 80 
			{
				//do_not_render_next_mesh = true;

				// tc at 28
				dev->SetFVF(D3DFVF_XYZB4 | D3DFVF_TEX7 | D3DFVF_TEXCOORDSIZE1(4)); // 84 - 4 as last tc is one float
				dev->GetVertexShader(&ff_vgui::s_shader03);
				dev->SetVertexShader(nullptr);
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[0]));
			}

			// Sprite shader
			else if (mesh->m_VertexFormat == 0x914900005)
			{
				//do_not_render_next_mesh = true; 

#if 0			// can be used to look into the vertex buffer to figure out the layout
				{
					IDirect3DVertexBuffer9* buff = nullptr;
					UINT t_stride = 0u, t_offset = 0u;
					dev->GetStreamSource(0, &buff, &t_offset, &t_stride);

					void* buffer_data;
					if (buff)
					{
						if (const auto hr = buff->Lock(0, 48u * 100u, &buffer_data, D3DLOCK_READONLY); hr >= 0)
						{
							buff->Unlock(); // break here
						}
					}

					//dev->SetStreamSource(0, buff, t_offset, 112 * 4); // times 48*4 is almost good lmao
					//render_with_new_stride = true;
				}
#endif
				//dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[0]));
				//dev->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[1]));
				//dev->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[2]));
				//dev->GetVertexShader(&ff_portalfx_04::s_shader);
				//dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3); // vertex fmt looks like pos normal 3xtc (float2)
				//dev->SetVertexShader(nullptr);

				//dev->SetTexture(0, tex_addons::portal_mask);
			}

			// SpriteCard vista smoke 
			else if (mesh->m_VertexFormat == 0x24914900005) // stride 144 ....
			{
				// flipbooks not visible at all so disable them
				//do_not_render_next_mesh = true;

				//IDirect3DBaseTexture9* tex = shaderapi->vtbl->GetD3DTexture(shaderapi, nullptr, buffer_state.m_BoundTexture[1]);
				//if (tex)
				//{
				//	// save og texture
				//	dev->GetTexture(0, &ff_water::s_texture);
				//	dev->SetTexture(0, tex);
				//}

				//dev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX7 | D3DFVF_TEXCOORDSIZE1(4)); // 84 - 4 as last tc is one float
				//dev->GetVertexShader(&ff_vgui::s_shader03);
				//dev->SetVertexShader(nullptr);
				//dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[0]));
				//dev->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[1]));
				//dev->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&buffer_state.m_Transform[2]));
			}
			else
			{
				//do_not_render_next_mesh = true;
				int break_me = 1;  
			}

			int break_me = 1;
		}
	}

	HOOK_RETN_PLACE_DEF(cmeshdx8_renderpass_pre_draw_retn_addr);
	void __declspec(naked) cmeshdx8_renderpass_pre_draw_stub()
	{
		__asm
			{
			pushad;
			push esi; // CMeshDX8
			call cmeshdx8_renderpass_pre_draw;
			add esp, 4;
			popad;

			// og code
			mov ecx, [esi + 0x4C];
			test ecx, ecx;
			jmp cmeshdx8_renderpass_pre_draw_retn_addr;
			}
	}


	// #

	//void cmeshdx8_renderpass_post_draw(std::uint32_t num_prims_rendered)
	void cmeshdx8_renderpass_post_draw([[maybe_unused]] void* device_ptr, D3DPRIMITIVETYPE type, std::int32_t base_vert_index, std::uint32_t min_vert_index, std::uint32_t num_verts, std::uint32_t start_index, std::uint32_t prim_count)
	{
		const auto dev = game::get_d3d_device();

		// 0 = Gamma 1.0 (fixes dark albedo) :: 1 = Gamma 2.2
		dev->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, render_next_mesh::with_high_gamma ? 1u : 0u);
		

		// do not render next surface if set
		if (!do_not_render_next_mesh)
		{
			if (render_with_new_stride)
			{
				num_verts *= 1;
				prim_count *= 1;
				type = D3DPT_TRIANGLELIST;
			}

			//bool skip_other_sky_surfs = false;

			DWORD og_texfactor = {}, og_colorarg2 = {}, og_colorop = {};
			if (render_next_mesh::as_sky)
			{
				// uh
				//dev->SetRenderState(D3DRS_FOGENABLE, FALSE);

				// HACK - as long as sky marking is broken, use WORLD SPACE UI (emissive)
				// -> means that we can not use a distant light
				// -> this reduces the emissive intensity
				dev->GetRenderState(D3DRS_TEXTUREFACTOR, &og_texfactor);
				dev->GetTextureStageState(0, D3DTSS_COLORARG2, &og_colorarg2);
				dev->GetTextureStageState(0, D3DTSS_COLOROP, &og_colorop);
				
				dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(25, 25, 25, 255));
				dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
				dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

				/*if (!model_render::rendered_first_sky_surface)
				{
					model_render::rendered_first_sky_surface = true;
					num_verts *= 6;
					prim_count *= 6;
				}
				else
				{
					skip_other_sky_surfs = true;
				}*/
			}

			//if (!skip_other_sky_surfs)
			dev->DrawIndexedPrimitive(type, base_vert_index, min_vert_index, num_verts, start_index, prim_count);

			// restore emissive sky settings
			if (render_next_mesh::as_sky)
			{
				dev->SetRenderState(D3DRS_TEXTUREFACTOR, og_texfactor);
				dev->SetTextureStageState(0, D3DTSS_COLORARG2, og_colorarg2);
				dev->SetTextureStageState(0, D3DTSS_COLOROP, og_colorop);
			}
		}

		// debug renderstates and texturestages
#if 0	
		auto x = game::get_cshaderapi();

		D3DSHADEMODE shademode;
		dev->GetRenderState(D3DRS_SHADEMODE, (DWORD*)&shademode);

		DWORD alpharef, alphafunc;
		dev->GetRenderState(D3DRS_ALPHAREF, (DWORD*)&alpharef);
		dev->GetRenderState(D3DRS_ALPHAFUNC, (DWORD*)&alphafunc);

		D3DBLEND srcblend, destblend;
		dev->GetRenderState(D3DRS_SRCBLEND, (DWORD*)&srcblend);
		dev->GetRenderState(D3DRS_DESTBLEND, (DWORD*)&destblend);

		D3DBLENDOP blendop, blendop_alpha;
		dev->GetRenderState(D3DRS_BLENDOP, (DWORD*)&blendop);
		dev->GetRenderState(D3DRS_BLENDOPALPHA, (DWORD*)&blendop_alpha);

		D3DCOLOR blend_factor;
		dev->GetRenderState(D3DRS_BLENDFACTOR, (DWORD*)&blend_factor);

		D3DCOLOR texfactor;
		dev->GetRenderState(D3DRS_TEXTUREFACTOR, (DWORD*)&texfactor);

		DWORD colorvertex, vertexblend, colorwrite1, colorwrite2, colorwrite3, srgbwrite;
		dev->GetRenderState(D3DRS_COLORVERTEX, (DWORD*)&colorvertex);
		dev->GetRenderState(D3DRS_VERTEXBLEND, (DWORD*)&vertexblend);
		dev->GetRenderState(D3DRS_COLORWRITEENABLE1, (DWORD*)&colorwrite1);
		dev->GetRenderState(D3DRS_COLORWRITEENABLE2, (DWORD*)&colorwrite2);
		dev->GetRenderState(D3DRS_COLORWRITEENABLE3, (DWORD*)&colorwrite3);
		dev->GetRenderState(D3DRS_SRGBWRITEENABLE, (DWORD*)&srgbwrite);

		DWORD sepalphablend;
		D3DBLENDOP srcblendalpha, destblendalpha;
		dev->GetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, (DWORD*)&sepalphablend);
		dev->GetRenderState(D3DRS_SRCBLENDALPHA, (DWORD*)&srcblendalpha);
		dev->GetRenderState(D3DRS_DESTBLENDALPHA, (DWORD*)&destblendalpha);

		DWORD tss_colorarg0, tss_colorarg1, tss_colorarg2, tss_alphaarg0, tss_alphaarg1, tss_alphaarg2;
		dev->GetTextureStageState(0, D3DTSS_COLORARG0, &tss_colorarg0);
		dev->GetTextureStageState(0, D3DTSS_COLORARG1, &tss_colorarg1);
		dev->GetTextureStageState(0, D3DTSS_COLORARG2, &tss_colorarg2);
		dev->GetTextureStageState(0, D3DTSS_ALPHAARG0, &tss_alphaarg0);
		dev->GetTextureStageState(0, D3DTSS_ALPHAARG0, &tss_alphaarg0);
		dev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &tss_alphaarg1);
		dev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &tss_alphaarg2);

		D3DTEXTUREOP tss_colorop, tss_alphaop;
		dev->GetTextureStageState(0, D3DTSS_COLOROP, (DWORD*)&tss_colorop);
		dev->GetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD*)&tss_alphaop);

		DWORD sampler_gamma;
		dev->GetSamplerState(0, D3DSAMP_SRGBTEXTURE, &sampler_gamma);
		dev->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, 0);
#endif

		// render the current surface a second time (alpha blended) if set
		// only works with shaders using basemap2 in sampler7
		if (render_second_pass_with_basetexture2)
		{
			BufferedState_t state = {};
			const auto shaderapi = game::get_shaderapi();
			shaderapi->vtbl->GetBufferedState(shaderapi, nullptr, &state);

			// check if basemap2 is assigned
			if (state.m_BoundTexture[7])
			{
				// save texture, renderstates and texturestates

				IDirect3DBaseTexture9* og_tex0 = nullptr;
				dev->GetTexture(0, &og_tex0);

				DWORD og_alphablend = {};
				dev->GetRenderState(D3DRS_ALPHABLENDENABLE, &og_alphablend);

				DWORD og_alphaop = {}, og_alphaarg1 = {}, og_alphaarg2 = {};
				dev->GetTextureStageState(0, D3DTSS_ALPHAOP, &og_alphaop);
				dev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &og_alphaarg1);
				dev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &og_alphaarg2);

				DWORD og_colorop = {}, og_colorarg1 = {}, og_colorarg2 = {};
				dev->GetTextureStageState(0, D3DTSS_COLOROP, &og_colorop);
				dev->GetTextureStageState(0, D3DTSS_COLORARG1, &og_colorarg1);
				dev->GetTextureStageState(0, D3DTSS_COLORARG2, &og_colorarg2);

				DWORD og_srcblend = {}, og_destblend = {};
				dev->GetRenderState(D3DRS_SRCBLEND, &og_srcblend);
				dev->GetRenderState(D3DRS_DESTBLEND, &og_destblend);


				

				// assign basemap2 to textureslot 0
				if (const auto basemap2 = shaderapi->vtbl->GetD3DTexture(shaderapi, nullptr, state.m_BoundTexture[7]);
					basemap2)
				{
					dev->SetTexture(0, basemap2);
					//dev->SetTexture(0, tex_addons::portal_mask);
				}

				// enable blending
				dev->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);

				// picking up / moving a cube affects this and causes flickering on the blended surface
				dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

				// can be used to lighten up the albedo and add a little more alpha
				dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0, 30));
				dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
				dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);

				// add a little more alpha
				dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
				dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
				dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_ADD);

				//state.m_Transform[0].m[3][2] += 40.0f;
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&state.m_Transform[0]));

				// draw second surface 
				dev->DrawIndexedPrimitive(type, base_vert_index, min_vert_index, num_verts, start_index, prim_count);

				// restore texture, renderstates and texturestates
				dev->SetTexture(0, og_tex0);
				dev->SetRenderState(D3DRS_ALPHABLENDENABLE, og_alphablend);
				dev->SetRenderState(D3DRS_SRCBLEND, og_srcblend);
				dev->SetRenderState(D3DRS_DESTBLEND, og_destblend);
				dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, og_alphaarg1);
				dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, og_alphaarg2);
				dev->SetTextureStageState(0, D3DTSS_ALPHAOP, og_alphaop);
				dev->SetTextureStageState(0, D3DTSS_COLORARG1, og_colorarg1);
				dev->SetTextureStageState(0, D3DTSS_COLORARG2, og_colorarg2);
				dev->SetTextureStageState(0, D3DTSS_COLOROP, og_colorop);
			}
		}

		if (ff_model::s_texture)
		{
			dev->SetTexture(0, ff_model::s_texture);
			ff_model::s_texture = nullptr;
		}

		if (saved_shader_unk)
		{
			dev->SetVertexShader(saved_shader_unk);
			dev->SetFVF(NULL);
			saved_shader_unk = nullptr;
		}

		if (ff_terrain::s_shader)
		{
			dev->SetVertexShader(ff_terrain::s_shader);
			dev->SetFVF(NULL);
			ff_terrain::s_shader = nullptr;
		}

		if (ff_water::s_shader)
		{
			dev->SetVertexShader(ff_water::s_shader);
			dev->SetFVF(NULL);
			ff_water::s_shader = nullptr;
		}

		if (ff_water::was_water)
		{
			dev->SetTexture(0, ff_water::s_texture);
			ff_water::s_texture = nullptr;
			ff_water::was_water = false;
		}

		if (ff_worldmodel::s_shader)
		{
			dev->SetVertexShader(ff_worldmodel::s_shader);
			dev->SetFVF(NULL);
			ff_worldmodel::s_shader = nullptr;
		}

		if (ff_beamtransport::s_shader)
		{
			dev->SetVertexShader(ff_beamtransport::s_shader);
			dev->SetFVF(NULL);
			ff_beamtransport::s_shader = nullptr;

			dev->SetTransform(D3DTS_TEXTURE0, &ff_beamtransport::s_tc_transform);
			dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE); //beamtransport::s_tc_stage);
		}

		if (ff_laserplatform::s_shader)
		{
			dev->SetVertexShader(ff_laserplatform::s_shader);
			dev->SetFVF(NULL);
			ff_laserplatform::s_shader = nullptr;

			dev->SetTransform(D3DTS_TEXTURE0, &ff_laserplatform::s_tc_transform);
			dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE); //laserplatform::s_tc_stage);
		}

		if (ff_laser::s_shader)
		{
			dev->SetVertexShader(ff_laser::s_shader);
			dev->SetFVF(NULL);
			ff_laser::s_shader = nullptr;
		}

		if (ff_portalfx_01::s_shader)
		{
			/*if (ff_portalfx_01::s_texture)
			{
				dev->SetTexture(0, ff_portalfx_01::s_texture);
				ff_portalfx_01::s_texture = nullptr;
			}

			if (ff_portalfx_01::s_texture2)
			{
				dev->SetTexture(1, ff_portalfx_01::s_texture2);
				ff_portalfx_01::s_texture2 = nullptr;
			}*/

			dev->SetVertexShader(ff_portalfx_01::s_shader);
			dev->SetFVF(NULL);
			ff_portalfx_01::s_shader = nullptr;
		}

		if (ff_portalfx_02::s_shader)
		{
			if (ff_portalfx_02::s_texture)
			{
				dev->SetTexture(1, ff_portalfx_02::s_texture);
				ff_portalfx_02::s_texture = nullptr;
			}

			dev->SetVertexShader(ff_portalfx_02::s_shader);
			dev->SetFVF(NULL);
			ff_portalfx_02::s_shader = nullptr;
		}

		if (ff_portalfx_03::s_shader)
		{
			if (ff_portalfx_03::s_texture1)
			{
				dev->SetTexture(0, ff_portalfx_03::s_texture1);
				ff_portalfx_03::s_texture1 = nullptr;
			}

			if (ff_portalfx_03::s_texture2)
			{
				dev->SetTexture(0, ff_portalfx_03::s_texture2);
				ff_portalfx_03::s_texture2 = nullptr;
			}

			dev->SetVertexShader(ff_portalfx_03::s_shader);
			dev->SetFVF(NULL);
			ff_portalfx_03::s_shader = nullptr;
		}

		if (ff_portalfx_04::s_shader)
		{
			if (ff_portalfx_04::s_texture)
			{
				dev->SetTexture(0, ff_portalfx_04::s_texture);
				ff_portalfx_04::s_texture = nullptr;
			}

			dev->SetVertexShader(ff_portalfx_04::s_shader);
			dev->SetFVF(NULL);
			ff_portalfx_04::s_shader = nullptr;
		}

		if (ff_more_models::s_shader)
		{
			dev->SetVertexShader(ff_more_models::s_shader);
			dev->SetFVF(NULL);
			ff_more_models::s_shader = nullptr;
		}

		if (ff_vgui::s_shader01)
		{ dev->SetVertexShader(ff_vgui::s_shader01); dev->SetFVF(NULL); ff_vgui::s_shader01 = nullptr; }

		if (ff_vgui::s_shader02)
		{ dev->SetVertexShader(ff_vgui::s_shader02); dev->SetFVF(NULL); ff_vgui::s_shader02 = nullptr; }

		if (ff_vgui::s_shader03)
		{ dev->SetVertexShader(ff_vgui::s_shader03); dev->SetFVF(NULL); ff_vgui::s_shader03 = nullptr; }

		if (ff_vgui::s_shader04)
		{ dev->SetVertexShader(ff_vgui::s_shader04); dev->SetFVF(NULL); ff_vgui::s_shader04 = nullptr; }

		do_not_render_next_mesh = false;
		render_second_pass_with_basetexture2 = false;
		render_with_new_stride = false;

		// reset all mesh tweakables
		render_next_mesh::reset();
	}

	HOOK_RETN_PLACE_DEF(cmeshdx8_renderpass_post_draw_retn_addr);
	//void __declspec(naked) cmeshdx8_renderpass_post_draw_stub()
	//{
	//	__asm
	//	{
	//		mov		ecx, [esi + 0x50];
	//		push	ecx;
	//		push	eax;

	//		mov		eax, do_not_render_next_mesh;
	//		test	eax, eax;
	//		jnz		SKIP;

	//		call	edx; // DrawIndexedPrimitive
	//		jmp		OG;

	//	SKIP:
	//		add		esp, 0x1C;

	//	OG:
	//		pushad;
	//		push	edi;
	//		call	cmeshdx8_renderpass_post_draw;
	//		add		esp, 4;
	//		popad;

	//		jmp		cmeshdx8_renderpass_post_draw_retn_addr;
	//	}
	//}

	void __declspec(naked) cmeshdx8_renderpass_post_draw_stub()
	{
		__asm
		{
			// og code
			mov		ecx, [esi + 0x50];
			push	ecx;
			push	eax;

			//pushad;
			call	cmeshdx8_renderpass_post_draw; // instead of 'edx' (DrawIndexedPrimitive)
			add		esp, 0x1C;
			//popad;

			jmp		cmeshdx8_renderpass_post_draw_retn_addr;
		}
	}


	// ##########################
	// ##########################

	
	namespace ff_brushmodels
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		IDirect3DBaseTexture9* s_texture;
	}

	// cl_brushfastpath 0 will result in this being called
	void __fastcall tbl_hk::bmodel_renderer::DrawBrushModelEx::Detour(void* ecx, void* o1, IClientEntity* baseentity, model_t* model, const Vector* origin, const QAngle* angles, DrawBrushModelMode_t mode)
	{
		const auto dev = game::get_d3d_device();
		dev->GetVertexShader(&og_bmodel_shader);

		tbl_hk::bmodel_renderer::table.original<FN>(Index)(ecx, o1, baseentity, model, origin, angles, mode);

		dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));
		dev->SetFVF(NULL);

		if (og_bmodel_shader)
		{
			dev->SetVertexShader(og_bmodel_shader);
			og_bmodel_shader = nullptr;
		}
	}

	// flags 0x40000000 = shadow?
	// called if cl_brushfastpath 1
	void __fastcall tbl_hk::bmodel_renderer::DrawBrushModelArray::Detour(void* ecx, void* o1, void* o2, int nCount, const BrushArrayInstanceData_t& pInstanceData, int nModelTypeFlags)
	{
		const auto dev = game::get_d3d_device();
		dev->GetVertexShader(&og_bmodel_shader);
		//dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));

		VMatrix mat = {}; 
		mat.m[0][0] = pInstanceData.m_pBrushToWorld->m_flMatVal[0][0];
		mat.m[1][0] = pInstanceData.m_pBrushToWorld->m_flMatVal[0][1];
		mat.m[2][0] = pInstanceData.m_pBrushToWorld->m_flMatVal[0][2];

		mat.m[0][1] = pInstanceData.m_pBrushToWorld->m_flMatVal[1][0];
		mat.m[1][1] = pInstanceData.m_pBrushToWorld->m_flMatVal[1][1];
		mat.m[2][1] = pInstanceData.m_pBrushToWorld->m_flMatVal[1][2];

		mat.m[0][2] = pInstanceData.m_pBrushToWorld->m_flMatVal[2][0];
		mat.m[1][2] = pInstanceData.m_pBrushToWorld->m_flMatVal[2][1];
		mat.m[2][2] = pInstanceData.m_pBrushToWorld->m_flMatVal[2][2];

		mat.m[3][0] = pInstanceData.m_pBrushToWorld->m_flMatVal[0][3];
		mat.m[3][1] = pInstanceData.m_pBrushToWorld->m_flMatVal[1][3];
		mat.m[3][2] = pInstanceData.m_pBrushToWorld->m_flMatVal[2][3];
		mat.m[3][3] = game::identity[3][3];

		dev->SetTransform(D3DTS_WORLD, reinterpret_cast<D3DMATRIX*>(&mat.m)); 
 

		tbl_hk::bmodel_renderer::table.original<FN>(Index)(ecx, o1, o2, nCount, pInstanceData, nModelTypeFlags);


		dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));
		dev->SetFVF(NULL);

		if (og_bmodel_shader)
		{
			dev->SetVertexShader(og_bmodel_shader);
			og_bmodel_shader = nullptr;
		}
	}

	// not used for brushmodels when cl_brushfastpath 0?
	void cmeshdx8_renderpassforinstances_pre_draw(CMeshDX8* mesh, [[maybe_unused]] MeshInstanceData_t* data)
	{
		const auto dev = game::get_d3d_device();

		IDirect3DVertexBuffer9* b = nullptr;
		UINT ofs = 0, stride = 0;
		dev->GetStreamSource(0, &b, &ofs, &stride);

		//Vector* model_org = reinterpret_cast<Vector*>(ENGINE_BASE + 0x50DA90);
		//VMatrix* model_to_world_mtx = reinterpret_cast<VMatrix*>(ENGINE_BASE + 0x637158);

		// g_pInstanceData ... same as second func argument
		MeshInstanceData_t* instance_info = reinterpret_cast<MeshInstanceData_t*>(*(DWORD*)(RENDERER_BASE + 0x1754AC));

		/*if (&instance_info != &data)
		{
			int x = 1;
		}*/

		VMatrix mat = {};
		mat.m[0][0] = instance_info->m_pPoseToWorld->m_flMatVal[0][0];
		mat.m[1][0] = instance_info->m_pPoseToWorld->m_flMatVal[0][1];
		mat.m[2][0] = instance_info->m_pPoseToWorld->m_flMatVal[0][2];

		mat.m[0][1] = instance_info->m_pPoseToWorld->m_flMatVal[1][0];
		mat.m[1][1] = instance_info->m_pPoseToWorld->m_flMatVal[1][1];
		mat.m[2][1] = instance_info->m_pPoseToWorld->m_flMatVal[1][2];

		mat.m[0][2] = instance_info->m_pPoseToWorld->m_flMatVal[2][0];
		mat.m[1][2] = instance_info->m_pPoseToWorld->m_flMatVal[2][1];
		mat.m[2][2] = instance_info->m_pPoseToWorld->m_flMatVal[2][2];

		mat.m[3][0] = instance_info->m_pPoseToWorld->m_flMatVal[0][3];
		mat.m[3][1] = instance_info->m_pPoseToWorld->m_flMatVal[1][3];
		mat.m[3][2] = instance_info->m_pPoseToWorld->m_flMatVal[2][3];
		mat.m[3][3] = game::identity[3][3];

		if (og_bmodel_shader && mesh->m_VertexFormat == 0x2480033) // THIS
		{
			// tc @ 24
			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX7);
			dev->SetVertexShader(nullptr);
		}
		else if (og_bmodel_shader)
		{
			if (mesh->m_VertexFormat == 0xa2183)
			{
				int break_me = 0;
			}
		}
		else 
		{
			const auto shaderapi = game::get_shaderapi();
			const auto current_material = shaderapi->vtbl->GetBoundMaterial(shaderapi, nullptr);
			const auto current_material_name = std::string_view(current_material->vftable->GetName(current_material));

			// metal door = 0xa2183
			if (mesh->m_VertexFormat == 0xa2183) // entities - not brushmodel (eg portal gun stand)
			{
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<D3DMATRIX*>(&mat.m));

				if (current_material_name.contains("props_destruction/glass_"))
				{
					IDirect3DBaseTexture9* aa = nullptr;
					dev->GetTexture(1, &aa);
					dev->SetTexture(0, aa); 
				}
				
				dev->SetFVF(D3DFVF_XYZB3 | D3DFVF_NORMAL | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE1(3));
				dev->GetVertexShader(&ff_brushmodels::s_shader);
				dev->SetVertexShader(nullptr);
			}

			// somewhat broken - never called ....
			else if (mesh->m_VertexFormat == 0xa0103) // glass shards
			{
				// todo set unique texture
				dev->SetTexture(0, nullptr); //tex_portal_mask); 

				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<D3DMATRIX*>(&mat.m));
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2);
				dev->GetVertexShader(&ff_glass_shards::s_shader);
				dev->SetVertexShader(nullptr);
			}
			else
			{
				int break_me = 1; 
			}
		}

		IDirect3DBaseTexture9* ttex = nullptr;
		dev->GetTexture(0, &ttex);

		if (!ttex)
		{
			int break_me = 1;
		}
	}

	void cmeshdx8_renderpassforinstances_post_draw()
	{
		const auto dev = game::get_d3d_device();

		// restore shader if we set it top null right before drawing in the func above
		if (ff_brushmodels::s_shader)
		{
			dev->SetVertexShader(ff_brushmodels::s_shader);
			dev->SetFVF(NULL);
			ff_brushmodels::s_shader = nullptr;
		}

		/*if (ff_unk01::s_shader)
		{
			dev->SetVertexShader(ff_unk01::s_shader);
			dev->SetFVF(NULL);
			ff_unk01::s_shader = nullptr;
		}*/
	}

	HOOK_RETN_PLACE_DEF(cmeshdx8_renderpassforinstances_pre_draw_retn_addr);
	void __declspec(naked) cmeshdx8_renderpassforinstances_pre_draw_stub()
	{
		__asm
		{
			// og code
			call eax;

			pushad;
			push	ebx; // actual instance data
			push	ecx; // CMeshDX8
			call	cmeshdx8_renderpassforinstances_pre_draw;
			add		esp, 8;
			popad;

			// og code
			mov		ecx, [ebp - 4];
			mov		edx, [esi + 0x148];
			push	eax;
			push	0;
			push	0;
			push	ecx;
			push	edi;
			call	edx; // DrawIndexedPrimitive

			pushad;
			call	cmeshdx8_renderpassforinstances_post_draw;
			popad;

			jmp cmeshdx8_renderpassforinstances_pre_draw_retn_addr;
		}
	}



	// #############
#if 0
	void cmeshdx8_renderpasswithvertexindexbuffer_pre_draw()
	{
		int z = 1;
	}

	void cmeshdx8_renderpasswithvertexindexbuffer_post_draw()
	{
		int z = 1;
	}

	HOOK_RETN_PLACE_DEF(cmeshdx8_renderpasswithvertexindexbuffer_retn_addr);
	void __declspec(naked) cmeshdx8_renderpasswithvertexindexbuffer_stub()
	{
		__asm
		{
			pushad;
			call	cmeshdx8_renderpasswithvertexindexbuffer_pre_draw;
			popad;

			// og code
			push eax;
			push ecx;
			push 0;
			push esi;
			push edi;
			call edx; // DrawIndexedPrimitive

			pushad;
			call	cmeshdx8_renderpasswithvertexindexbuffer_post_draw;
			popad;

			jmp cmeshdx8_renderpasswithvertexindexbuffer_retn_addr;
		}
	}
#endif

	void drawrenderable_pre_draw(C_BaseEntity* ent)
	{
		current_render_ent = ent;
	}

	HOOK_RETN_PLACE_DEF(drawrenderable_pre_draw_retn_addr);
	void __declspec(naked) drawrenderable_pre_draw_stub()
	{
		__asm
		{
			pushad;
			push	eax;
			call	drawrenderable_pre_draw;
			add		esp, 4;
			popad;

			// og
			mov     edx, [edi + 0x70];
			push    eax;
			call	edx;
			jmp		drawrenderable_pre_draw_retn_addr;
		}
	}

	model_render::model_render()
	{
		tbl_hk::model_renderer::_interface = utils::module_interface.get<tbl_hk::model_renderer::IVModelRender*>("engine.dll", "VEngineModel016");

		XASSERT(tbl_hk::model_renderer::table.init(tbl_hk::model_renderer::_interface) == false);
		XASSERT(tbl_hk::model_renderer::table.hook(&tbl_hk::model_renderer::DrawModelExecute::Detour, tbl_hk::model_renderer::DrawModelExecute::Index) == false);

		utils::hook(RENDERER_BASE + 0xAD23, cmeshdx8_renderpass_pre_draw_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(cmeshdx8_renderpass_pre_draw_retn_addr, RENDERER_BASE + 0xAD28);

		//utils::hook(RENDERER_BASE + 0xADF5, cmeshdx8_renderpass_post_draw_stub, HOOK_JUMP).install()->quick();
		//HOOK_RETN_PLACE(cmeshdx8_renderpass_post_draw_retn_addr, RENDERER_BASE + 0xADFC);
		utils::hook(RENDERER_BASE + 0xADF5, cmeshdx8_renderpass_post_draw_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(cmeshdx8_renderpass_post_draw_retn_addr, RENDERER_BASE + 0xADFC);

		// brushmodels - cubes - etc - CMeshMgr::RenderPassForInstances
		// A56D

		tbl_hk::bmodel_renderer::_interface = utils::module_interface.get<tbl_hk::bmodel_renderer::IVRenderView*>("engine.dll", "VEngineRenderView013");

		XASSERT(tbl_hk::bmodel_renderer::table.init(tbl_hk::bmodel_renderer::_interface) == false);
		XASSERT(tbl_hk::bmodel_renderer::table.hook(&tbl_hk::bmodel_renderer::DrawBrushModelEx::Detour, tbl_hk::bmodel_renderer::DrawBrushModelEx::Index) == false);
		XASSERT(tbl_hk::bmodel_renderer::table.hook(&tbl_hk::bmodel_renderer::DrawBrushModelArray::Detour, tbl_hk::bmodel_renderer::DrawBrushModelArray::Index) == false);

		utils::hook(RENDERER_BASE + 0xA56D, cmeshdx8_renderpassforinstances_pre_draw_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(cmeshdx8_renderpassforinstances_pre_draw_retn_addr, RENDERER_BASE + 0xA581);

		// enable mat_wireframe on portals to make them stable?
		//utils::hook::nop(CLIENT_BASE + 0x2BD41C, 6);
#if 0
		// 0xA685
		utils::hook(RENDERER_BASE + 0xA685, cmeshdx8_renderpasswithvertexindexbuffer_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(cmeshdx8_renderpasswithvertexindexbuffer_retn_addr, RENDERER_BASE + 0xA68D);
#endif

		utils::hook::nop(CLIENT_BASE + 0x1E4A18, 6);
		utils::hook(CLIENT_BASE + 0x1E4A18, drawrenderable_pre_draw_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(drawrenderable_pre_draw_retn_addr, CLIENT_BASE + 0x1E4A1E);
	}
}

