#include "std_include.hpp"

// surface dual render (displacement with blending) gets invisible when skinned objects are moved?
// eg terrain gets invisible if cube is picked up?

namespace components
{
	bool is_portalgun_viewmodel = false;

	bool render_with_new_stride = false;
	std::uint32_t new_stride = 0u;

	namespace ff_model
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		IDirect3DBaseTexture9* s_texture;
	}

	namespace ff_worldmodel
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		D3DMATRIX s_world_mtx = {};
	}

	namespace ff_glass_shards
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		IDirect3DBaseTexture9* s_texture;
	}

	namespace ff_bmodel
	{
		IDirect3DVertexShader9* s_shader = nullptr;
	}

	namespace tex_addons
	{
		LPDIRECT3DTEXTURE9 portal_mask;
		LPDIRECT3DTEXTURE9 portal_blue;
		LPDIRECT3DTEXTURE9 portal_blue_closed;
		LPDIRECT3DTEXTURE9 portal_orange;
		LPDIRECT3DTEXTURE9 portal_orange_closed;
		LPDIRECT3DTEXTURE9 glass_shards;
		LPDIRECT3DTEXTURE9 glass_window_lamps;
		LPDIRECT3DTEXTURE9 glass_window_observ;
		LPDIRECT3DTEXTURE9 black_shader;
		LPDIRECT3DTEXTURE9 blue_laser_dualrender;
		LPDIRECT3DTEXTURE9 sky_gray_ft;
		LPDIRECT3DTEXTURE9 sky_gray_bk;
		LPDIRECT3DTEXTURE9 sky_gray_lf;
		LPDIRECT3DTEXTURE9 sky_gray_rt;
		LPDIRECT3DTEXTURE9 sky_gray_up;
		LPDIRECT3DTEXTURE9 sky_gray_dn;
	}

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
		dev->SetTransform(D3DTS_WORLD, &game::IDENTITY);

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
			mat.m[3][3] = game::IDENTITY.m[3][3];

			dev->SetTransform(D3DTS_WORLD, reinterpret_cast<D3DMATRIX*>(&mat.m)); 
		}
		else if (pInfo.pModel->radius == 37.3153992f) // models/weapons/v_portalgun.mdl
		{
			dev->SetTransform(D3DTS_WORLD, &game::IDENTITY);

			is_portalgun_viewmodel = true;
			if (auto shaderapi = game::get_shaderapi(); shaderapi)
			{
				BufferedState_t buffer_state = {};
				shaderapi->vtbl->GetBufferedState(shaderapi, nullptr, &buffer_state);

				dev->GetTransform(D3DTS_VIEW, &saved_view);
				dev->GetTransform(D3DTS_PROJECTION, &saved_proj);

				dev->SetTransform(D3DTS_VIEW, &buffer_state.m_Transform[1]);
				dev->SetTransform(D3DTS_PROJECTION, &buffer_state.m_Transform[2]);
			}
		}

		tbl_hk::model_renderer::table.original<FN>(Index)(ecx, edx, oo, state, pInfo, pCustomBoneToWorld);

		if (is_portalgun_viewmodel)
		{
			dev->SetTransform(D3DTS_VIEW, &saved_view);
			dev->SetTransform(D3DTS_PROJECTION, &saved_proj);
		}

		is_portalgun_viewmodel = false;
		dev->SetTransform(D3DTS_WORLD, &game::IDENTITY);
		dev->SetFVF(NULL);

		if (ff_model::s_shader)
		{
			dev->SetVertexShader(ff_model::s_shader);
			ff_model::s_shader = nullptr;
		}

		// this tex is only used for refract shaders .. do not reset
		if (ff_model::s_texture)
		{
			dev->SetTexture(0, ff_model::s_texture);
			ff_model::s_texture = nullptr;
			add_light_to_texture_color_restore();
		}
	}

	// #
	// #

	

	// #TODO call from somewhere appropriate
	void model_render::init_texture_addons()
	{
		const auto dev = game::get_d3d_device();

		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_mask.png", &tex_addons::portal_mask);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_blue.png", &tex_addons::portal_blue);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_blue_closed.png", &tex_addons::portal_blue_closed);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_orange.png", &tex_addons::portal_orange);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_orange_closed.png", &tex_addons::portal_orange_closed);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\glass_shards.png", &tex_addons::glass_shards);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\glass_window_refract.png", &tex_addons::glass_window_lamps);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\glass_window_observ.png", &tex_addons::glass_window_observ);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\black_shader.png", &tex_addons::black_shader);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\laser_blue_dualrender_alpha_col.png", &tex_addons::blue_laser_dualrender);
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
		mtx.m[3][3] = game::IDENTITY.m[3][3];

		auto& ctx = model_render::primctx;
		const auto shaderapi = game::get_shaderapi();

		if (ctx.get_info_for_pass(shaderapi))
		{
			// added format check
			if (mesh->m_VertexFormat == 0x2480033 || mesh->m_VertexFormat == 0x80033)
			{
				if (ctx.info.shader_name.contains("Water"))
				{
					IMaterialVar* var = nullptr;
					if (has_materialvar(ctx.info.material, "$basetexture", &var))
					{
						// if material has NO defined basetexture
						if (var && !var->vftable->IsDefined(var))
						{
							// check if it has a defined bottommaterial
							var = nullptr;
							const auto has_bottom_mat = has_materialvar(ctx.info.material, "$bottommaterial", &var);

							if (!has_bottom_mat)
							{
								// do not render water surfaces that have no bottom material (this is the surface below the water)
								// could just check $abovewater I guess? lmao

								// we only need one surface
								ctx.modifiers.do_not_render = true;
							}

							// put the normalmap into texture slot 0
							else
							{
								//  BindTexture( SHADER_SAMPLER2, TEXTURE_BINDFLAGS_NONE, NORMALMAP, BUMPFRAME );
								IDirect3DBaseTexture9* tex = shaderapi->vtbl->GetD3DTexture(shaderapi, nullptr, ctx.info.buffer_state.m_BoundTexture[2]);
								if (tex)
								{
									// save og texture
									ctx.modifiers.as_water = true;
									ctx.save_texture(dev, 0);
									dev->SetTexture(0, tex);
								}
							}
						}

						// material has defined a $basetexture
						else
						{
							//  sampler 10
							IDirect3DBaseTexture9* tex = shaderapi->vtbl->GetD3DTexture(shaderapi, nullptr, ctx.info.buffer_state.m_BoundTexture[10]);
							if (tex)
							{
								// save og texture
								ctx.modifiers.as_water = true;
								ctx.save_texture(dev, 0);
								dev->SetTexture(0, tex);
							}
						}
					}
				}
			}

			//if (ctx.info.material_name.contains("water"))
			//{
			//	//ctx.modifiers.do_not_render = true;
			//}

			//if (ctx.info.material_name.contains("toxicslime002a_beneath"))
			//{
			//	//ctx.modifiers.do_not_render = true;
			//}
		}

		//if (ctx.info.material_name.contains("lab/glassw"))
		//{
		//	int break_me = 1;
		//}

		if (ff_bmodel::s_shader && mesh->m_VertexFormat == 0x2480033)
		{
			//ctx.modifiers.do_not_render = true;
			dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX7);
			dev->SetVertexShader(nullptr);
		}

		// player model - gun - grabable stuff - portal button - portal door - stairs
		else if (ff_model::s_shader && mesh->m_VertexFormat == 0xa0003)
		{
			//ctx.modifiers.do_not_render = true;
		
			if (ctx.info.material_name.contains("props_destruction/glass_"))
			{
				//ctx.modifiers.do_not_render = true;
				if (tex_addons::glass_shards)
				{
					// this can cause some issues with other glass textures?!
					// a prob. because: models/props_destruction/glass_fracture_a_inner
					ctx.save_texture(dev, 0);
					dev->SetTexture(0, tex_addons::glass_shards);
				}
			}
			/*else if (cname.contains("chell"))
			{
				ctx.modifiers.do_not_render = true;
			}*/

			//if (!is_portalgun_viewmodel)
			{
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
			}

			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX6);
			dev->SetVertexShader(nullptr); // vertexformat 0x00000000000a0003 
		}

		// this also renders the glass? infront of white panel lamps
		// also renders some foliage (2nd level - emissive)
		else if (ff_model::s_shader) // 0xa0103
		{
			
			// ctx.modifiers.do_not_render = true;

			// replace all refract shaders with wireframe
			if (ctx.info.shader_name.contains("Refract_DX90"))
			{
				// I think we are simply missing basetex0 here
				ctx.info.material->vftable->SetShader(ctx.info.material, "Wireframe");
			}

			// change observer window texture
			else if (ctx.info.material_name.contains("lab/glassw"))
			{
				if (tex_addons::glass_window_observ)
				{
					//dev->GetTexture(0, &ff_model::s_texture);
					ctx.save_texture(dev, 0);
					dev->SetTexture(0, tex_addons::glass_window_observ);
					add_light_to_texture_color_edit(0.9f, 1.3f, 1.5f, 0.05f);
				}
			}
			else if (ctx.info.material_name.contains("glass/glassw"))
			{
				if (tex_addons::glass_window_lamps)
				{
					//dev->GetTexture(0, &ff_model::s_texture);
					ctx.save_texture(dev, 0);
					dev->SetTexture(0, tex_addons::glass_window_lamps);
				}
			}
			else if (ctx.info.shader_name.contains("Black"))
			{
				dev->SetTexture(0, tex_addons::black_shader);
			}

			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3);
			dev->SetVertexShader(nullptr); // vertexformat 0x00000000000a0003
		}
		else
		{
			bool was_portal_related = false;

			// open / close anim ideas:
			// CPortalRender -> m_portalIsOpening vector to check for portal openings
			// C_Prop_Portal -> m_fOpenAmount ?

			// r_portal_stencil_depth 0 heavy influence
			if (ctx.info.material_name.contains("portal_stencil"))
			{
				was_portal_related = true; // prevent all other else's
				ctx.modifiers.do_not_render = true;
			}
			else if (ctx.info.material_name.contains("portalstaticoverlay_1"))
			{
				if (tex_addons::portal_mask)
				{
					ctx.save_texture(dev, 1);
					dev->SetTexture(1, tex_addons::portal_mask);
				}

				if (tex_addons::portal_blue)
				{
					ctx.save_texture(dev, 0);
					dev->SetTexture(0, tex_addons::portal_blue);
				}

				// replace with wireframe (makes life much easier)
				if (ctx.info.shader_name != "Wireframe_DX9")
				{
					ctx.info.material->vftable->SetShader(ctx.info.material, "Wireframe");
				}

				// #
				// scale portal on opening

				// portal opening value is eased in -> apply inverse ease-in
				float s = std::sqrtf(model_render::portal1_open_amount); 

				// ease out - but not really
				s = 1 - (1 - s) * (1 - s);
				s = 1 - (1 - s) * (1 - s);
				s *= s * s;

				// map to a different range because a scalar > 1 => smaller portal
				// opening factor of 0 means that we start at with a 6x smaller portal
				s = -4.0f * s + 5.0f;

				// create a scaling matrix
				D3DXMATRIX scaleMatrix;
				D3DXMatrixScaling(&scaleMatrix, s, s, 1.0f);

				// translate uv's to the center, scale from the center and translate back 
				scaleMatrix = game::TC_TRANSLATE_TO_CENTER * scaleMatrix * game::TC_TRANSLATE_FROM_CENTER_TO_TOP_LEFT;

				//dev->SetTransform(D3DTS_TEXTURE0, &scaleMatrix);
				ctx.set_texture_transform(dev, &scaleMatrix);

				ctx.save_tss(dev, D3DTSS_TEXTURETRANSFORMFLAGS);
				dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);


				// #
				// inactive / active portal state

				ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
				ctx.save_tss(dev, D3DTSS_ALPHAARG2);

				if (!model_render::portal1_is_linked)
				{
					dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0, 255));
					dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
				}
				else
				{
					// transition n
					int t = static_cast<int>(std::roundf(((1.0f - std::sqrtf(model_render::portal2_open_amount)) - 0.1f) * (255.0f / 0.9f)));
						t = static_cast<int>(std::clamp(t, 0, 255)) ;

					dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0, t));
					dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
				}


				//ctx.modifiers.do_not_render = true;
				was_portal_related = true;
			}
			else if (ctx.info.material_name.contains("portalstaticoverlay_2"))
			{
				{
					if (tex_addons::portal_mask)
					{
						ctx.save_texture(dev, 1);
						dev->SetTexture(1, tex_addons::portal_mask);
					}

					if (tex_addons::portal_orange)
					{
						ctx.save_texture(dev, 0);
						dev->SetTexture(0, tex_addons::portal_orange);
					}
				}
				
				// replace with wireframe (makes life much easier)
				if (ctx.info.shader_name != "Wireframe_DX9")
				{
					ctx.info.material->vftable->SetShader(ctx.info.material, "Wireframe");
				}


				// #
				// scale portal on opening

				// portal opening value is eased in -> apply inverse ease-in
				float s = std::sqrtf(model_render::portal2_open_amount);

				// ease out - but not really
				s = 1 - (1 - s) * (1 - s);
				s = 1 - (1 - s) * (1 - s);
				s *= s * s;

				// map to a different range because a scalar > 1 => smaller portal
				// opening factor of 0 means that we start at with a 6x smaller portal
				s = -5.0f * s + 6.0f;

				// create a scaling matrix
				D3DXMATRIX scaleMatrix;
				D3DXMatrixScaling(&scaleMatrix, s, s, 1.0f);

				// translate uv's to the center, scale from the center and translate back 
				scaleMatrix = game::TC_TRANSLATE_TO_CENTER * scaleMatrix * game::TC_TRANSLATE_FROM_CENTER_TO_TOP_LEFT;

				//dev->SetTransform(D3DTS_TEXTURE0, &scaleMatrix);
				ctx.set_texture_transform(dev, &scaleMatrix);

				ctx.save_tss(dev, D3DTSS_TEXTURETRANSFORMFLAGS);
				dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);


				// #
				// inactive / active portal state

				ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
				ctx.save_tss(dev, D3DTSS_ALPHAARG2); 

				if (!model_render::portal2_is_linked)
				{
					dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0, 255));
					dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
				}
				else
				{
					// transition n
					int t = static_cast<int>(std::roundf(((1.0f - std::sqrtf(model_render::portal1_open_amount)) - 0.1f) * (255.0f / 0.9f)));
					t = static_cast<int>(std::clamp(t, 0, 255));

					dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0, t));
					dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
				}

				//ctx.modifiers.do_not_render = true;
				was_portal_related = true;
			}

			if (was_portal_related) 
			{
				// through wall overlays
				/*if (mesh->m_VertexFormat == 0xa0007)
				{
					int break_me = 1;
				}*/

				// draws portal stencil hole
				if (mesh->m_VertexFormat == 0x4a0003)
				{
					//ctx.modifiers.do_not_render = true;
					ctx.save_vs(dev);
					dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64
					dev->SetVertexShader(nullptr);
					dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
				}

				// if set to wireframe mode 
				else if (mesh->m_VertexFormat == 0x80003)
				{
					//ctx.modifiers.do_not_render = true;
					ctx.save_vs(dev);
					dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));
					dev->SetVertexShader(nullptr);
					dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
				}
			}

			// world geo - floor / walls --- "LightmappedGeneric"
			// this renders water but not the $bottommaterial
			else if (mesh->m_VertexFormat == 0x2480033)
			{
				//ctx.modifiers.do_not_render = true;
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3); // tc @ 24
				dev->SetTransform(D3DTS_WORLD, &game::IDENTITY);

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
				//ctx.modifiers.do_not_render = true;

				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2);
				dev->SetTransform(D3DTS_WORLD, &game::IDENTITY);

				D3DXMATRIX ret = {};
				dev->GetTransform(D3DTS_TEXTURE0, &ret);
				
				// tc scroll
				ret(3, 1) = (float)main_module::framecount * 0.0015f;

				ctx.set_texture_transform(dev, &ret);
				ctx.save_tss(dev, D3DTSS_TEXTURETRANSFORMFLAGS);
				dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			}

			// laser platforms + DebugTextureView
			// renders a small quad at 0 0 0 ?
			// also rendering transporting beams now?
			else if (mesh->m_VertexFormat == 0x80001)
			{
				//ctx.modifiers.do_not_render = true;

				if (ctx.info.shader_name.starts_with("SolidEn")) // SolidEnergy_dx9
				{
					ctx.save_vs(dev);
					dev->SetVertexShader(nullptr);

					dev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(1)); // tc @ 12 - missing 4 bytes at the end here - fixed with tc2 size 3?
					dev->SetTransform(D3DTS_WORLD, &game::IDENTITY);

					D3DXMATRIX ret = {};
					dev->GetTransform(D3DTS_TEXTURE0, &ret);

					// tc scroll
					ret(3, 1) = (float)main_module::framecount * 0.01f;

					ctx.set_texture_transform(dev, &ret);
					ctx.save_tss(dev, D3DTSS_TEXTURETRANSFORMFLAGS);
					dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

					// slightly increase the alpha so that the 'fog' becomes visible
					ctx.modifiers.as_transport_beam = true;
				}
				else
				{
					ctx.modifiers.do_not_render = true;
				}
			}

			// lasers - indicator dots - some of the white light stripes
			// renders the the wireframe portal if hook enabled to render wireframe for portals
			// renders door portals
			// treeleaf shader
			// + skybox
			else if (mesh->m_VertexFormat == 0x80003)
			{
				//ctx.modifiers.do_not_render = true;

				/*if (ctx.info.material_name.contains("light_panel_"))
				{
					add_nocull_materialvar(ctx.info.material); // no longer needed
					//ctx.modifiers.do_not_render = true;
				}
				// side beams of light bridges - effects/projected_wall_rail
				else*/ if (ctx.info.material_name.contains("ed_wall_ra"))
				{
					//ctx.modifiers.do_not_render = true;
					if (tex_addons::blue_laser_dualrender)
					{
						ctx.modifiers.dual_render_with_specified_texture = true;
						ctx.modifiers.dual_render_texture = tex_addons::blue_laser_dualrender;
					}
				}
				// TODO - create actual portals for this?
				// requires portal stencil depth of at least 1
				else if (ctx.info.material_name.contains("decals/portalstencildecal"))
				{
					//ctx.modifiers.do_not_render = true;

					// #TODO set unique texture
					dev->SetTexture(0, tex_addons::portal_mask);
				}
				// unique textures for the white sky so they can be marked
				else if (ctx.info.material_name.contains("sky"))
				{
					if (ctx.info.material_name.contains("_white"))
					{
						if (ctx.info.material_name.contains("eft")) // sky_whiteft
						{
							dev->SetTexture(0, tex_addons::sky_gray_ft);
						}
						else if (ctx.info.material_name.contains("ebk"))
						{
							dev->SetTexture(0, tex_addons::sky_gray_bk);
						}
						else if (ctx.info.material_name.contains("elf"))
						{
							dev->SetTexture(0, tex_addons::sky_gray_lf);
						}
						else if (ctx.info.material_name.contains("ert"))
						{
							dev->SetTexture(0, tex_addons::sky_gray_rt);
						}
						else if (ctx.info.material_name.contains("eup"))
						{
							dev->SetTexture(0, tex_addons::sky_gray_up);
						}
						else if (ctx.info.material_name.contains("edn"))
						{
							dev->SetTexture(0, tex_addons::sky_gray_dn);
						}

						ctx.modifiers.as_sky = true;
					}
				}

				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);

				// noticed some normal issues on vgui_indicator's .. disable normals for now?
				dev->SetFVF(D3DFVF_XYZB3 /*| D3DFVF_NORMAL*/ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));
				dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
			}

			// portal_draw_ghosting 0 disables this
			// this normally shows portals through walls
			else if (mesh->m_VertexFormat == 0xa0007) // portal fx 
			{
				//ctx.modifiers.do_not_render = true;
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX5 | D3DFVF_TEXCOORDSIZE1(4)); // tc at 16 + 12 :: 68 - 4 as last tc is one float
				dev->SetTransform(D3DTS_WORLD, &game::IDENTITY);
			}

			// related to props like portalgun, pickup-ables
			// does not have a visibile texture set? (setting one manually makes it show up)
			// renders decals/simpleshadow
			else if (mesh->m_VertexFormat == 0x6c0005)
			{
				ctx.modifiers.do_not_render = true;
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZB1 | D3DFVF_TEX5); // stride 48
			}

			// this would draw the portal1 and portal2 mesh (but we already do that way above)
			// can still be used on some levels (eg sp_a2_bridge_intro)
			// portal refract texture
			else if (mesh->m_VertexFormat == 0x4a0003)
			{
				ctx.modifiers.do_not_render = true;
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64
			}

			// engine/shadowbuild
			else if (mesh->m_VertexFormat == 0xa0003)
			{
				ctx.modifiers.do_not_render = true;
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr); // def. render using FF as the shader is causing heavy frametime drops
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX5 | D3DFVF_TEXCOORDSIZE1(4)); // stride 0x30
			}

			// HUD
			else if (mesh->m_VertexFormat == 0x80007) 
			{
				//ctx.modifiers.do_not_render = true;
				
				// always render UI and world ui with high gamma
				ctx.modifiers.with_high_gamma = true;

				if (ctx.info.material_name.contains("vgui__fontpage"))
				{
					// get rid of all world-rendered text as its using the same glyph as HUD elements?!
					if (ctx.info.buffer_state.m_Transform[0].m[3][0] != 0.0f)
					{
						ctx.modifiers.do_not_render = true;
					}
				}
				else if (ctx.info.material_name.contains("vgui_coop_progress_board")
					  || ctx.info.material_name.contains("p2_lightboard_vgui")
					  || ctx.info.material_name.contains("elevator_video_overlay"))
				{
					//ctx.modifiers.do_not_render = true;
					ctx.save_vs(dev);
					dev->SetVertexShader(nullptr);
					dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
					//dev->SetFVF(D3DFVF_XYZB3 | D3DFVF_TEX4); // no need to set fvf here!
				}

				// video on intro3
				else if (ctx.info.material_name.contains("elevator_video_lines"))
				{
					ctx.save_vs(dev);
					dev->SetVertexShader(nullptr);
					dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
				}
			}

			// on portal open - spark fx (center)
			// + portal clearing gate (blue sweeping beam)
			// + portal gun pickup effect
			// can be rendered but also requires vertexshader + position
			else if (mesh->m_VertexFormat == 0x924900005) // stride 0x70 - 112
			{
				//ctx.modifiers.do_not_render = true;

				dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
				dev->SetTransform(D3DTS_VIEW, &ctx.info.buffer_state.m_Transform[1]);
				dev->SetTransform(D3DTS_PROJECTION, &ctx.info.buffer_state.m_Transform[2]);

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

			// general models (eg. furniture in first lvl - container)
			else if (mesh->m_VertexFormat == 0xa0103)  
			{
				//ctx.modifiers.do_not_render = true;
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64 :: tc @ 24
			}

			// terrain - "WorldVertexTransition"
			else if (mesh->m_VertexFormat == 0x480007)
			{
				//ctx.modifiers.do_not_render = true;

				if (ctx.info.shader_name.contains("WorldVertexTransition_DX9"))
				{
					ctx.modifiers.dual_render_with_basetexture2 = true;
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
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE1(2)); // tc @ 28

				// not doing this and picking up a skinned model (eg. cube) will break displacement rendering???
				dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
			}

			// hanging cables - requires vertex shader - verts not modified on the cpu
			else if (mesh->m_VertexFormat == 0x24900005)
			{
				ctx.modifiers.do_not_render = true; // they can freak out sometimes so just ignore them for now
				
#if 0
				// do not set fvf
				//dev->SetFVF(D3DFVF_XYZB5 | D3DFVF_NORMAL | D3DFVF_TEX2); // tc @ 28
				ctx.save_vs(dev);
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
#ifdef DEBUG
				//ctx.modifiers.do_not_render = true;
				int break_me = 0;
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
				//dev->SetTexture(0, tex_addons::portal_mask);

				//ctx.save_vs(dev);
				//dev->SetVertexShader(nullptr);
				//dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3); // vertex fmt looks like: pos - normal - 3x texcoord (float2) = 48 byte

				//dev->SetTransform(D3DTS_WORLD, &buffer_state.m_Transform[0]);  
				//dev->SetTransform(D3DTS_VIEW, &buffer_state.m_Transform[1]);
				//dev->SetTransform(D3DTS_PROJECTION, &buffer_state.m_Transform[2]);
#endif
			}

			// on portal open - blob in the middle (impact)
			else if (mesh->m_VertexFormat == 0x80037) // TODO - test with buffer_state transforms 
			{
#ifdef DEBUG
				//ctx.modifiers.do_not_render = true; // this needs a position as it spawns on 0 0 0 // stride 0x40
				int break_me = 0;

				//ctx.save_vs(dev);
				//dev->SetVertexShader(nullptr);
				//dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX5);
#endif
			}

			// on portal open - outer ring effect
			else if (mesh->m_VertexFormat == 0x1b924900005) // stride 0x90 - 144
			{
#ifdef DEBUG
				//ctx.modifiers.do_not_render = true;
				int break_me = 0;
#endif
			}

			// portal clearing gate
			// renders water $bottommaterial
			else if (mesh->m_VertexFormat == 0x80033) //stride = 0x40 
			{
				//ctx.modifiers.do_not_render = true;
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64 :: tc @ 24
				dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
			}

			// decals
			else if (mesh->m_VertexFormat == 0x2480037)  // stride 0x50 - 80 
			{
				//ctx.modifiers.do_not_render = true;
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZB4 | D3DFVF_TEX7 | D3DFVF_TEXCOORDSIZE1(4)); // 84 - 4 as last tc is one float :: tc at 28
				dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
			}

			// Sprite shader
			else if (mesh->m_VertexFormat == 0x914900005)
			{
				//ctx.modifiers.do_not_render = true; 

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
				//dev->SetTexture(0, tex_addons::portal_mask);

				//ctx.save_vs(dev);
				//dev->SetVertexShader(nullptr);
				//dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3); // vertex fmt looks like pos normal 3xtc (float2)

				//dev->SetTransform(D3DTS_WORLD, &buffer_state.m_Transform[0]);
				//dev->SetTransform(D3DTS_VIEW, &buffer_state.m_Transform[1]);
				//dev->SetTransform(D3DTS_PROJECTION, &buffer_state.m_Transform[2]);
			}

			// SpriteCard vista smoke 
			else if (mesh->m_VertexFormat == 0x24914900005) // stride 144 ....
			{
				//ctx.modifiers.do_not_render = true;
			}
#ifdef DEBUG
			else
			{
				//ctx.modifiers.do_not_render = true;
				int break_me = 1;  
			}

			int break_me = 1;
#endif
		}
	}

	HOOK_RETN_PLACE_DEF(cmeshdx8_renderpass_pre_draw_retn_addr);
	void __declspec(naked) cmeshdx8_renderpass_pre_draw_stub()
	{
		__asm
		{
			pushad;
			push	esi; // CMeshDX8
			call	cmeshdx8_renderpass_pre_draw;
			add		esp, 4;
			popad;

			// og code
			mov		ecx, [esi + 0x4C];
			test	ecx, ecx;
			jmp		cmeshdx8_renderpass_pre_draw_retn_addr;
		}
	}


	// #

	//void cmeshdx8_renderpass_post_draw(std::uint32_t num_prims_rendered)
	void cmeshdx8_renderpass_post_draw([[maybe_unused]] void* device_ptr, D3DPRIMITIVETYPE type, std::int32_t base_vert_index, std::uint32_t min_vert_index, std::uint32_t num_verts, std::uint32_t start_index, std::uint32_t prim_count)
	{
		const auto dev = game::get_d3d_device();
		const auto shaderapi = game::get_shaderapi();
		const auto& ctx = model_render::primctx;

		// 0 = Gamma 1.0 (fixes dark albedo) :: 1 = Gamma 2.2
		dev->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, ctx.modifiers.with_high_gamma ? 1u : 0u);
		

		// do not render next surface if set
		if (!ctx.modifiers.do_not_render)
		{
			if (render_with_new_stride)
			{
				num_verts *= 1;
				prim_count *= 1;
				type = D3DPT_TRIANGLELIST;
			}

			DWORD og_texfactor = {}, og_colorarg2 = {}, og_colorop = {};
			if (ctx.modifiers.as_sky)
			{
				// dev->SetRenderState(D3DRS_FOGENABLE, FALSE);

				// HACK - as long as sky marking is broken, use WORLD SPACE UI (emissive)
				// -> means that we can not use a distant light
				// -> this reduces the emissive intensity
				dev->GetRenderState(D3DRS_TEXTUREFACTOR, &og_texfactor);
				dev->GetTextureStageState(0, D3DTSS_COLORARG2, &og_colorarg2);
				dev->GetTextureStageState(0, D3DTSS_COLOROP, &og_colorop);
				
				dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(25, 25, 25, 255));
				dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
				dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			}
			else if (ctx.modifiers.as_transport_beam)
			{
				dev->GetRenderState(D3DRS_TEXTUREFACTOR, &og_texfactor);
				dev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &og_colorarg2);
				dev->GetTextureStageState(0, D3DTSS_ALPHAOP, &og_colorop);

				// slightly increase the alpha so that the 'fog' becomes visible
				dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0, 40));
				dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
				dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_ADD);
			}

			dev->DrawIndexedPrimitive(type, base_vert_index, min_vert_index, num_verts, start_index, prim_count);

			// restore emissive sky settings
			if (ctx.modifiers.as_sky)
			{
				dev->SetRenderState(D3DRS_TEXTUREFACTOR, og_texfactor);
				dev->SetTextureStageState(0, D3DTSS_COLORARG2, og_colorarg2);
				dev->SetTextureStageState(0, D3DTSS_COLOROP, og_colorop);
			}
			else if (ctx.modifiers.as_transport_beam)
			{
				dev->SetRenderState(D3DRS_TEXTUREFACTOR, og_texfactor);
				dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, og_colorarg2);
				dev->SetTextureStageState(0, D3DTSS_ALPHAOP, og_colorop);
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
		if (ctx.modifiers.dual_render_with_basetexture2)
		{
			// check if basemap2 is assigned
			if (ctx.info.buffer_state.m_BoundTexture[7])
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
				if (const auto basemap2 = shaderapi->vtbl->GetD3DTexture(shaderapi, nullptr, ctx.info.buffer_state.m_BoundTexture[7]);
					basemap2)
				{
					dev->SetTexture(0, basemap2);
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
				dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);

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

		if (ctx.modifiers.dual_render_with_specified_texture)
		{
			// save og texture
			IDirect3DBaseTexture9* og_tex0 = nullptr;
			dev->GetTexture(0, &og_tex0);

			// set new texture
			dev->SetTexture(0, ctx.modifiers.dual_render_texture);

			// re-draw surface
			dev->DrawIndexedPrimitive(type, base_vert_index, min_vert_index, num_verts, start_index, prim_count);

			// restore texture
			dev->SetTexture(0, og_tex0);
		}

		if (ff_worldmodel::s_shader)
		{
			dev->SetVertexShader(ff_worldmodel::s_shader);
			dev->SetFVF(NULL);
			ff_worldmodel::s_shader = nullptr;
		}

		render_with_new_stride = false;

		// reset prim/pass modifications
		model_render::primctx.restore_all(dev); 
		model_render::primctx.reset_context();
		dev->SetFVF(NULL);
	}

	HOOK_RETN_PLACE_DEF(cmeshdx8_renderpass_post_draw_retn_addr);
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

	// used if cl_brushfastpath = 0
	void __fastcall tbl_hk::bmodel_renderer::DrawBrushModelEx::Detour(void* ecx, void* o1, IClientEntity* baseentity, model_t* model, const Vector* origin, const QAngle* angles, DrawBrushModelMode_t mode)
	{
		const auto dev = game::get_d3d_device();
		dev->GetVertexShader(&ff_bmodel::s_shader);

		tbl_hk::bmodel_renderer::table.original<FN>(Index)(ecx, o1, baseentity, model, origin, angles, mode);

		dev->SetTransform(D3DTS_WORLD, &game::IDENTITY);
		dev->SetFVF(NULL);

		if (ff_bmodel::s_shader)
		{
			dev->SetVertexShader(ff_bmodel::s_shader);
			ff_bmodel::s_shader = nullptr;
		}
	}

	// used if cl_brushfastpath = 1
	void __fastcall tbl_hk::bmodel_renderer::DrawBrushModelArray::Detour(void* ecx, void* o1, void* o2, int nCount, const BrushArrayInstanceData_t& pInstanceData, int nModelTypeFlags)
	{
		const auto dev = game::get_d3d_device();
		dev->GetVertexShader(&ff_bmodel::s_shader);
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
		mat.m[3][3] = game::IDENTITY.m[3][3];

		dev->SetTransform(D3DTS_WORLD, reinterpret_cast<D3DMATRIX*>(&mat.m)); 

		tbl_hk::bmodel_renderer::table.original<FN>(Index)(ecx, o1, o2, nCount, pInstanceData, nModelTypeFlags);

		dev->SetTransform(D3DTS_WORLD, &game::IDENTITY);
		dev->SetFVF(NULL);

		if (ff_bmodel::s_shader)
		{
			dev->SetVertexShader(ff_bmodel::s_shader);
			ff_bmodel::s_shader = nullptr;
		}
	}

	// not used for brushmodels when cl_brushfastpath = 0
	void cmeshdx8_renderpassforinstances_pre_draw(const CMeshDX8* mesh, [[maybe_unused]] const MeshInstanceData_t* data)
	{
		const auto dev = game::get_d3d_device();

		IDirect3DVertexBuffer9* b = nullptr;
		UINT ofs = 0, stride = 0;
		dev->GetStreamSource(0, &b, &ofs, &stride);

		// g_pInstanceData ... same as second func argument
		//MeshInstanceData_t* instance_info = reinterpret_cast<MeshInstanceData_t*>(*(DWORD*)(RENDERER_BASE + 0x1754AC));

		VMatrix mat = {};
		mat.m[0][0] = data->m_pPoseToWorld->m_flMatVal[0][0];
		mat.m[1][0] = data->m_pPoseToWorld->m_flMatVal[0][1];
		mat.m[2][0] = data->m_pPoseToWorld->m_flMatVal[0][2];

		mat.m[0][1] = data->m_pPoseToWorld->m_flMatVal[1][0];
		mat.m[1][1] = data->m_pPoseToWorld->m_flMatVal[1][1];
		mat.m[2][1] = data->m_pPoseToWorld->m_flMatVal[1][2];

		mat.m[0][2] = data->m_pPoseToWorld->m_flMatVal[2][0];
		mat.m[1][2] = data->m_pPoseToWorld->m_flMatVal[2][1];
		mat.m[2][2] = data->m_pPoseToWorld->m_flMatVal[2][2];

		mat.m[3][0] = data->m_pPoseToWorld->m_flMatVal[0][3];
		mat.m[3][1] = data->m_pPoseToWorld->m_flMatVal[1][3];
		mat.m[3][2] = data->m_pPoseToWorld->m_flMatVal[2][3];
		mat.m[3][3] = game::IDENTITY.m[3][3];

		if (ff_bmodel::s_shader && mesh->m_VertexFormat == 0x2480033) // THIS
		{
			// tc @ 24
			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX7);
			dev->SetVertexShader(nullptr);
		}

#ifdef DEBUG
		else if (ff_bmodel::s_shader)
		{
			if (mesh->m_VertexFormat == 0xa2183)
			{
				int break_me = 0;
			}
		}
#endif

		else 
		{
			const auto shaderapi = game::get_shaderapi();
			const auto current_material = shaderapi->vtbl->GetBoundMaterial(shaderapi, nullptr);
			const auto current_material_name = std::string_view(current_material->vftable->GetName(current_material));

			// metal door = 0xa2183
			if (mesh->m_VertexFormat == 0xa2183) // entities - not brushmodel (eg portal gun stand)
			{
				if (current_material_name.contains("props_destruction/glass_"))
				{
					IDirect3DBaseTexture9* aa = nullptr;
					dev->GetTexture(1, &aa);
					dev->SetTexture(0, aa); 
				}
				
				dev->GetVertexShader(&ff_brushmodels::s_shader);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZB3 | D3DFVF_NORMAL | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE1(3));
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<D3DMATRIX*>(&mat.m));
			}

			// somewhat broken - never called ....
			else if (mesh->m_VertexFormat == 0xa0103) // glass shards
			{
				// todo set unique texture
				dev->SetTexture(0, nullptr); //tex_portal_mask); 

				dev->GetVertexShader(&ff_glass_shards::s_shader);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2);
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<D3DMATRIX*>(&mat.m));
			}

#ifdef DEBUG
			else
			{
				int break_me = 1; 
			}
#endif
		}

#ifdef DEBUG
		/*IDirect3DBaseTexture9* ttex = nullptr;
		dev->GetTexture(0, &ttex);

		if (!ttex)
		{
			int break_me = 1;
		}*/
#endif
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
	}

	HOOK_RETN_PLACE_DEF(cmeshdx8_renderpassforinstances_pre_draw_retn_addr);
	void __declspec(naked) cmeshdx8_renderpassforinstances_pre_draw_stub()
	{
		__asm
		{
			// og code
			call	eax;

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
		int break_me = 1;
	}

	void cmeshdx8_renderpasswithvertexindexbuffer_post_draw()
	{
		int break_me = 1;
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
			push	eax;
			push	ecx;
			push	0;
			push	esi;
			push	edi;
			call	edx; // DrawIndexedPrimitive

			pushad;
			call	cmeshdx8_renderpasswithvertexindexbuffer_post_draw;
			popad;

			jmp		cmeshdx8_renderpasswithvertexindexbuffer_retn_addr;
		}
	}
#endif

	// -----

	// TODO - this fails when there are multiple portals on a level (static portal = separate prop, so there can be more then 2)
	// need to find a way to match up the portals
	void prop_portal_client_think_hk(const C_Prop_Portal* portal)
	{
		if (portal && portal->m_bActivated)
		{
			if (!portal->m_bIsPortal2)
			{
#ifdef DEBUG
				if (portal->m_pLinkedPortal)
				{
					int break_me = 1;
				}

				if (portal->m_bIsMobile)
				{
					int break_me = 1;
				}
#endif
				
				model_render::portal1_is_linked = portal->m_pLinkedPortal ? true : false;
				model_render::portal1_open_amount = portal->m_fOpenAmount;
			}
			else
			{
#ifdef DEBUG
				if (portal->m_pLinkedPortal)
				{
					int break_me = 1;
				}

				if (portal->m_bIsMobile)
				{
					int break_me = 1;
				}
#endif
				model_render::portal2_is_linked = portal->m_pLinkedPortal ? true : false;
				model_render::portal2_open_amount = portal->m_fOpenAmount;
			}
		}
	}

	void __declspec(naked) prop_portal_client_think_stub()
	{
		__asm
		{
			pushad;
			push	esi; // C_Prop_Portal ptr
			call	prop_portal_client_think_hk;
			add		esp, 4;
			popad;

			// og
			pop     esi;
			retn;
		}
	}

	model_render::model_render()
	{
		tbl_hk::model_renderer::_interface = utils::module_interface.get<tbl_hk::model_renderer::IVModelRender*>("engine.dll", "VEngineModel016");

		XASSERT(tbl_hk::model_renderer::table.init(tbl_hk::model_renderer::_interface) == false);
		XASSERT(tbl_hk::model_renderer::table.hook(&tbl_hk::model_renderer::DrawModelExecute::Detour, tbl_hk::model_renderer::DrawModelExecute::Index) == false);

		utils::hook(RENDERER_BASE + 0xAD23, cmeshdx8_renderpass_pre_draw_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(cmeshdx8_renderpass_pre_draw_retn_addr, RENDERER_BASE + 0xAD28);

		utils::hook(RENDERER_BASE + 0xADF5, cmeshdx8_renderpass_post_draw_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(cmeshdx8_renderpass_post_draw_retn_addr, RENDERER_BASE + 0xADFC);


		// brushmodels - cubes - etc - CMeshMgr::RenderPassForInstances

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

		// C_Prop_Portal::ClientThink :: hook to get portal 1/2 m_fOpenAmount member var
		utils::hook(CLIENT_BASE + 0x280012, prop_portal_client_think_stub, HOOK_JUMP).install()->quick();
	}
}

