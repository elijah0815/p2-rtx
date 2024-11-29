#include "std_include.hpp"

// surface dual render (displacement with blending) gets invisible when skinned objects are moved?
// eg terrain gets invisible if cube is picked up?

namespace components
{
	int	 is_rendering_paint = false;
	int	 is_rendering_bmodel_paint = false;

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
		LPDIRECT3DTEXTURE9 portal_blue_overlay;
		LPDIRECT3DTEXTURE9 portal_blue_closed;
		LPDIRECT3DTEXTURE9 portal_orange;
		LPDIRECT3DTEXTURE9 portal_orange_overlay;
		LPDIRECT3DTEXTURE9 portal_orange_closed;
		LPDIRECT3DTEXTURE9 portal_red;
		LPDIRECT3DTEXTURE9 portal_purple;
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
		LPDIRECT3DTEXTURE9 emancipation_grill;
		LPDIRECT3DTEXTURE9 emancipation_grill_bg;
		LPDIRECT3DTEXTURE9 water_drip;
		LPDIRECT3DTEXTURE9 white;
	}

	// #TODO call from somewhere appropriate
	void model_render::init_texture_addons(bool release)
	{
		if (release)
		{
			if (tex_addons::portal_mask) tex_addons::portal_mask->Release();
			if (tex_addons::portal_blue) tex_addons::portal_blue->Release();
			if (tex_addons::portal_blue_overlay) tex_addons::portal_blue_overlay->Release();
			if (tex_addons::portal_blue_closed) tex_addons::portal_blue_closed->Release();
			if (tex_addons::portal_orange) tex_addons::portal_orange->Release();
			if (tex_addons::portal_orange_overlay) tex_addons::portal_orange_overlay->Release();
			if (tex_addons::portal_orange_closed) tex_addons::portal_orange_closed->Release();
			if (tex_addons::portal_red) tex_addons::portal_red->Release();
			if (tex_addons::portal_purple) tex_addons::portal_purple->Release();
			if (tex_addons::glass_shards) tex_addons::glass_shards->Release();
			if (tex_addons::glass_window_lamps) tex_addons::glass_window_lamps->Release();
			if (tex_addons::glass_window_observ) tex_addons::glass_window_observ->Release();
			if (tex_addons::black_shader) tex_addons::black_shader->Release();
			if (tex_addons::blue_laser_dualrender) tex_addons::blue_laser_dualrender->Release();
			if (tex_addons::sky_gray_ft) tex_addons::sky_gray_ft->Release();
			if (tex_addons::sky_gray_bk) tex_addons::sky_gray_bk->Release();
			if (tex_addons::sky_gray_lf) tex_addons::sky_gray_lf->Release();
			if (tex_addons::sky_gray_rt) tex_addons::sky_gray_rt->Release();
			if (tex_addons::sky_gray_up) tex_addons::sky_gray_up->Release();
			if (tex_addons::sky_gray_dn) tex_addons::sky_gray_dn->Release();
			if (tex_addons::emancipation_grill) tex_addons::emancipation_grill->Release();
			if (tex_addons::emancipation_grill_bg) tex_addons::emancipation_grill_bg->Release();
			if (tex_addons::water_drip) tex_addons::water_drip->Release();
			if (tex_addons::white) tex_addons::white->Release();
			return;
		}

		const auto dev = game::get_d3d_device();

		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_mask.png", &tex_addons::portal_mask);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_blue.png", &tex_addons::portal_blue);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_blue_overlay.png", &tex_addons::portal_blue_overlay);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_blue_closed.png", &tex_addons::portal_blue_closed);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_orange.png", &tex_addons::portal_orange);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_orange_overlay.png", &tex_addons::portal_orange_overlay);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_orange_closed.png", &tex_addons::portal_orange_closed);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_red.png", &tex_addons::portal_red);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\portal_purple.png", &tex_addons::portal_purple);
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
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\emancipation_grill.png", &tex_addons::emancipation_grill);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\emancipation_grill_bg.png", &tex_addons::emancipation_grill_bg);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\water_drip.png", &tex_addons::water_drip);
		D3DXCreateTextureFromFileA(dev, "portal2-rtx\\textures\\white.dds", &tex_addons::white);
	}

	// check for specific material var and return it in 'out_var'
	bool has_materialvar(IMaterialInternal* cmat, const char* var_name, IMaterialVar** out_var = nullptr)
	{
		bool found = false;
		const auto var = cmat->vftable->FindVar(cmat, nullptr, var_name, &found, false);

		if (out_var) {
			*out_var = var;
		}

		return found;
	}

	// adds '$nocull' material var to material - returns true if exists
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
	bool g_light_to_texture_modified = false;

	// To be used before rendering a surface with a texture that is marked with the 'add light to tex' category in remix
	// > will change the color and intensity of the light created by remix
	// > supports 1 saved state for now (should be enough)
	// > values also influence radiance (can be larger than 1)
	void add_light_to_texture_color_edit(const float& r, const float& g, const float& b, const float scalar = 1.0f)
	{
		const auto dev = game::get_d3d_device();

		D3DMATERIAL9 temp_mat = {};
		dev->GetMaterial(&temp_mat);

		// save prev. color
		g_old_light_to_texture_color = temp_mat.Diffuse;

		temp_mat.Diffuse = { .r = r * scalar, .g = g * scalar, .b = b * scalar };
		dev->SetMaterial(&temp_mat);

		g_light_to_texture_modified = true;
	}

	// restore color
	void add_light_to_texture_color_restore()
	{
		if (g_light_to_texture_modified)
		{
			const D3DMATERIAL9 temp_mat =
			{
				.Diffuse = {.r = g_old_light_to_texture_color.r, .g = g_old_light_to_texture_color.g, .b = g_old_light_to_texture_color.b }
			};

			game::get_d3d_device()->SetMaterial(&temp_mat);
			g_light_to_texture_modified = false;
		}
	}

	// uses unused Renderstate 149 to tweak the emissive intensity of remix legacy materials
	void set_remix_emissive_intensity(IDirect3DDevice9* dev, prim_fvf_context& ctx, float intensity)
	{
		ctx.save_rs(dev, (D3DRENDERSTATETYPE)149);
		dev->SetRenderState((D3DRENDERSTATETYPE)149, *reinterpret_cast<DWORD*>(&intensity));
	}

	// can be used to figure out the layout of the vertex buffer
	void lookat_vertex_decl([[maybe_unused]] IDirect3DDevice9* dev, [[maybe_unused]] CPrimList* primlist = nullptr)
	{
#ifdef DEBUG
		IDirect3DVertexDeclaration9* vertex_decl = nullptr;
		dev->GetVertexDeclaration(&vertex_decl);

		enum d3ddecltype : BYTE
		{
			D3DDECLTYPE_FLOAT1 = 0,		// 1D float expanded to (value, 0., 0., 1.)
			D3DDECLTYPE_FLOAT2 = 1,		// 2D float expanded to (value, value, 0., 1.)
			D3DDECLTYPE_FLOAT3 = 2,		// 3D float expanded to (value, value, value, 1.)
			D3DDECLTYPE_FLOAT4 = 3,		// 4D float
			D3DDECLTYPE_D3DCOLOR = 4,	// 4D packed unsigned bytes mapped to 0. to 1. range

			// Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
			D3DDECLTYPE_UBYTE4 = 5,		// 4D unsigned byte
			D3DDECLTYPE_SHORT2 = 6,		// 2D signed short expanded to (value, value, 0., 1.)
			D3DDECLTYPE_SHORT4 = 7,		// 4D signed short

			// The following types are valid only with vertex shaders >= 2.0
			D3DDECLTYPE_UBYTE4N = 8,	// Each of 4 bytes is normalized by dividing to 255.0
			D3DDECLTYPE_SHORT2N = 9,	// 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
			D3DDECLTYPE_SHORT4N = 10,	// 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
			D3DDECLTYPE_USHORT2N = 11,  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
			D3DDECLTYPE_USHORT4N = 12,  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
			D3DDECLTYPE_UDEC3 = 13,		// 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
			D3DDECLTYPE_DEC3N = 14,		// 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
			D3DDECLTYPE_FLOAT16_2 = 15,	// Two 16-bit floating point values, expanded to (value, value, 0, 1)
			D3DDECLTYPE_FLOAT16_4 = 16,	// Four 16-bit floating point values
			D3DDECLTYPE_UNUSED = 17,	// When the type field in a decl is unused.
		};

		enum d3ddecluse : BYTE
		{
			D3DDECLUSAGE_POSITION = 0,
			D3DDECLUSAGE_BLENDWEIGHT,   // 1
			D3DDECLUSAGE_BLENDINDICES,  // 2
			D3DDECLUSAGE_NORMAL,        // 3
			D3DDECLUSAGE_PSIZE,         // 4
			D3DDECLUSAGE_TEXCOORD,      // 5
			D3DDECLUSAGE_TANGENT,       // 6
			D3DDECLUSAGE_BINORMAL,      // 7
			D3DDECLUSAGE_TESSFACTOR,    // 8
			D3DDECLUSAGE_POSITIONT,     // 9
			D3DDECLUSAGE_COLOR,         // 10
			D3DDECLUSAGE_FOG,           // 11
			D3DDECLUSAGE_DEPTH,         // 12
			D3DDECLUSAGE_SAMPLE,        // 13
		};

		struct d3dvertelem
		{
			WORD Stream;		// Stream index
			WORD Offset;		// Offset in the stream in bytes
			d3ddecltype Type;	// Data type
			BYTE Method;		// Processing method
			d3ddecluse Usage;	// Semantics
			BYTE UsageIndex;	// Semantic index
		};

		d3dvertelem decl[MAX_FVF_DECL_SIZE]; UINT numElements = 0;
		vertex_decl->GetDeclaration((D3DVERTEXELEMENT9*)decl, &numElements);
		int break_me = 1; // look into decl

#if 0
		if (primlist)
		{
			IDirect3DVertexBuffer9* vb = nullptr; UINT t_stride = 0u, t_offset = 0u;
			dev->GetStreamSource(0, &vb, &t_offset, &t_stride);

			//auto first_vert = *reinterpret_cast<std::uint32_t*>(RENDERER_BASE + 0x17547C);
			//auto num_verts_real = *reinterpret_cast<std::uint32_t*>(RENDERER_BASE + 0x1754A0);

			IDirect3DIndexBuffer9* ib = nullptr;
			if (SUCCEEDED(dev->GetIndices(&ib)))
			{
				void* ib_data; // lock index buffer to retrieve the relevant vertex indices
				if (SUCCEEDED(ib->Lock(0, 0, &ib_data, D3DLOCK_READONLY)))
				{
					// add relevant indices without duplicates
					std::unordered_set<std::uint16_t> indices; indices.reserve(primlist->m_NumIndices);

					for (auto i = 0u; i < (std::uint32_t)primlist->m_NumIndices; i++) {
						indices.insert(static_cast<std::uint16_t*>(ib_data)[primlist->m_FirstIndex + i]);
					}

					ib->Unlock();

					// get the range of vertices that we are going to work with
					UINT min_vert = 0u, max_vert = 0u;
					{
						auto [min_it, max_it] = std::minmax_element(indices.begin(), indices.end());
						min_vert = *min_it;
						max_vert = *max_it;
					}

					void* src_buffer_data;

					// lock vertex buffer from first used vertex (in total bytes) to X used vertices (in total bytes)
					if (SUCCEEDED(vb->Lock(min_vert * t_stride, max_vert * t_stride, &src_buffer_data, 0)))
					{
						struct src_vert
						{
							Vector pos;				 
							D3DCOLOR color;
							Vector4D tc0;		 
							Vector4D tc1;		 
							Vector4D tc2; 
							Vector2D tc3;
							Vector4D tc4;			 
							//Vector4D tc5;			 
							//Vector4D tc6;
							//Vector4D tc7;
						};

						for (auto i : indices)
						{
							// we need to subtract min_vert because we locked @ min_vert which is the start of our lock
							i -= static_cast<std::uint16_t>(min_vert);

							const auto v_pos_in_src_buffer = i * t_stride;
							const auto src = reinterpret_cast<src_vert*>(((DWORD)src_buffer_data + v_pos_in_src_buffer));

							//if (src->tc7.z != 1.337f)
							{
								//src->tc7.z = 1.337f;
								src->tc0.x = std::lerp(src->tc0.z, src->tc0.x, src->tc3.x);
								src->tc0.y = std::lerp(src->tc0.w, src->tc0.y, src->tc3.y);
							}
						}

						vb->Unlock();
					}
				}
			}
		}
#endif
#endif
	}


	void __fastcall tbl_hk::model_renderer::DrawModelExecute::Detour(void* ecx, void* edx, void* oo, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		const auto dev = game::get_d3d_device();
		dev->GetVertexShader(&ff_model::s_shader);
		dev->SetTransform(D3DTS_WORLD, &game::IDENTITY);

		// get origin of wheatly for flashlight calc. on bts3
		if (pInfo.flags == 0x9 && pInfo.pModel->radius == 19.3280182f &&
			map_settings::is_level.sp_a2_bts3)
		{
			if (std::string_view(pInfo.pModel->szPathName).contains("sphere")) {
				api::remix_lights::bts3_set_flashlight_start_pos(pInfo.origin);
			}
		}

		// MODELFLAG_MATERIALPROXY | MODELFLAG_STUDIOHDR_AMBIENT_BOOST
		else if (pInfo.flags == 0x80000011 || pInfo.flags == 0x11)
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
		
		tbl_hk::model_renderer::table.original<FN>(Index)(ecx, edx, oo, state, pInfo, pCustomBoneToWorld);

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
		}
	}

	// #
	// #

	// This is a mid-hook function that's called when the vertex buffer used to render painted BSP is being build
	// > Gets called right after the og function assigned the last uv coord
	// > Doing this here saves a lot of performance - skipping the need to lock and unlock the VB for each surface in 'render_painted_surface' 
	void BuildMSurfaceVertexArrays_mid_hk(CMeshBuilder* builder)
	{
		// only modify if is_rendering_paint is true because this function is also used elsewhere
		if (is_rendering_paint)
		{
			const auto o_tc_base = builder->m_VertexBuilder.m_pCurrTexCoord[0];
			const auto o_tc_lmap = builder->m_VertexBuilder.m_pCurrTexCoord[1];
			const auto o_tc_lmap_offset = builder->m_VertexBuilder.m_pCurrTexCoord[2];

			Vector2D tc_lmap = { o_tc_lmap[0], o_tc_lmap[1] };
			Vector2D tc_offs = { o_tc_lmap_offset[0], o_tc_lmap_offset[1] };

			// same code as in 'render_painted_surface'
			Vector2D tc_base = tc_lmap + tc_offs;
			tc_base = tc_base - ((tc_base + tc_offs) - tc_base);

			o_tc_base[0] = tc_base[0];
			o_tc_base[1] = tc_base[1];
		}
	}

	HOOK_RETN_PLACE_DEF(BuildMSurfaceVertexArrays_retn_addr);
	void __declspec(naked) BuildMSurfaceVertexArrays_stub()
	{
		__asm
		{
			// og
			movss   dword ptr[eax + 4], xmm0;

			pushad;
			push	esi; // builder
			call	BuildMSurfaceVertexArrays_mid_hk;
			add		esp, 4;
			popad;

			jmp		BuildMSurfaceVertexArrays_retn_addr;
		}
	}

	// Helper function to draw portal gel's
	// > will directly edit the vertex buffer when brushmodels are rendered
	void render_painted_surface(prim_fvf_context& ctx, CPrimList* primlist)
	{
		/*	// vs
			float3 vPos : POSITION;
			float4 vNormal : NORMAL;
			float2 vBaseTexCoord : TEXCOORD0;
			float2 vLightmapTexCoord : TEXCOORD1;
			float2 vLightmapTexCoordOffset : TEXCOORD2;

			{
				o.lightmapTexCoord1And2.xy = v.vLightmapTexCoord + v.vLightmapTexCoordOffset;

				float2 lightmapTexCoord2 = o.lightmapTexCoord1And2.xy + v.vLightmapTexCoordOffset;
				o.lightmapTexCoord1And2.w = lightmapTexCoord2.x;
				o.lightmapTexCoord1And2.z = lightmapTexCoord2.y;
			}

			// ps
			float2 paintCoord = lightmapTexCoord1And2.xy - ( lightmapTexCoord1And2.wz - lightmapTexCoord1And2.xy );

			// sampler PaintRefractSampler	: register(s6);
			// sampler paintSampler			: register(s9);
			// sampler SplatNormalSampler	: register(s10);
		*/

		const auto dev = game::get_d3d_device();
		const auto shaderapi = game::get_shaderapi();
		//lookat_vertex_decl(dev);

		IDirect3DVertexBuffer9* vb = nullptr; UINT t_stride = 0u, t_offset = 0u;
		dev->GetStreamSource(0, &vb, &t_offset, &t_stride);

		if (vb)
		{
			void* src_buffer_data;
			
			// This can be pretty bad performance wise if used on a lot of individual surfaces
			// > BSP is not rendered in batches so we would lock and unlock the VB for each surface
			// > Brushmodels are rendered in batches -> waaaay less locks
			// - Brushmodels are considered static if mat_forcedynamic or mat_drawflat is not 1 (vb is NOT recreated every frame)
			if (is_rendering_bmodel_paint)
			{
				IDirect3DIndexBuffer9* ib = nullptr;
				if (SUCCEEDED(dev->GetIndices(&ib)))
				{
					void* ib_data; // lock index buffer to retrieve the relevant vertex indices
					if (SUCCEEDED(ib->Lock(0, 0, &ib_data, D3DLOCK_READONLY)))
					{
						// add relevant indices without duplicates
						std::unordered_set<std::uint16_t> indices; indices.reserve(primlist->m_NumIndices);
						for (auto i = 0u; i < (std::uint32_t)primlist->m_NumIndices; i++)
						{
							indices.insert(static_cast<std::uint16_t*>(ib_data)[primlist->m_FirstIndex + i]);
						}

						ib->Unlock();

						// get the range of vertices that we are going to work with
						UINT min_vert = 0u, max_vert = 0u;
						{
							auto [min_it, max_it] = std::minmax_element(indices.begin(), indices.end());
							min_vert = *min_it;
							max_vert = *max_it;
						}

						// lock vertex buffer from first used vertex (in total bytes) to X used vertices (in total bytes)
						if (SUCCEEDED(vb->Lock(min_vert * t_stride, max_vert * t_stride, &src_buffer_data, 0)))
						{
							struct src_vert
							{
								Vector pos;				 // 12
								Vector normal;			 // 12	> 24
								Vector2D tc_base;		 // 8	> 32
								Vector2D tc_lmap;		 // 8	> 40
								Vector2D tc_lmap_offset; // 8	> 48
								Vector2D tc3;			 // 8	> 56 // @48 actually float3 tangent
								Vector2D tc4;			 // 8	> 64 // @60 actually float3 binormal
								Vector2D tc5;			 // 8	> 72 
								Vector2D tc6;			 // 8	> 80 // last 8 byte junk?
							};

							for (auto i : indices)
							{
								// we need to subtract min_vert because we locked @ min_vert which is the start of our lock
								i -= static_cast<std::uint16_t>(min_vert);

								const auto v_pos_in_src_buffer = i * t_stride;
								const auto src = reinterpret_cast<src_vert*>(((DWORD)src_buffer_data + v_pos_in_src_buffer));

								// mark this vertex as modified - yes this even works if paint is cleared 
								if (src->tc6.x != 1.337f)
								{
									src->tc6.x = 1.337f;

									// calc paint coordinates
									src->tc_base = src->tc_lmap + src->tc_lmap_offset;
									src->tc_base = src->tc_base - ((src->tc_base + src->tc_lmap_offset) - src->tc_base);
								}
							}

							vb->Unlock();
						}
					}
				}
			}


			// #
			// this part of the code will be used for BSP paint & brushmodel paint

			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX7);

			// assign paint map texture to texture slot 0
			if (ctx.info.buffer_state.m_BoundTexture[9])
			{
				if (const auto  paint_map = shaderapi->vtbl->GetD3DTexture(shaderapi, nullptr, ctx.info.buffer_state.m_BoundTexture[9]);
					paint_map)
				{
					ctx.save_texture(dev, 0);
					dev->SetTexture(0, paint_map);
				}
			}

			// this requires dxvk-remix modifications (https://github.com/NVIDIAGameWorks/dxvk-remix/pull/79)

			// set remix texture categories
			ctx.save_rs(dev, (D3DRENDERSTATETYPE)42);
			dev->SetRenderState((D3DRENDERSTATETYPE)42, IgnoreOpacityMicromap | DecalStatic);

			// set custom remix hash
			ctx.save_rs(dev, (D3DRENDERSTATETYPE)150);
			dev->SetRenderState((D3DRENDERSTATETYPE)150, 0x1337);
		}
	}

#define SPRITE_TRAIL_TEST
#ifdef SPRITE_TRAIL_TEST
	// works good on some maps but breaks on others?
	void RenderSpriteTrail_mid_hk(CMeshBuilder* builder, [[maybe_unused]] int type)
	{
		// TODO: use hlslpp
		//using namespace hlslpp;

		const auto dev = game::get_d3d_device();

		/*if (builder->m_VertexBuilder.m_VertexSize_Position == 144)
		{
			int x = 1;
		}*/

		int orientation = 0;
		if (const auto shaderapi = game::get_shaderapi();
			shaderapi)
		{
			if (const auto material = shaderapi->vtbl->GetBoundMaterial(shaderapi, nullptr);
				material)
			{
				//const auto name = material->vftable->GetName(material);

				IMaterialVar* var_out = nullptr;
				if (has_materialvar(material, "$ORIENTATION", &var_out))
				{
					if (var_out) {
						orientation = var_out->vftable->GetIntValueInternal(var_out) != 0;
					}
				}
			}
		}

		// #
		// #

		auto CatmullRomSpline = [](const Vector4D& a, const Vector4D& b, const Vector4D& c, const Vector4D& d, const float t)
			{
				return b + 0.5f * t * (c - a + t * (2.0f * a - 5.0f * b + 4.0f * c - d + t * (-a + 3.0f * b - 3.0f * c + d)));
			};

		auto DCatmullRomSpline3 = [](const Vector& a, const Vector& b, const Vector& c, const Vector& d, const float t)
			{
				return 0.5f * (c - a + t * (2.0f * a - 5 * b + 4 * c - d + t * (3.0f * b - a - 3.0f * c + d))
					+ t * (2.0f * a - 5.0f * b + 4 * c - d + 2.0f * (t * (3 * b - a - 3.0f * c + d))));
			};


		float ALPHATFADE, RADIUSTFADE;
		{
			float v[4] = {}; dev->GetVertexShaderConstantF(57, v, 1);
			ALPHATFADE = v[2];
			RADIUSTFADE = v[3];
		}

		Vector eyePos;
		{
			float v[4] = {}; dev->GetVertexShaderConstantF(2, v, 1);
			eyePos = Vector(v[0], v[1], v[2]);
		}

		/*if (type)
		{
			int x = 1;
		}*/

#if 1
		for (auto v = 0; v < builder->m_VertexBuilder.m_nVertexCount; v++)
		{
			const auto v_pos_in_src_buffer = v * builder->m_VertexBuilder.m_VertexSize_Position;

			const auto src_vParms = reinterpret_cast<Vector*>(((DWORD)builder->m_VertexBuilder.m_pCurrPosition + v_pos_in_src_buffer));
			const auto dest_pos = reinterpret_cast<Vector*>(src_vParms);

			const auto src_vTint = reinterpret_cast<D3DCOLOR*>(((DWORD)builder->m_VertexBuilder.m_pCurrColor + v_pos_in_src_buffer));

			const auto src_vSplinePt0 = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[0] + v_pos_in_src_buffer));
			const auto dest_tc = reinterpret_cast<Vector2D*>(src_vSplinePt0);

			const auto src_vSplinePt1 = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[1] + v_pos_in_src_buffer));
			const auto src_vSplinePt2 = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[2] + v_pos_in_src_buffer));
			const auto src_vSplinePt3 = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[3] + v_pos_in_src_buffer));
			const auto src_vTexCoordRange = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[4] + v_pos_in_src_buffer));
			const auto src_vEndPointColor = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[5] + v_pos_in_src_buffer));

			const auto src_vNormal0 = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[6] + v_pos_in_src_buffer));
			const auto src_vNormal1 = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[7] + v_pos_in_src_buffer));

			// save vParms (because we will be overriding them when writing pos)
			const float parmsX = src_vParms->x;
			const float parmsY = src_vParms->y;
			const float parmsZ = src_vParms->z;

			const auto P0 = *src_vSplinePt0;
			const auto P1 = *src_vSplinePt1;
			const auto P2 = *src_vSplinePt2;
			const auto P3 = *src_vSplinePt3;

			auto posrad = CatmullRomSpline(P0, P1, P2, P3, parmsX);
			posrad.w *= std::lerp(1.0f, RADIUSTFADE, parmsX);

			Vector v2p = { 0.0f, 0.0f, 1.0f };
			if (orientation == 0) {
				v2p = { posrad.x - eyePos.x, posrad.y - eyePos.y, posrad.z - eyePos.z }; // screen aligned
			}
			else if (orientation == 3) 
			{
				v2p.x = std::lerp(src_vNormal0->x, src_vNormal1->x, parmsX);
				v2p.y = std::lerp(src_vNormal0->y, src_vNormal1->y, parmsX);
				v2p.z = std::lerp(src_vNormal0->z, src_vNormal1->z, parmsX);
			}
			
			Vector tangent = DCatmullRomSpline3(P0, P1, P2, P3, parmsX);

			//float3 ofs = normalize(cross(v2p, normalize(tangent)));
			tangent.NormalizeChecked();
			Vector ofs = v2p.Cross(tangent); // maybe switch these - no difference
			ofs.NormalizeChecked();

			//posrad.xyz += ofs * (posrad.w * (v.vParms.z - .5));
			const auto add = ofs.Scale(posrad.w * (parmsZ - 0.5f));
			posrad.x += add.x;
			posrad.y += add.y;
			posrad.z += add.z;

			// pos
			dest_pos->x = posrad.x;
			dest_pos->y = posrad.y;
			dest_pos->z = posrad.z;

			dest_tc->x = std::lerp(src_vTexCoordRange->z, src_vTexCoordRange->x, parmsZ);
			dest_tc->y = std::lerp(src_vTexCoordRange->y, src_vTexCoordRange->w, parmsY);

			// unpack color
			Vector4D color;
			color.x = static_cast<float>((*src_vTint >> 16) & 0xFF) / 255.0f * 1.0f;
			color.y = static_cast<float>((*src_vTint >> 8) & 0xFF) / 255.0f * 1.0f;
			color.z = static_cast<float>((*src_vTint >> 0) & 0xFF) / 255.0f * 1.0f;
			color.w = static_cast<float>((*src_vTint >> 24) & 0xFF) / 255.0f * 0.1f; // ! 0.1

			color.x = std::lerp(color.x, src_vEndPointColor->x, parmsX);
			color.y = std::lerp(color.y, src_vEndPointColor->y, parmsX);
			color.z = std::lerp(color.z, src_vEndPointColor->z, parmsX);
			color.w = std::lerp(color.w, src_vEndPointColor->w, parmsX);
			color.w *= std::lerp(1.0f, ALPHATFADE, parmsX); //parmsX); 

			// write color
			*src_vTint = D3DCOLOR_COLORVALUE(color.x, color.y, color.z, color.w);
			//*src_vTint = D3DCOLOR_COLORVALUE(1, 0, 0, color.w * std::lerp(1.0f, ALPHATFADE, parmsX));
		}
#endif

		//for (auto v = 0; v < builder->m_VertexBuilder.m_nVertexCount; v++)
		//{
		//	const auto v_pos_in_src_buffer = v * builder->m_VertexBuilder.m_VertexSize_Position;

		//	const auto src_vParms = reinterpret_cast<Vector*>(((DWORD)builder->m_VertexBuilder.m_pCurrPosition + v_pos_in_src_buffer));
		//	const auto src_vTint = reinterpret_cast<D3DCOLOR*>(((DWORD)builder->m_VertexBuilder.m_pCurrColor + v_pos_in_src_buffer));
		//	const auto src_vEndPointColor = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[5] + v_pos_in_src_buffer));

		//	// unpack color
		//	Vector4D color;
		//	color.x = static_cast<float>((*src_vTint >> 16) & 0xFF) / 255.0f * 1.0f;
		//	color.y = static_cast<float>((*src_vTint >> 8) & 0xFF) / 255.0f * 1.0f;
		//	color.z = static_cast<float>((*src_vTint >> 0) & 0xFF) / 255.0f * 1.0f;
		//	color.w = static_cast<float>((*src_vTint >> 24) & 0xFF) / 255.0f * 0.1f;

		//	color.x = std::lerp(color.x, src_vEndPointColor->x, src_vParms->x);
		//	color.y = std::lerp(color.y, src_vEndPointColor->y, src_vParms->x);
		//	color.z = std::lerp(color.z, src_vEndPointColor->z, src_vParms->x);
		//	color.w = std::lerp(color.w, src_vEndPointColor->w, src_vParms->x);
		//	color.w *= std::lerp(1.0f, ALPHATFADE, src_vParms->x);

		//	// write color
		//	*src_vTint = D3DCOLOR_COLORVALUE(color.x, color.y, color.z, color.w);
		//}
	}

	HOOK_RETN_PLACE_DEF(RenderSpriteCardFastRopeVertex_retn_addr);
	void __declspec(naked) RenderSpriteCardFastRopeVertex_stub()
	{
		__asm
		{
			pushad;
			push	0;
			push	edi; // builder
			call	RenderSpriteTrail_mid_hk;
			add		esp, 8;
			popad;

			// og
			mov     ecx, [edi + 0xF4];
			jmp		RenderSpriteCardFastRopeVertex_retn_addr;
		}
	}

	HOOK_RETN_PLACE_DEF(RenderSpriteCardFastRopeVertexNormal_retn_addr);
	void __declspec(naked) RenderSpriteCardFastRopeVertexNormal_stub()
	{
		__asm
		{
			pushad;
			push	0;
			push	edi; // builder
			call	RenderSpriteTrail_mid_hk;
			add		esp, 8;
			popad;

			// og
			mov     ecx, [edi + 0xF4];
			jmp		RenderSpriteCardFastRopeVertexNormal_retn_addr;
		}
	}

	HOOK_RETN_PLACE_DEF(RenderSpriteCardFastRopeVertexNormalCache_retn_addr);
	void __declspec(naked) RenderSpriteCardFastRopeVertexNormalCache_stub()
	{
		__asm
		{
			pushad;
			push	0;
			push	edi; // builder
			call	RenderSpriteTrail_mid_hk;
			add		esp, 8;
			popad;

			// og
			mov     ecx, [edi + 0xF4];
			jmp		RenderSpriteCardFastRopeVertexNormalCache_retn_addr;
		}
	}

	HOOK_RETN_PLACE_DEF(RenderSpritesTrail_Render_retn_addr);
	void __declspec(naked) RenderSpritesTrail_Render_stub()
	{
		__asm
		{
			pushad;
			push	1;
			lea     eax, [ebp - 0x22C]
			push	eax; // builder
			call	RenderSpriteTrail_mid_hk;
			add		esp, 8;
			popad;

			// og
			mov     ecx, [ebp - 0x128];
			jmp		RenderSpritesTrail_Render_retn_addr;
		}
	}
#endif


	/**
	 * Called right before unlocking the sprite mesh. m_nCurrentVertex should match m_nVertexCount
	 */
	void fix_sprite_card_texcoords_mid_hk(CMeshBuilder* builder, [[maybe_unused]] int type)
	{
		const auto dev = game::get_d3d_device();

		bool use_crop = false;
		if (const auto shaderapi = game::get_shaderapi();
			shaderapi)
		{
			if (const auto material = shaderapi->vtbl->GetBoundMaterial(shaderapi, nullptr);
				material)
			{
				//const auto name = material->vftable->GetName(material);
				IMaterialVar* var_out = nullptr;
				if (has_materialvar(material, "$CROPFACTOR", &var_out))
				{
					if (var_out) {
						use_crop = var_out->vftable->GetVecValueInternal1(var_out)[0] != 1.0f || var_out->vftable->GetVecValueInternal1(var_out)[1] != 1.0f;
					}
				}
			}
		}

		float g_vCropFactor[4] = {};
		dev->GetVertexShaderConstantF(15, g_vCropFactor, 1);

		// meshBuilder positions are set to the last vertex it created
		for (auto v = builder->m_VertexBuilder.m_nVertexCount; v > 0; v--)
		{
			const auto v_pos_in_src_buffer = v * builder->m_VertexBuilder.m_VertexSize_Position;

			//const auto src_vParms = reinterpret_cast<Vector*>(((DWORD)builder->m_VertexBuilder.m_pCurrPosition + v_pos_in_src_buffer));
			//const auto dest_pos = reinterpret_cast<Vector*>(src_vParms);
			//const auto src_vTint = reinterpret_cast<D3DCOLOR*>(((DWORD)builder->m_VertexBuilder.m_pCurrColor - v_pos_in_src_buffer));

			const auto src_tc0 = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[0] - v_pos_in_src_buffer));
			const auto dest_tc = reinterpret_cast<Vector2D*>(src_tc0);

			//const auto src_tc1 = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[1] + v_pos_in_src_buffer));
			//const auto src_vParms = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[2] - v_pos_in_src_buffer));
			const auto src_tc3 = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[3] - v_pos_in_src_buffer));
			//const auto src_tc4 = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[4] + v_pos_in_src_buffer));
			//const auto src_tc5 = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[5] + v_pos_in_src_buffer));
			//const auto src_vParms1 = reinterpret_cast<Vector4D*>(((DWORD)builder->m_VertexBuilder.m_pCurrTexCoord[7] - v_pos_in_src_buffer));

			if (use_crop) 
			{
				dest_tc->x = std::lerp(src_tc0->z, src_tc0->x, src_tc3->x * g_vCropFactor[0] + g_vCropFactor[2]);
				dest_tc->y = std::lerp(src_tc0->w, src_tc0->y, src_tc3->y * g_vCropFactor[1] + g_vCropFactor[3]);
			}
			else
			{
				dest_tc->x = std::lerp(src_tc0->z, src_tc0->x, src_tc3->x);
				dest_tc->y = std::lerp(src_tc0->w, src_tc0->y, src_tc3->y);
			}

			/*{
				Vector4D color;
				color.x = static_cast<float>((*src_vTint >> 16) & 0xFF) / 255.0f * 1.0f;
				color.y = static_cast<float>((*src_vTint >> 8) & 0xFF) / 255.0f * 1.0f;
				color.z = static_cast<float>((*src_vTint >> 0) & 0xFF) / 255.0f * 1.0f;
				color.w = static_cast<float>((*src_vTint >> 24) & 0xFF) / 255.0f * 1.0f;

				*src_vTint = D3DCOLOR_COLORVALUE(color.x, color.y, color.z, color.w);
			}*/
		}
	}

	HOOK_RETN_PLACE_DEF(RenderSpriteCardNew_retn_addr);
	void __declspec(naked) RenderSpriteCardNew_stub()
	{
		__asm
		{
			pushad;
			push	0;
			lea     eax, [ebp - 0x25C]
			push	eax; // builder
			call	fix_sprite_card_texcoords_mid_hk;
			add		esp, 8;
			popad;

			// og
			mov     ecx, [ebp - 0x1A8];
			jmp		RenderSpriteCardNew_retn_addr;
		}
	}


	// ##
	// ##

	void render_emancipation_grill(prim_fvf_context& ctx)
	{
		const auto dev = game::get_d3d_device();

		// ps
		//const float4 g_vWriteDepthToAlpha_FlowParams : register(c3);
		//#define g_bWriteDepthToAlpha			( g_vWriteDepthToAlpha_FlowParams.x )
		//#define g_flTime						( g_vWriteDepthToAlpha_FlowParams.y )
		//#define g_flPowerUp						( g_vWriteDepthToAlpha_FlowParams.z )
		//#define g_flIntensity					( g_vWriteDepthToAlpha_FlowParams.w )

		// const float4 g_vFlowParams1 : register( c6 );
		// #define g_flWorldUvScale  ( g_vFlowParams1.x ) // 1.0f / 10.0f
		// #define g_flOutputIntensity ( g_vFlowParams1.w ) 

		//const float4 g_vFlowColor : register(c8);
		

		float g_vWriteDepthToAlpha_FlowParams[4] = {};
		dev->GetPixelShaderConstantF(3, g_vWriteDepthToAlpha_FlowParams, 1);
		const float g_flTime = g_vWriteDepthToAlpha_FlowParams[1];
		const float g_flPowerUp = g_vWriteDepthToAlpha_FlowParams[2];//1
		//const float g_flIntensity = g_vWriteDepthToAlpha_FlowParams[3]; // 1

		float g_vFlowColor[4] = {};
		dev->GetPixelShaderConstantF(8, g_vFlowColor, 1); // 0.025, 0.08, 0.1


		bool side_emitters = ctx.info.material_name.ends_with("side_emitters");
		//ctx.modifiers.do_not_render = true;

		ctx.save_vs(dev); 

		ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
		ctx.save_tss(dev, D3DTSS_COLORARG1);
		ctx.save_tss(dev, D3DTSS_COLORARG2);
		ctx.save_tss(dev, D3DTSS_COLOROP);

		ctx.save_tss(dev, D3DTSS_ALPHAOP);
		ctx.save_tss(dev, D3DTSS_ALPHAARG2);

		//if (side_emitters) 
		{
			ctx.save_texture(dev, 0);
			dev->SetTexture(0, tex_addons::emancipation_grill);

			dev->SetRenderState(D3DRS_TEXTUREFACTOR, 
				
				side_emitters ? 
					D3DCOLOR_COLORVALUE(
					g_vFlowColor[0] * g_flPowerUp * 0.5f, 
					g_vFlowColor[1] * g_flPowerUp * 0.5f, 
					g_vFlowColor[2] * g_flPowerUp * 0.35f, 
					0.7f * g_flPowerUp)

				  : D3DCOLOR_COLORVALUE(
					  0.2f * g_flPowerUp, 0.4f * g_flPowerUp, 0.52f * g_flPowerUp, 0.05f * g_flPowerUp)
			);
		}
		//else
		//{
		//	//dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(0.2f * g_flPowerUp, 0.4f * g_flPowerUp, 0.52f * g_flPowerUp, 0.05f * g_flPowerUp));
		//	dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(1.0f, 0.0f, 0.0f, 0.05f * g_flPowerUp));
		//}
		
		dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
		dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

		dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_ADD);
		dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

		D3DXMATRIX current_transform = {};
		dev->GetTransform(D3DTS_TEXTURE0, &current_transform);

		D3DXMATRIX scale_matrix; 
		if (ctx.info.material_name.ends_with("_l") || ctx.info.material_name.ends_with("_r")) 
		{
			D3DXMatrixScaling(&scale_matrix, 0.25f - (std::sinf(g_flTime) * 0.0015f), 0.5f, 1.0f);
			current_transform(3, 0) += 0.335f; // tc scroll
			current_transform(3, 1) = g_flTime * 0.005f; // tc scroll
		}
		else 
		{
			D3DXMatrixScaling(&scale_matrix, (side_emitters ? 0.7f : 1.0f) - (std::sinf(g_flTime) * 0.003f), (side_emitters ? 0.7f : 0.5f) /*- (std::sinf(g_flTime) * 0.005f)*/, 1.0f);
			current_transform(3, 1) = g_flTime * 0.005f; // tc scroll
		}

		current_transform *= scale_matrix;
		ctx.modifiers.as_emancipation_grill = !side_emitters;
		ctx.modifiers.emancipation_scale = { 1.2f - (std::cosf(g_flTime * 0.01f) * 1.0f), 1.2f - (std::cosf(g_flTime * 0.01f) * 1.0f) };
		ctx.modifiers.emancipation_offset = { g_flTime * 0.01f, g_flTime * -0.0015f };
		ctx.modifiers.emancipation_color_scale = g_flPowerUp;

		ctx.set_texture_transform(dev, &current_transform);
		ctx.save_tss(dev, D3DTSS_TEXTURETRANSFORMFLAGS);
		dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

		dev->SetVertexShader(nullptr);  
		dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64 :: tc @ 24
		dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
	}

	void handle_portal_mesh(prim_fvf_context& ctx, std::uint32_t portal_index, bool is_tinted_portal = false)
	{
		const auto dev = game::get_d3d_device();

		if (tex_addons::portal_mask)
		{
			ctx.save_texture(dev, 1);
			//dev->SetTexture(1, tex_addons::portal_mask); // does not work on coop for some reason
			dev->SetTexture(1, nullptr); // so let remix estimate the portal
		}

		float open_amount = 0.0f;
		float open_amount_linked = 0.0f;
		bool is_linked = false;
		const auto& p = model_render::game_portals;

		// cba to refactor rn
		// todo: fix portal colors on coop if possible
		switch (portal_index)
		{
			default:
			case 0:
			{
				open_amount = p[0].open_amount;
				open_amount_linked = p[1].open_amount;
				is_linked = p[0].is_linked;
				if (tex_addons::portal_blue)
				{
					// render static portal overlay - currently not visible through walls
#if 0
					ctx.modifiers.dual_render_with_specified_texture = true;
					ctx.modifiers.dual_render_texture = tex_addons::portal_blue_overlay;
					ctx.modifiers.dual_render_with_specified_texture_blend_add = true;
#endif
					ctx.save_texture(dev, 0);
					dev->SetTexture(0, tex_addons::portal_blue);
				}
				break;
			}

			case 1:
			{
				open_amount = p[1].open_amount;
				open_amount_linked = p[0].open_amount;
				is_linked = p[1].is_linked;
				if (tex_addons::portal_orange)
				{
					// render static portal overlay - currently not visible through walls
#if 0
					ctx.modifiers.dual_render_with_specified_texture = true;
					ctx.modifiers.dual_render_texture = tex_addons::portal_orange_overlay;
					ctx.modifiers.dual_render_with_specified_texture_blend_add = true;
#endif
					ctx.save_texture(dev, 0);
					dev->SetTexture(0, tex_addons::portal_orange);
				}
				break;
			}

			case 2:
			{
				open_amount = p[2].open_amount;
				open_amount_linked = p[3].open_amount;
				is_linked = p[2].is_linked;
				if (tex_addons::portal_purple)
				{
					ctx.save_texture(dev, 0);
					dev->SetTexture(0, tex_addons::portal_purple);
				}
				break;
			}

			case 3:
			{
				open_amount = p[3].open_amount;
				open_amount_linked = p[2].open_amount;
				is_linked = p[3].is_linked;
				if (tex_addons::portal_red)
				{
					ctx.save_texture(dev, 0);
					dev->SetTexture(0, tex_addons::portal_red);
				}
				break;
			}
		}

		// replace with wireframe (makes life much easier)
		if (!is_tinted_portal && ctx.info.shader_name != "Wireframe_DX9")
		{
			const auto cmat = ctx.info.material;
			cmat->vftable->SetShader(ctx.info.material, "Wireframe");

			//bool found = false;
			//auto var = cmat->vftable->FindVar(cmat, nullptr, "$nocull", &found, false);
			////auto varname = cullvar->vftable->GetName(cullvar);

			//if (!found)
			//{
			//	utils::function<IMaterialVar* (IMaterialInternal* pMaterial, const char* pKey, int val)> IMaterialVar_Create = MATERIALSTYSTEM_BASE + 0x1A2F0;
			//	auto var = IMaterialVar_Create(cmat, "$PortalColorGradientLight", 1);

			//	cmat->vftable->AddMaterialVar(cmat, nullptr, var);
			//	var = cmat->vftable->FindVar(cmat, nullptr, "$nocull", &found, false);
			//}
		}

		// #
		// scale portal on opening

		// portal opening value is eased in -> apply inverse ease-in
		float s = std::sqrtf(open_amount);

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

		if (!is_linked)
		{
			dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0, 255));
			dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		}
		else
		{
			// transition n
			int t = static_cast<int>(std::roundf(((1.0f - std::sqrtf(open_amount_linked)) - 0.1f) * (255.0f / 0.9f)));
			t = static_cast<int>(std::clamp(t, 0, 255));

			dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0, t));
			dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		}
	}

	// 
	// main render path for every surface

	void cmeshdx8_renderpass_pre_draw(CMeshDX8* mesh, [[maybe_unused]] CPrimList* primlist)
	{

#if defined(BENCHMARK)
		utils::benchmark bench;
#endif

		const auto dev = game::get_d3d_device();

		IDirect3DVertexBuffer9* b = nullptr;
		UINT stride = 0;
		{
			UINT ofs = 0; dev->GetStreamSource(0, &b, &ofs, &stride);
		}

		auto& ctx = model_render::primctx;
		const auto shaderapi = game::get_shaderapi();

		if (ctx.get_info_for_pass(shaderapi)) 
		{
			// added format check
			if (mesh->m_VertexFormat == 0x2480033 || mesh->m_VertexFormat == 0x80033) 
			{
				if (ctx.info.shader_name.starts_with("Wa") && ctx.info.shader_name.contains("Water"))
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
		}

		/*if (ctx.info.material_name.contains("props_foliage"))
		{
			int break_me = 1;    
		}*/

		if (ff_bmodel::s_shader && mesh->m_VertexFormat == 0x2480033)
		{
			//ctx.modifiers.do_not_render = true;
			dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX7);
			dev->SetVertexShader(nullptr);

			if (is_rendering_bmodel_paint) {
				render_painted_surface(ctx, primlist);
			}
		}

		// player model - gun - grabable stuff - portal button - portal door - stairs
		else if (ff_model::s_shader && mesh->m_VertexFormat == 0xa0003)
		{
			//ctx.modifiers.do_not_render = true;

			// viewmodel
			if (ctx.info.buffer_state.m_Transform[2].m[3][2] == -1.00003529f)
			{
				ctx.save_view_transform(dev);
				ctx.save_projection_transform(dev);
				dev->SetTransform(D3DTS_VIEW, &ctx.info.buffer_state.m_Transform[1]);
				dev->SetTransform(D3DTS_PROJECTION, &ctx.info.buffer_state.m_Transform[2]);
			}
			else if (ctx.info.material_name.contains("models/props_destruction/glass_")) 
			{
				//ctx.modifiers.do_not_render = true;
				if (tex_addons::glass_shards)
				{
					ctx.save_texture(dev, 0);
					dev->SetTexture(0, tex_addons::glass_shards);
				}
			}
			else if (ctx.info.material_name.starts_with("gla"))
			{
				// glass/glasswindow_ ...
				if (ctx.info.material_name.starts_with("glass/glassw"))
				{
					if (tex_addons::glass_window_lamps)
					{
						ctx.save_texture(dev, 0);
						dev->SetTexture(0, tex_addons::glass_window_lamps);
					}
				}
			}
			// models/player/chell/gambler_eyeball_ l/r
			else if (ctx.info.material_name.contains("models/player/chell/gambler_eyeball"))
			{
				if (const auto basemap2 = shaderapi->vtbl->GetD3DTexture(shaderapi, nullptr, ctx.info.buffer_state.m_BoundTexture[1]); basemap2) 
				{
					ctx.save_texture(dev, 0);
					dev->SetTexture(0, basemap2);
				}
			}

			dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX6);
			dev->SetVertexShader(nullptr); // vertexformat 0x00000000000a0003 
		}

		// this also renders the glass infront of white panel lamps
		// also renders some foliage (2nd level - emissive) - treeleaf
		else if (ff_model::s_shader) // 0xa0103
		{
			//ctx.modifiers.do_not_render = true;

			// replace all refract shaders with wireframe (ex. glass/containerwindow_)
			if (ctx.info.shader_name.starts_with("Re") && ctx.info.shader_name.contains("Refract_DX90") && !ctx.info.material_name.starts_with("glass/contain"))
			{
				// I think we are simply missing basetex0 here
				ctx.info.material->vftable->SetShader(ctx.info.material, "Wireframe");
			}

			// make sure that alpha testing is set
			else if (ctx.info.material_name.contains("props_foliage"))
			{
				//ctx.modifiers.do_not_render = true;
				ctx.save_rs(dev, D3DRS_ALPHABLENDENABLE);
				ctx.save_rs(dev, D3DRS_SRCBLEND);
				ctx.save_rs(dev, D3DRS_DESTBLEND);
				ctx.save_rs(dev, D3DRS_ALPHATESTENABLE);
				ctx.save_rs(dev, D3DRS_ALPHAFUNC);
				ctx.save_rs(dev, D3DRS_ALPHAREF);

				dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
				dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				dev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
				dev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
				dev->SetRenderState(D3DRS_ALPHAREF, 128);
			}

			// change observer window texture (models/props_lab/glasswindow_observation)
			else if (ctx.info.material_name.ends_with("tion") && ctx.info.material_name.ends_with("w_observation"))
			{
				if (tex_addons::glass_window_observ)
				{
					//dev->GetTexture(0, &ff_model::s_texture);
					ctx.save_texture(dev, 0);
					dev->SetTexture(0, tex_addons::glass_window_observ);
					add_light_to_texture_color_edit(0.9f, 1.3f, 1.5f, 0.05f);
				}
			}

			// glass
			else if (ctx.info.material_name.starts_with("gla"))
			{
				// glass/glasswindow_ ...
				if (ctx.info.material_name.starts_with("glass/glassw"))
				{
					if (tex_addons::glass_window_lamps)
					{
						//dev->GetTexture(0, &ff_model::s_texture);
						ctx.save_texture(dev, 0);
						dev->SetTexture(0, tex_addons::glass_window_lamps);
					}
				}
				// glass/containerwindow_
				else if (ctx.info.material_name.starts_with("glass/contain"))
				{
					if (const auto basemap2 = shaderapi->vtbl->GetD3DTexture(shaderapi, nullptr, ctx.info.buffer_state.m_BoundTexture[2]);
						basemap2)
					{
						ctx.save_texture(dev, 0);
						dev->SetTexture(0, basemap2);
					}

					// create a scaling matrix
					D3DXMATRIX scaleMatrix;
					D3DXMatrixScaling(&scaleMatrix, 1.0f, 29.0f, 1.0f);

					ctx.set_texture_transform(dev, &scaleMatrix);
					ctx.save_tss(dev, D3DTSS_TEXTURETRANSFORMFLAGS);
					dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
				}
			}

			else if (ctx.info.material_name.ends_with("01") && ctx.info.material_name == "models/props_hub/glados_chamber_dest01")
			{
				ctx.save_texture(dev, 0);
				dev->SetTexture(0, tex_addons::black_shader);
			}

			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3);
			dev->SetVertexShader(nullptr); // vertexformat 0x00000000000a0003
		}
		else
		{
			bool was_portal_related = false;

			// world geo - floor / walls --- "LightmappedGeneric"
			// this renders water but not the $bottommaterial
			if (mesh->m_VertexFormat == 0x2480033)
			{
				//ctx.modifiers.do_not_render = true;
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3); // tc @ 24
				dev->SetTransform(D3DTS_WORLD, &game::IDENTITY); 

				// scale water uv's
				if (ctx.modifiers.as_water)  
				{
					// create a scaling matrix
					D3DXMATRIX scaleMatrix;
					D3DXMatrixScaling(&scaleMatrix, 1.5f, 1.5f, 1.0f);

					ctx.save_ss(dev, D3DSAMP_ADDRESSU);
					ctx.save_ss(dev, D3DSAMP_ADDRESSV);
					dev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
					dev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

					ctx.set_texture_transform(dev, &scaleMatrix); 
					ctx.save_tss(dev, D3DTSS_TEXTURETRANSFORMFLAGS);
					dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
				}

				// painted surfaces use the 2nd and 3rd set of texcoords (lightmap coords)
				// -> so we need to edit the vertex buffer for each and every surface
				// mat_fullbright 1 does not draw paint
				if (is_rendering_paint) {
					render_painted_surface(ctx, primlist);
				}
			} 

			// transport tubes
			// billboard spites and decals
			// main menu bik background
			else if (mesh->m_VertexFormat == 0x80005) // stride 0x20
			{
				//ctx.modifiers.do_not_render = true;
				bool mod_shader = true;

				// transport tubes
				if (ctx.info.material_name.starts_with("eff")) 
				{
					if (ctx.info.material_name.contains("tractor_beam")) 
					{
						D3DXMATRIX ret = {};
						dev->GetTransform(D3DTS_TEXTURE0, &ret);

						// tc scroll
						ret(3, 1) = (float)main_module::framecount * 0.0015f;

						ctx.set_texture_transform(dev, &ret);
						ctx.save_tss(dev, D3DTSS_TEXTURETRANSFORMFLAGS);
						dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

						ctx.modifiers.as_transport_beam = false;
					}
				}
				// render bik using shaders
				else if (ctx.info.material_name.starts_with("videobik") || ctx.info.material_name.starts_with("media/"))
				{
					// set remix texture categories
					ctx.save_rs(dev, (D3DRENDERSTATETYPE)42);
					dev->SetRenderState((D3DRENDERSTATETYPE)42, DecalStatic);

					// set custom remix hash
					ctx.save_rs(dev, (D3DRENDERSTATETYPE)150);
					dev->SetRenderState((D3DRENDERSTATETYPE)150, utils::string_hash32(ctx.info.material_name));

					// works but not of much use if we cant use the albedo as emissive
					//ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
					//ctx.save_tss(dev, D3DTSS_COLORARG1);
					//ctx.save_tss(dev, D3DTSS_COLORARG2);
					//ctx.save_tss(dev, D3DTSS_COLOROP);
					//dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(255, 255, 255, 255));
					//dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
					//dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
					//dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

					mod_shader = false; 
				}

				if (mod_shader)
				{
					ctx.save_vs(dev);
					dev->SetVertexShader(nullptr);
					dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2);
					dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
				}
			}

			// laser platforms + DebugTextureView
			// renders a small quad at 0 0 0 ?
			// also rendering transporting beams now?
			else if (mesh->m_VertexFormat == 0x80001)
			{
				//ctx.modifiers.do_not_render = true;

				// also renders laser field on sp_a4_laser_platform
				if (ctx.info.shader_name.starts_with("SolidEn")) // SolidEnergy_dx9
				{
					ctx.save_vs(dev);
					dev->SetVertexShader(nullptr); 

					dev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(1)); // tc @ 12 - missing 4 bytes at the end here - fixed with tc2 size 3?
					dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);

					if (ctx.info.material_name != "effects/laserplane")
					{
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
				}


				// FIRST "UI/HUD" elem (remix injection triggers here)
				// -> fullscreen color transitions (damage etc.) and also "enables" the crosshair
				// -> takes ~ 0.8ms on a debug build
#if !defined(BENCHMARK) // do not measure when benchmarking
				else if (ctx.info.shader_name.starts_with("Engine_")) // Engine_Post
				{
					// do not fog HUD elements :D
					dev->SetRenderState(D3DRS_FOGENABLE, FALSE);

					const auto s_viewFadeColor = reinterpret_cast<Vector4D*>(CLIENT_BASE + USE_OFFSET(0x9F7748, 0x9EDAF8));

					/*const auto s_viewFadeModulate = reinterpret_cast<bool*>(CLIENT_BASE + USE_OFFSET(0x0, 0x9ECEE0));
					if (s_viewFadeModulate && *s_viewFadeModulate)
					{ }*/

					ctx.save_vs(dev);
					dev->SetVertexShader(nullptr);
					dev->SetPixelShader(nullptr); // needed

					ctx.save_texture(dev, 0);
					dev->SetTexture(0, nullptr); // disable bound texture

					ctx.save_rs(dev, D3DRS_ALPHABLENDENABLE);
					dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

					ctx.save_rs(dev, D3DRS_BLENDOP);
					dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

					ctx.save_rs(dev, D3DRS_SRCBLEND);
					dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

					ctx.save_rs(dev, D3DRS_DESTBLEND);
					dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

					ctx.save_rs(dev, D3DRS_ZWRITEENABLE);
					dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

					ctx.save_rs(dev, D3DRS_ZENABLE);
					dev->SetRenderState(D3DRS_ZENABLE, FALSE);

					dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
					dev->SetTransform(D3DTS_VIEW, &ctx.info.buffer_state.m_Transform[1]);
					dev->SetTransform(D3DTS_PROJECTION, &ctx.info.buffer_state.m_Transform[2]);

					struct CUSTOMVERTEX
					{
						float x, y, z, rhw;
						D3DCOLOR color;
					};

					auto color = D3DCOLOR_COLORVALUE(s_viewFadeColor->x, s_viewFadeColor->y, s_viewFadeColor->z, s_viewFadeColor->w);
					const auto w = (float)ctx.info.buffer_state.m_Viewport.Width + 0.5f;
					const auto h = (float)ctx.info.buffer_state.m_Viewport.Height + 0.5f;

					CUSTOMVERTEX vertices[] =
					{
						{ -0.5f, -0.5f, 0.0f, 1.0f, color }, // tl
						{     w, -0.5f, 0.0f, 1.0f, color }, // tr
						{ -0.5f,     h, 0.0f, 1.0f, color }, // bl
						{     w,     h, 0.0f, 1.0f, color }  // br
					};

					dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
					dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(CUSTOMVERTEX));

					// do not render the original mesh
					ctx.modifiers.do_not_render = true;
				}

				// Eye of Glados?
				//else if (ctx.info.material_name == "dev/fade_blur")  
				//{
				//	ctx.modifiers.do_not_render = true;

				//	ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
				//	dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(1, 0, 0, 1));
				//	dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				//	dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
				//	dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);

				//	ctx.save_tss(dev, D3DTSS_ALPHAARG1);
				//	ctx.save_tss(dev, D3DTSS_ALPHAARG2);
				//	ctx.save_tss(dev, D3DTSS_ALPHAOP);
				//	dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				//	dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
				//	dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE2X);

				//	//add_light_to_texture_color_edit(0.8f, 50.7f, 0.05f, 0.1f);
				//}
#endif
				//else 
				//{
					
					// dev/blurfilterx_nohdr
					// dev/blurfiltery_nohdr
					// dev/fade_blur
					// engine/writez
					// engine/occlusionproxy
					// dev/lumcompare
					// dev/downsample
				//}
			}

			// lasers - indicator dots - some of the white light stripes
			// portals
			// area portal windows
			// treeleaf shader
			// skybox
			else if (mesh->m_VertexFormat == 0x4a0003 || mesh->m_VertexFormat == 0x80003)
			{
				//ctx.modifiers.do_not_render = true;

				// handle coop portals
				if (ctx.info.material_name.ends_with("overlay_tinted"))
				{
					// var: $portalopenamount

					//"$PortalCoopColorPlayerOnePortalOne" "[0.125 0.500 0.824]"
					//"$PortalCoopColorPlayerOnePortalTwo" "[0.075 0.000 0.824]"
					
					//"$PortalCoopColorPlayerTwoPortalOne" "[1.000 0.705 0.125]"
					//"$PortalCoopColorPlayerTwoPortalTwo" "[0.225 0.010 0.010]"

					IMaterialVar* var_out = nullptr;
					if (has_materialvar(ctx.info.material, "$PortalColorGradientLight", &var_out)) 
					{
						if (var_out)
						{
							const auto portal_color = var_out->vftable->GetVecValueInternal1(var_out);

							bool is_p1p1 = false, is_p1p2 = false, is_p2p1 = false, is_p2p2 = false;
							is_p1p1 =  utils::float_equal(portal_color[0], 0.125f, 0.01f)
									&& utils::float_equal(portal_color[1], 0.500f, 0.01f)
									&& utils::float_equal(portal_color[2], 0.824f, 0.01f);

							if (!is_p1p1)
							{
								is_p1p2 =  utils::float_equal(portal_color[0], 0.075f, 0.01f)
										&& utils::float_equal(portal_color[1], 0.000f, 0.01f)
										&& utils::float_equal(portal_color[2], 0.824f, 0.01f);

								if (!is_p1p2)
								{
									is_p2p1 =  utils::float_equal(portal_color[0], 1.000f, 0.01f)
											&& utils::float_equal(portal_color[1], 0.705f, 0.01f)
											&& utils::float_equal(portal_color[2], 0.125f, 0.01f);

									if (!is_p2p1)
									{
										is_p2p2 =  utils::float_equal(portal_color[0], 0.225f, 0.01f)
												&& utils::float_equal(portal_color[1], 0.010f, 0.01f)
												&& utils::float_equal(portal_color[2], 0.010f, 0.01f);
									}
								}
							}

							if (is_p1p1 || is_p1p2 || is_p2p1 || is_p2p2)
							{
								handle_portal_mesh(ctx, is_p1p1 ? 0 : is_p1p2 ? 1 : is_p2p1 ? 2 : is_p2p2 ? 3 : 0, true);
								was_portal_related = true;
							}
						}
					}
				}

				// r_portal_stencil_depth 0 heavy influence
				// TODO: is this check still needed?
				if (ctx.info.material_name.contains("portal_stencil") || ctx.info.material_name.contains("portal_refract"))
				{
					was_portal_related = true; // prevent all other else's
					ctx.modifiers.do_not_render = true;
				}
				else if (ctx.info.material_name.ends_with("portalstaticoverlay_1"))
				{
					handle_portal_mesh(ctx, 0);

					//ctx.modifiers.do_not_render = true;
					was_portal_related = true;
				}
				else if (ctx.info.material_name.ends_with("portalstaticoverlay_2"))
				{
					handle_portal_mesh(ctx, 1);

					//ctx.modifiers.do_not_render = true;
					was_portal_related = true;
				}

				if (was_portal_related)
				{
					// draws portal stencil hole
					if (mesh->m_VertexFormat == 0x4a0003)
					{
						//ctx.modifiers.do_not_render = true;
						ctx.save_vs(dev);
						dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64
						dev->SetVertexShader(nullptr);
						dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
					}

					// if set to wireframe mode 
					else if (mesh->m_VertexFormat == 0x80003)
					{
						//ctx.modifiers.do_not_render = true;
						ctx.save_vs(dev);
						dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));
						dev->SetVertexShader(nullptr);
						dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);

						// dirty hack to invert the portal direction in the spawn area on sp_a4_finale2 because
						// the static overlays on portals (that we use to identify and render the rayportals) are rendered on the inside of the moving object
						if (map_settings::is_level.sp_a4_finale2 && g_player_current_area == 4)
						{
							// invert along the x axis
							ctx.info.buffer_state.m_Transform[0].m[0][0] = -1;
							dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);

							if (primlist)
							{
								IDirect3DVertexBuffer9* vb = nullptr; UINT t_stride = 0u, t_offset = 0u;
								dev->GetStreamSource(0, &vb, &t_offset, &t_stride);

								IDirect3DIndexBuffer9* ib = nullptr;
								if (SUCCEEDED(dev->GetIndices(&ib)))
								{
									WORD* ib_data; // lock index buffer to retrieve the relevant vertex indices
									if (SUCCEEDED(ib->Lock(0, 0, (void**)&ib_data, D3DLOCK_READONLY)))
									{
										// add relevant indices without duplicates
										std::unordered_set<std::uint16_t> indices; indices.reserve(primlist->m_NumIndices);

										for (auto i = 0u; i < (std::uint32_t)primlist->m_NumIndices; i++) {
											indices.insert(ib_data[primlist->m_FirstIndex + i]);
										}

										ib->Unlock();

										// get the range of vertices that we are going to work with
										UINT min_vert = 0u, max_vert = 0u;
										{
											auto [min_it, max_it] = std::minmax_element(indices.begin(), indices.end());
											min_vert = *min_it;
											max_vert = *max_it;
										}

										void* src_buffer_data; // lock vertex buffer from first used vertex (in total bytes) to X used vertices (in total bytes)
										if (SUCCEEDED(vb->Lock(min_vert * t_stride, max_vert * t_stride, &src_buffer_data, 0)))
										{
											struct src_vert {
												Vector pos; Vector normal; Vector2D tc;
											};

											for (auto i : indices)
											{
												// we need to subtract min_vert because we locked @ min_vert which is the start of our lock
												i -= static_cast<std::uint16_t>(min_vert);

												const auto v_pos_in_src_buffer = i * t_stride;
												const auto src = reinterpret_cast<src_vert*>(((DWORD)src_buffer_data + v_pos_in_src_buffer));

												// invert x coordinate as we scaled X by -1 using the world matrix
												src->pos.x = src->pos.x * -1.0f - 0.5f;

												// flip normal
												src->normal *= -1.0f;
											}
											vb->Unlock();
										}
									}
								}
							}
						}
					}
				} // end 'was_portal_related'
				else
				{
					// side beams of light bridges - effects/projected_wall_rail
					if (ctx.info.material_name.contains("ed_wall_ra"))
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
						ctx.save_texture(dev, 0);
						dev->SetTexture(0, tex_addons::portal_mask);
					}
					// unique textures for the white sky so they can be marked
					else if (ctx.info.material_name.contains("sky"))
					{
						if (ctx.info.material_name.contains("_white"))
						{
							ctx.save_texture(dev, 0);

							// sky_whiteft
							if (ctx.info.material_name.contains("eft")) {
								dev->SetTexture(0, tex_addons::sky_gray_ft);
							}
							else if (ctx.info.material_name.contains("ebk")) {
								dev->SetTexture(0, tex_addons::sky_gray_bk);
							}
							else if (ctx.info.material_name.contains("elf")) {
								dev->SetTexture(0, tex_addons::sky_gray_lf);
							}
							else if (ctx.info.material_name.contains("ert")) {
								dev->SetTexture(0, tex_addons::sky_gray_rt);
							}
							else if (ctx.info.material_name.contains("eup")) {
								dev->SetTexture(0, tex_addons::sky_gray_up);
							}
							else if (ctx.info.material_name.contains("edn")) {
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

			// engine/shadowbuild
			else if (mesh->m_VertexFormat == 0xa0003)
			{
				//ctx.modifiers.do_not_render = true; // not used anywhere else besides in puzzlemaker?
				//if (game::is_puzzlemaker_active())

				//lookat_vertex_decl(dev, primlist);
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr); // def. render using FF as the shader is causing heavy frametime drops
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE1(4)); // stride 0x30
			}

			// HUD
			// Video decals
			// rain
			else if (mesh->m_VertexFormat == 0x80007) 
			{
				//ctx.modifiers.do_not_render = true; 
				
				// always render UI and world ui with high gamma
				ctx.modifiers.with_high_gamma = true; 

				// early out if vgui_white
				if (ctx.info.material_name != "vgui_white" && ctx.info.buffer_state.m_Transform[0].m[3][0] != 0.0f)
				{
					// should be fine now that engine_post is the first hud elem
					//if (ctx.info.material_name == "vgui__fontpage")
					//{
					//	// get rid of all world-rendered text as its using the same glyph as HUD elements?!
					//	if (ctx.info.buffer_state.m_Transform[0].m[3][0] != 0.0f) {
					//		ctx.modifiers.do_not_render = true;
					//	}
					//}
					//else

					bool is_world_ui_text = ctx.info.buffer_state.m_Transform[0].m[3][0] != 0.0f && ctx.info.material_name == "vgui__fontpage";

					if (is_world_ui_text)
					{
						// set remix texture categories
						ctx.save_rs(dev, (D3DRENDERSTATETYPE)42);
						dev->SetRenderState((D3DRENDERSTATETYPE)42, WorldUI);
					}

					// vgui/screens/vgui_coop_progress_board
					if (ctx.info.material_name.ends_with("progress_board"))
					{
						//lookat_vertex_decl(dev, primlist);
						float vcol_r = 0.0f;
						float vcol_g = 0.0f;
						float vcol_b = 0.0f;

						// cant get vertex color to work here? -> grab vertex color and use tfactor instead
						{ 
							IDirect3DVertexBuffer9* vb = nullptr; UINT t_stride = 0u, t_offset = 0u;
							dev->GetStreamSource(0, &vb, &t_offset, &t_stride);

							IDirect3DIndexBuffer9* ib = nullptr;
							if (SUCCEEDED(dev->GetIndices(&ib)))
							{
								void* ib_data; // retrieve a single vertex index (*2 because WORD)
								if (SUCCEEDED(ib->Lock(primlist->m_FirstIndex * 2, 2, &ib_data, D3DLOCK_READONLY)))
								{
									const auto first_index = *static_cast<std::uint16_t*>(ib_data);
									ib->Unlock();

									void* src_buffer_data; // retrieve single indexed vertex
									if (SUCCEEDED(vb->Lock(first_index * t_stride, t_stride, &src_buffer_data, D3DLOCK_READONLY)))
									{
										struct src_vert { Vector pos; Vector normal;  D3DCOLOR color; Vector2D tc0; };
										const auto src = reinterpret_cast<src_vert*>(((DWORD)src_buffer_data));

										// unpack color
										vcol_r = static_cast<float>((src->color >> 16) & 0xFF) / 255.0f * 1.0f;
										vcol_g = static_cast<float>((src->color >> 8) & 0xFF) / 255.0f * 1.0f;
										vcol_b = static_cast<float>((src->color >> 0) & 0xFF) / 255.0f * 1.0f;
										vb->Unlock();
									}
								}
							}
						}

						ctx.save_vs(dev);
						dev->SetVertexShader(nullptr);
						dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);
						dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);

						ctx.save_tss(dev, D3DTSS_COLORARG1);
						ctx.save_tss(dev, D3DTSS_COLORARG2);
						ctx.save_tss(dev, D3DTSS_COLOROP);
						dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
						dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
						dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

						float scalar = 1.0f;
						if (vcol_r <= 0.4f) {
							scalar = std::powf(vcol_r / 0.4f, 1.5f) * 0.4f; // crush values closer to 0 as even 0.01 is bright af
						}

						// save val for screen overlays - no longer needed ig
						model_render::vgui_progress_board_scalar = std::clamp(vcol_r * scalar, 0.0f, 0.15f);
						model_render::vgui_progress_board_scalar /= 0.15f;

						ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
						dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(
							vcol_r * scalar, 
							vcol_g * scalar, 
							vcol_b * scalar, 1.0f));
					}

					else if (is_world_ui_text
						|| ctx.info.material_name.ends_with("board_numbers")
						|| ctx.info.material_name.ends_with("board_bar")
						|| ctx.info.material_name.ends_with("board_vgui"))
					{
						//lookat_vertex_decl(dev, primlist);
						float vcol_r = 0.0f;
						float vcol_g = 0.0f;
						float vcol_b = 0.0f;
						float vcol_a = 1.0f;

						// cant get vertex color to work here? -> grab vertex color and use tfactor instead
						{
							IDirect3DVertexBuffer9* vb = nullptr; UINT t_stride = 0u, t_offset = 0u;
							dev->GetStreamSource(0, &vb, &t_offset, &t_stride);

							IDirect3DIndexBuffer9* ib = nullptr;
							if (SUCCEEDED(dev->GetIndices(&ib)))
							{
								void* ib_data; // retrieve a single vertex index (*2 because WORD)
								if (SUCCEEDED(ib->Lock(primlist->m_FirstIndex * 2, 2, &ib_data, D3DLOCK_READONLY)))
								{
									const auto first_index = *static_cast<std::uint16_t*>(ib_data);
									ib->Unlock();

									void* src_buffer_data; // retrieve single indexed vertex
									if (SUCCEEDED(vb->Lock(first_index * t_stride, t_stride, &src_buffer_data, D3DLOCK_READONLY)))
									{
										struct src_vert { Vector pos; Vector normal;  D3DCOLOR color; Vector2D tc0; };
										const auto src = reinterpret_cast<src_vert*>(((DWORD)src_buffer_data));

										// unpack color
										vcol_r = static_cast<float>((src->color >> 16) & 0xFF) / 255.0f * 1.0f;
										vcol_g = static_cast<float>((src->color >> 8) & 0xFF) / 255.0f * 1.0f;
										vcol_b = static_cast<float>((src->color >> 0) & 0xFF) / 255.0f * 1.0f;
										vcol_a = static_cast<float>((src->color >> 24) & 0xFF) / 255.0f * 1.0f;
										vb->Unlock();
									}
								}
							}
						}

						ctx.save_vs(dev);
						dev->SetVertexShader(nullptr);
						dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);
						dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);

						ctx.save_tss(dev, D3DTSS_COLORARG1);
						ctx.save_tss(dev, D3DTSS_COLORARG2);
						ctx.save_tss(dev, D3DTSS_COLOROP);
						ctx.save_tss(dev, D3DTSS_ALPHAARG2);
						ctx.save_tss(dev, D3DTSS_ALPHAOP);

						dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
						dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
						dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
						dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
						dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

						float scalar = vcol_a;
						if (vcol_a <= 0.5f) {
							scalar = std::powf(vcol_a / 0.5f, 2.0f) * 0.5f; // crush values closer to 0
						}

						ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
						dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(
							vcol_r* scalar,
							vcol_g* scalar,
							vcol_b* scalar, scalar));
					}

					else if (ctx.info.material_name.contains("elevator_video_")) 
					{
						//ctx.modifiers.do_not_render = true;
						ctx.save_vs(dev);
						dev->SetVertexShader(nullptr);
						dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
						//dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1); // no need to set fvf here
					}

					// fix rain by making it slightly emissive
					else if (ctx.info.material_name.ends_with("/rain"))
					{
						ctx.modifiers.with_high_gamma = false;
						ctx.save_rs(dev, D3DRS_DESTBLEND);
						dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

						ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
						ctx.save_tss(dev, D3DTSS_COLOROP);
						ctx.save_tss(dev, D3DTSS_COLORARG2);

						dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(100, 100, 100, 255));
						dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
						dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
					}

					// some light sprites are rendered as ui through other geo 
					else if (ctx.info.material_name.ends_with("_noz")) {
						ctx.modifiers.do_not_render = true;
					}

					// fix particles on intro1 after breaking the wall
					else if (ctx.info.material_name.starts_with("particle/"))
					{
						//lookat_vertex_decl(dev, primlist);
						ctx.save_vs(dev);
						dev->SetVertexShader(nullptr);
						dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);
						dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]); 
						dev->SetTransform(D3DTS_VIEW, &ctx.info.buffer_state.m_Transform[1]);
						dev->SetTransform(D3DTS_PROJECTION, &ctx.info.buffer_state.m_Transform[2]);

						ctx.save_tss(dev, D3DTSS_COLOROP);
						ctx.save_tss(dev, D3DTSS_COLORARG2);
						ctx.save_tss(dev, D3DTSS_ALPHAOP);
						ctx.save_tss(dev, D3DTSS_ALPHAARG2);

						dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
						dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
						dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
						dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE2X);
					}
				}
			}

			// on portal open - spark fx (center)
			// + portal clearing gate (blue sweeping beam)
			// + portal gun pickup effect (beam)
			// + laser emitter swirrl
			// + portal cleanser
			// can be rendered but requires vertexshader + position
#if 1
			else if (mesh->m_VertexFormat == 0x924900005) // stride 0x70 - 112 
			{
				// Spritecard -> Splinecard

				bool is_spline = false; 
				IMaterialVar* splinetype = nullptr;
				if (has_materialvar(ctx.info.material, "$splinetype", &splinetype) // could fix other splines as well but performance is not great esp. when shooting portals
					&& splinetype && splinetype->m_intVal > 0)
				{
					is_spline = true;
				}

				bool add_self = 0;
				float fAdditiveSelfBlendWeight = 1.0f;
				if (has_materialvar(ctx.info.material, "$ADDSELF", &splinetype)
					&& splinetype && splinetype->m_intVal > 0)
				{
					add_self = splinetype->m_intVal;

					float v[4] = {}; dev->GetPixelShaderConstantF(0, v, 1);
					fAdditiveSelfBlendWeight = v[2];
				}

#ifndef SPRITE_TRAIL_TEST
				const bool is_cleanser = ctx.info.material_name == "effects/portal_cleanser"
									  || ctx.info.material_name == "effects/cleanser_edge";
#endif

				// FF sparks look really good but are heavy on the framerate + it messes up the trail shot by the portal gun ....
				const bool is_spark = ctx.info.material_name.starts_with("particle/sparks/"); //|| ctx.info.material_name == "particle/particle_glow_02_additive_trail";

				if (is_spark || ctx.info.material_name.ends_with("additive_trail")) {
					set_remix_emissive_intensity(dev, ctx, 2.0f);
				}

#ifdef SPRITE_TRAIL_TEST
				if (is_spline)
				{
#else
				if (is_spline && !is_spark &&
					(is_cleanser
						//|| is_spark
						|| map_settings::is_level.sp_a1_wakeup
						|| ctx.info.buffer_state.m_Transform[2].m[3][2] > -2.0f // portal gun pickup beams (the two materials in the next check)
						|| (g_player_current_area == 2 && map_settings::is_level.sp_a1_intro3) // allow in this area
						|| (g_player_current_area == 5 && map_settings::is_level.sp_a2_intro) // allow in this area
						)
					)
				{
#endif
#ifndef SPRITE_TRAIL_TEST
					if (is_cleanser
						//|| is_spark
						|| map_settings::is_level.sp_a1_wakeup
						|| ctx.info.material_name.starts_with("particle/beam_generic")
						|| ctx.info.material_name.ends_with("electricity_beam_01"))
#endif
					{
						ctx.save_vs(dev);
						dev->SetVertexShader(nullptr);
						//dev->SetPixelShader(nullptr);
						dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX8);

#ifdef SPRITE_TRAIL_TEST
						if (is_spark && !add_self)
						{
							//ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
							//dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(0.25f, 0.25f, 0.25f, 1.0f));

							ctx.save_tss(dev, D3DTSS_COLORARG1);
							ctx.save_tss(dev, D3DTSS_COLORARG2);
							ctx.save_tss(dev, D3DTSS_COLOROP);
							dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
							dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
							dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

							//ctx.save_tss(dev, D3DTSS_ALPHAARG1);
							//ctx.save_tss(dev, D3DTSS_ALPHAOP);
							//dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
							//dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

							ctx.save_rs(dev, D3DRS_SRCBLEND);
							ctx.save_rs(dev, D3DRS_DESTBLEND);
							dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
							//dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
							dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTALPHA); // no alpha but lights up the scene
						}
#endif
						
						// if overbright
						if (add_self)
						{
							//const auto v_color = model_render_hlslpp::get_sprite_trail_particle_color(primlist);

							Vector4D v_color = {};
							{
								IDirect3DVertexBuffer9* vb = nullptr; UINT t_stride = 0u, t_offset = 0u;
								dev->GetStreamSource(0, &vb, &t_offset, &t_stride);

								IDirect3DIndexBuffer9* ib = nullptr;
								if (SUCCEEDED(dev->GetIndices(&ib)))
								{
									void* ib_data; // retrieve a single vertex index (*2 because WORD)
									if (SUCCEEDED(ib->Lock(primlist->m_FirstIndex * 2, 2, &ib_data, D3DLOCK_READONLY)))
									{
										const auto first_index = *static_cast<std::uint16_t*>(ib_data);
										ib->Unlock();

										void* src_buffer_data; // retrieve single indexed vertex
										if (SUCCEEDED(vb->Lock(first_index * t_stride, t_stride, &src_buffer_data, D3DLOCK_READONLY)))
										{
											struct src_vert {
												Vector vParms; D3DCOLOR vTint;
											};

											const auto src = reinterpret_cast<src_vert*>(((DWORD)src_buffer_data));

											// unpack color
											v_color.x = static_cast<float>((src->vTint >> 16) & 0xFF) / 255.0f * 1.0f;
											v_color.y = static_cast<float>((src->vTint >> 8) & 0xFF) / 255.0f * 1.0f;
											v_color.z = static_cast<float>((src->vTint >> 0) & 0xFF) / 255.0f * 1.0f;
											v_color.w = static_cast<float>((src->vTint >> 24) & 0xFF) / 255.0f * 1.0f;
											vb->Unlock();
										}
									}
								}
							}

							ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
							dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(fAdditiveSelfBlendWeight - (1.0f - v_color.x), fAdditiveSelfBlendWeight - (1.0f - v_color.y), fAdditiveSelfBlendWeight - (1.0f - v_color.z), v_color.w));

							ctx.save_tss(dev, D3DTSS_COLORARG1);
							ctx.save_tss(dev, D3DTSS_COLORARG2);
							ctx.save_tss(dev, D3DTSS_COLOROP);
							dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); 
							dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
							dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);

							ctx.save_tss(dev, D3DTSS_ALPHAARG1);
							ctx.save_tss(dev, D3DTSS_ALPHAARG2);
							ctx.save_tss(dev, D3DTSS_ALPHAOP);
							dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
							dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
							dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE2X);

							//ctx.save_rs(dev, D3DRS_SRCBLEND);
							//ctx.save_rs(dev, D3DRS_DESTBLEND);
							//dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
							//dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA); // no alpha but lights up the scene
						}

						// fix out of bounds point (dirty)
						ctx.modifiers.as_portalgun_pickup_beam = true;

						// portal gun pickup beams
						if (ctx.info.buffer_state.m_Transform[2].m[3][2] > -2.0f) 
						{
							// modify light of add-light-to-texture light
							add_light_to_texture_color_edit(0.4f, 0.85f, 0.55f, 0.001f);
						}
					}
				}
				else if (is_spark)
				{
					ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
					dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(0.25f, 0.25f, 0.25f, 1.0f)); // v_color.w

					ctx.save_tss(dev, D3DTSS_COLORARG1);
					ctx.save_tss(dev, D3DTSS_COLORARG2);
					ctx.save_tss(dev, D3DTSS_COLOROP);
					dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
					dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
					dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

					ctx.save_tss(dev, D3DTSS_ALPHAARG1);
					ctx.save_tss(dev, D3DTSS_ALPHAOP);
					dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
					dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

					ctx.save_rs(dev, D3DRS_SRCBLEND);
					ctx.save_rs(dev, D3DRS_DESTBLEND);
					dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
					dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA); // no alpha but lights up the scene
				}

				//ctx.modifiers.do_not_render = true;
				dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
				dev->SetTransform(D3DTS_VIEW, &ctx.info.buffer_state.m_Transform[1]); 
				dev->SetTransform(D3DTS_PROJECTION, &ctx.info.buffer_state.m_Transform[2]);
			}
#endif

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

				if (ctx.info.shader_name == "WorldVertexTransition_DX9") {
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

				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE1(2)); // tc @ 28

				// not doing this and picking up a skinned model (eg. cube) will break displacement rendering???
				dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
			}

			// hanging cables - requires vertex shader - verts not modified on the cpu
			else if (mesh->m_VertexFormat == 0x24900005)
			{
				//ctx.modifiers.do_not_render = true; // they can freak out sometimes so just ignore them for now
				ctx.save_texture(dev, 0);
				dev->SetTexture(0, tex_addons::black_shader);
			}

			// SpriteCard shader
			// on portal open
			// portal gun pickup effect (pusling lights (not the beam))
			// other particles like smoke - wakeup scene ring - water splash
#if 1
			else if (mesh->m_VertexFormat == 0x114900005) // stride 96 
			{
				//const auto is_spritecard = ctx.info.material->vftable->IsSpriteCard(ctx.info.material);
				//ctx.modifiers.do_not_render = true;

				// scale the projection matrix for viewmodel particles so that they match the scaled remix viewmodel (currently set to a scale of 0.4)
				if (ctx.info.buffer_state.m_Transform[2].m[3][2] == -1.00003529f) 
				{
					// #TODO - remove when floating point perc. gets better with shaders
					//if (map_settings::get_map_name() == "sp_a1_wakeup")  
					{
						if (ctx.info.material_name == "particle/particle_glow_05_add_15ob" || ctx.info.material_name == "particle/electrical_arc/electrical_arc")   {
							ctx.modifiers.do_not_render = true;
						}
					}

#if 0				// Disabled as we do not show the portal gun viewmodel effects
					// - maybe needed when shaders are no longer affected by floating point imprecision
					//ctx.modifiers.do_not_render = true;
					D3DXMATRIX scaleMatrix = game::IDENTITY;
					scaleMatrix.m[0][0] = scaleMatrix.m[1][1] = scaleMatrix.m[2][2] = 2.5f;
					scaleMatrix.m[3][3] = 1.0;

					ctx.info.buffer_state.m_Transform[2] = scaleMatrix * ctx.info.buffer_state.m_Transform[2];
#endif
				}

				bool disable_vertex_color_modulation = false;

				// get flashlight end pos (wheatly) on bts3
				if (map_settings::is_level.sp_a2_bts3)
				{
					if (ctx.info.material_name == "particle/flashlight_glow")
					{
						Vector flashlight_pos = {};

						IDirect3DVertexBuffer9* vb = nullptr; UINT t_stride = 0u, t_offset = 0u;
						dev->GetStreamSource(0, &vb, &t_offset, &t_stride);

						IDirect3DIndexBuffer9* ib = nullptr;
						if (SUCCEEDED(dev->GetIndices(&ib)))
						{
							void* ib_data; // lock index buffer to retrieve the relevant vertex indices
							if (SUCCEEDED(ib->Lock(0, 0, &ib_data, D3DLOCK_READONLY)))
							{
								// add relevant indices without duplicates
								std::unordered_set<std::uint16_t> indices; indices.reserve(primlist->m_NumIndices);

								for (auto i = 0u; i < (std::uint32_t)primlist->m_NumIndices; i++) {
									indices.insert(static_cast<std::uint16_t*>(ib_data)[primlist->m_FirstIndex + i]);
								}

								ib->Unlock();

								// get the range of vertices that we are going to work with
								UINT min_vert = 0u, max_vert = 0u;
								{
									auto [min_it, max_it] = std::minmax_element(indices.begin(), indices.end());
									min_vert = *min_it;
									max_vert = *max_it;
								}

								void* src_buffer_data;

								// lock vertex buffer from first used vertex (in total bytes) to X used vertices (in total bytes)
								if (SUCCEEDED(vb->Lock(min_vert * t_stride, max_vert * t_stride, &src_buffer_data, D3DLOCK_READONLY)))
								{
									struct src_vert {
										Vector pos;
									};

									for (auto i : indices)
									{
										// we need to subtract min_vert because we locked @ min_vert which is the start of our lock
										i -= static_cast<std::uint16_t>(min_vert);

										const auto v_pos_in_src_buffer = i * t_stride;
										const auto src = reinterpret_cast<src_vert*>(((DWORD)src_buffer_data + v_pos_in_src_buffer));

										flashlight_pos += src->pos;
									}

									flashlight_pos /= static_cast<float>(indices.size());
									vb->Unlock();
								}
							}
						}
						api::remix_lights::bts3_set_flashlight_end_pos(flashlight_pos);
					}
				}
				else if (map_settings::is_level.sp_a4_finale4)
				{
					DWORD dstblend;
					dev->GetRenderState(D3DRS_DESTBLEND, &dstblend);

					// fix emissive smoke near end
					if (dstblend == D3DBLEND_ONE)
					{
						if (ctx.info.material_name == "particle/smoke1/smoke1_additive")
						{
							//ctx.save_rs(dev, D3DRS_DESTBLEND);
							//dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); // disables emissive smoke

							ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
							ctx.save_tss(dev, D3DTSS_COLOROP);
							ctx.save_tss(dev, D3DTSS_COLORARG2);
							ctx.save_tss(dev, D3DTSS_ALPHAOP);
							ctx.save_tss(dev, D3DTSS_ALPHAARG2);
							dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(30, 30, 30, 255));
							dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
							dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
							dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
							dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
							disable_vertex_color_modulation = true;
						}
						/*else if (ctx.info.material_name == "particle/star_noz")
						{
							ctx.save_rs(dev, D3DRS_DESTBLEND);
							dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

							ctx.save_tss(dev, D3DTSS_COLOROP);
							ctx.save_tss(dev, D3DTSS_COLORARG2);
							ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
							dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(1, 1, 1, 1.0f));
							dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
							dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
							disable_vertex_color_modulation = true;
						}*/
					}
					/*else if (dstblend == D3DBLEND_INVSRCALPHA && ctx.info.material_name.ends_with("spray1_addself"))
					{
						ctx.save_rs(dev, D3DRS_DESTBLEND);
						dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
					}*/
				}

				if (!disable_vertex_color_modulation) 
				{
					ctx.save_tss(dev, D3DTSS_COLOROP);
					ctx.save_tss(dev, D3DTSS_COLORARG2);
					dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
					dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

					ctx.save_tss(dev, D3DTSS_ALPHAOP);
					ctx.save_tss(dev, D3DTSS_ALPHAARG2);
					dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE2X);
					dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

					// testing
					//ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
					//dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(1, 1, 1, 0.25f));
					//dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
				}

				dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);  
				dev->SetTransform(D3DTS_VIEW, &ctx.info.buffer_state.m_Transform[1]);
				dev->SetTransform(D3DTS_PROJECTION, &ctx.info.buffer_state.m_Transform[2]);
			}
#endif
			// on portal open - blob in the middle (impact)
			// water drops on glados wakeup
			else if (mesh->m_VertexFormat == 0x80037) // TODO - test with buffer_state transforms  
			{
//#ifdef DEBUG
				//ctx.modifiers.do_not_render = true; // this needs a position as it spawns on 0 0 0 // stride 0x40
				//int break_me = 0;

				if (ctx.info.material_name.ends_with("warp_alpha"))
				{
					ctx.save_texture(dev, 0);
					dev->SetTexture(0, tex_addons::water_drip);
					dev->SetTexture(1, tex_addons::water_drip);
					
					ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
					dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(0.2f, 0.2f, 0.2f, 1.0f)); // v_color.w

					ctx.save_tss(dev, D3DTSS_COLORARG1);
					ctx.save_tss(dev, D3DTSS_COLORARG2);
					ctx.save_tss(dev, D3DTSS_COLOROP);
					dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
					dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
					dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);

					ctx.save_tss(dev, D3DTSS_ALPHAARG1);
					ctx.save_tss(dev, D3DTSS_ALPHAOP);
					dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
					dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE2X);
				}

				/*if (ctx.info.shader_name != "Wireframe_DX9") {
					ctx.info.material->vftable->SetShader(ctx.info.material, "Wireframe");
				}*/

				//ctx.save_vs(dev);
				//dev->SetVertexShader(nullptr);
				//dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX5);
//#endif
			}

			// on portal open - outer ring effect
			// transport beam emitter effect
			else if (mesh->m_VertexFormat == 0x1b924900005) // stride 0x90 - 144
			{
				//ctx.modifiers.do_not_render = true;
				//int break_me = 0;
				//lookat_vertex_decl(dev, primlist);

#ifdef SPRITE_TRAIL_TEST
				{
#else
				if (map_settings::is_level.sp_a4_finale4)
				{
					model_render_hlslpp::fix_sprite_trail_particles(ctx, primlist);
#endif
					ctx.save_vs(dev); 
					dev->SetVertexShader(nullptr);
					dev->SetPixelShader(nullptr);
					dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX8); // fill up to stride? - 112

					dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
					dev->SetTransform(D3DTS_VIEW, &ctx.info.buffer_state.m_Transform[1]);
					dev->SetTransform(D3DTS_PROJECTION, &ctx.info.buffer_state.m_Transform[2]);

					// fix out of bounds point (dirty)
					ctx.modifiers.as_portalgun_pickup_beam = true;
				}
			}

			// emancipation grill
			// renders water $bottommaterial
			else if (mesh->m_VertexFormat == 0x80033) //stride = 0x40   
			{
				/*
				float flPowerUp = params[ info.m_nPowerUp ]->GetFloatValue();
				float flIntensity = params[info.m_nFlowColorIntensity]->GetFloatValue();

				bool bActive = (flIntensity > 0.0f);
				if ((bHasFlowmap) && (flPowerUp <= 0.0f))
				{
					bActive = false;
				}*/

				//ctx.modifiers.do_not_render = true;

				//lookat_vertex_decl(dev, primlist);

				if (ctx.info.material_name.starts_with("nature/"))
				{
					ctx.save_vs(dev);
					dev->SetVertexShader(nullptr);
					dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64 :: tc @ 24
					dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
				}
				else
				{
					render_emancipation_grill(ctx);
				}
			}

			// decals
			// moon surface
			else if (mesh->m_VertexFormat == 0x2480037)  // stride 0x50 - 80 
			{
				//ctx.modifiers.do_not_render = true;
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr); 
				//dev->SetFVF(D3DFVF_XYZB4 | D3DFVF_TEX7 | D3DFVF_TEXCOORDSIZE1(4)); // 84 - 4 as last tc is one float :: tc at 28
				//lookat_vertex_decl(dev, primlist);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX7 | D3DFVF_TEXCOORDSIZE1(4));
				dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
			}

			// Sprite shader
			else if (mesh->m_VertexFormat == 0x914900005)
			{
				//ctx.modifiers.do_not_render = true;

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
				// maybe edit vert texcoords directly within Client :: C_OP_RenderSprites::Render

				float g_vCropFactor[4] = {};
				dev->GetVertexShaderConstantF(15, g_vCropFactor, 1);
				bool use_crop = false; //g_vCropFactor[0] != 1.0f || g_vCropFactor[1] != 1.0f; // not save

				IMaterialVar* var_out = nullptr;
				if (has_materialvar(ctx.info.material, "$CROPFACTOR", &var_out))
				{
					if (var_out) {
						use_crop = var_out->vftable->GetVecValueInternal1(var_out)[0] != 1.0f || var_out->vftable->GetVecValueInternal1(var_out)[1] != 1.0f;
					}
				}

				bool use_dualsequence = false;
				if (has_materialvar(ctx.info.material, "$DUALSEQUENCE", &var_out))
				{
					if (var_out) {
						use_dualsequence = var_out->vftable->GetIntValueInternal(var_out) != 0;
					}
				}

				/*bool bZoomSeq2 = false;
				if (has_materialvar(ctx.info.material, "$ZOOMANIMATESEQ2", &var_out))
				{
					if (var_out) {
						bZoomSeq2 = var_out->vftable->GetFloatValueInternal(var_out) > 1.0f;
					}
				}*/

				ctx.save_tss(dev, D3DTSS_ALPHAARG2);
				ctx.save_tss(dev, D3DTSS_ALPHAOP);
				dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
				dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

				bool modulate_alpha = false;
				{
					DWORD dest_blend;
					dev->GetRenderState(D3DRS_DESTBLEND, &dest_blend);

					if ((D3DBLEND)dest_blend == D3DBLEND_ONE) {
						modulate_alpha = true;
					}
				}

				// ---

				float scale_parms[4] = {};
				dev->GetVertexShaderConstantF(55, scale_parms, 1);
#if 0
				const float OLDFRM_SCALE_START = scale_parms[0];
				const float OLDFRM_SCALE_END = scale_parms[1];

				auto getlerpscaled = [](float l_in, float s0, float s1, float ts)
					{
						l_in = 2.0f * (l_in - 0.5f);
						l_in *= std::lerp(s0, s1, ts);
						return 0.5f + 0.5f * l_in;
					};
#endif

#if 1
				IDirect3DVertexBuffer9* vb = nullptr; UINT t_stride = 0u, t_offset = 0u; 
				if (SUCCEEDED(dev->GetStreamSource(0, &vb, &t_offset, &t_stride)))
				{
					IDirect3DIndexBuffer9* ib = nullptr;
					if (SUCCEEDED(dev->GetIndices(&ib)))
					{
						void* ib_data; // lock index buffer to retrieve the relevant vertex indices
						if (SUCCEEDED(ib->Lock(0, 0, &ib_data, D3DLOCK_READONLY)))
						{
							// add relevant indices without duplicates
							std::unordered_set<std::uint16_t> indices; indices.reserve(primlist->m_NumIndices);
							for (auto i = 0u; i < (std::uint32_t)primlist->m_NumIndices; i++)
							{
								indices.insert(static_cast<std::uint16_t*>(ib_data)[primlist->m_FirstIndex + i]);
							}
							ib->Unlock();

							// get the range of vertices that we are going to work with
							UINT min_vert = 0u, max_vert = 0u;
							{
								auto [min_it, max_it] = std::minmax_element(indices.begin(), indices.end());
								min_vert = *min_it; max_vert = *max_it;
							}

							void* src_buffer_data;

							// lock vertex buffer from first used vertex (in total bytes) to X used vertices (in total bytes)
							if (SUCCEEDED(vb->Lock(min_vert * t_stride, max_vert * t_stride, &src_buffer_data, 0)))
							{
								struct src_vert
								{
									Vector pos; D3DCOLOR color;
									Vector4D tc0; Vector4D tc1; Vector4D tc2; Vector2D tc3;
									Vector4D tc4; Vector4D tc5; Vector4D tc6; Vector4D tc7;
								};

								for (auto i : indices)
								{
									// we need to subtract min_vert because we locked @ min_vert which is the start of our lock
									i -= static_cast<std::uint16_t>(min_vert);

									const auto v_pos_in_src_buffer = i * t_stride;
									const auto src = reinterpret_cast<src_vert*>(((DWORD)src_buffer_data + v_pos_in_src_buffer));

									// float2 vCornerID : TEXCOORD3;
									// o.texCoord0_1.xy = lerp( v.vTexCoord0.zw, v.vTexCoord0.xy, v.vCornerID.xy );
									// o.texCoord0_1.wz = lerp( v.vTexCoord1.zw, v.vTexCoord1.xy, v.vCornerID.xy );

									//src->tc0.x = std::lerp(src->tc0.z, src->tc0.x, src->tc3.x /** g_vCropFactor[0] + g_vCropFactor[2]*/);
									//src->tc0.y = std::lerp(src->tc0.w, src->tc0.y, src->tc3.y /** g_vCropFactor[1] + g_vCropFactor[3]*/);

									/*
										float2 vCornerID        : TEXCOORD3;   // 0,0 1,0 1,1 0,1
										float4 vTexCoord2		: TEXCOORD4;
										#if DUALSEQUENCE
										float4 vSeq2TexCoord0   : TEXCOORD5;
										float4 vSeq2TexCoord1   : TEXCOORD6; 
										float4 vParms1          : TEXCOORD7;  // second frame blend, ?,?,?

										float2 lerpold = v.vCornerID.xy;
										float2 lerpnew = v.vCornerID.xy;

										#if ( ZOOM_ANIMATE_SEQ2 )
										{
											lerpold.x = getlerpscale_for_old_frame( v.vCornerID.x, v.vParms1.x );
											lerpold.y = getlerpscale_for_old_frame( v.vCornerID.y, v.vParms1.x );
											lerpnew.x = getlerpscale_for_new_frame( v.vCornerID.x, v.vParms1.x );
											lerpnew.y = getlerpscale_for_new_frame( v.vCornerID.y, v.vParms1.x );
										}
										#endif

										o.vSeq2TexCoord0_1.xy	= lerp( v.vSeq2TexCoord0.zw, v.vSeq2TexCoord0.xy, lerpold.xy );
										o.vSeq2TexCoord0_1.wz	= lerp( v.vSeq2TexCoord1.zw, v.vSeq2TexCoord1.xy, lerpnew.xy );
									 */

									if (use_crop)
									{
										src->tc0.x = std::lerp(src->tc0.z, src->tc0.x, src->tc3.x * g_vCropFactor[0] + g_vCropFactor[2]);
										src->tc0.y = std::lerp(src->tc0.w, src->tc0.y, src->tc3.y * g_vCropFactor[1] + g_vCropFactor[3]);
									}
									else
									{
										src->tc0.x = std::lerp(src->tc0.z, src->tc0.x, src->tc3.x);
										src->tc0.y = std::lerp(src->tc0.w, src->tc0.y, src->tc3.y);
									}

									if (use_dualsequence)
									{
#if 0
										Vector2D lerpold = { src->tc3.x, src->tc3.y };
										Vector2D lerpnew = { src->tc3.x, src->tc3.y };

										if (bZoomSeq2)
										{
											lerpold.x = getlerpscaled(src->tc3.x, OLDFRM_SCALE_START, OLDFRM_SCALE_END, src->tc7.x);
											lerpold.y = getlerpscaled(src->tc3.y, OLDFRM_SCALE_START, OLDFRM_SCALE_END, src->tc7.x);
											lerpnew.x = getlerpscaled(src->tc3.x, 1.0f, OLDFRM_SCALE_START, src->tc7.x);
											lerpnew.y = getlerpscaled(src->tc3.y, 1.0f, OLDFRM_SCALE_START, src->tc7.x);
										}

										// src->tc7.x = blendfactor between tc5 lerpold and tc6 lerpnew
										if (src->tc7.x < 0.5f)
										{
											src->tc0.x = std::lerp(src->tc5.z, src->tc5.x, lerpold.x);
											src->tc0.y = std::lerp(src->tc5.w, src->tc5.y, lerpold.y);
										}
										else
										{
											src->tc0.x = std::lerp(src->tc6.z, src->tc6.x, lerpnew.x);
											src->tc0.y = std::lerp(src->tc6.w, src->tc6.y, lerpnew.y);
										}
#endif

										if (src->tc7.x < 1.0f)
										{
											src->tc0.x = std::lerp(src->tc5.z, src->tc5.x, src->tc3.x);
											src->tc0.y = std::lerp(src->tc5.w, src->tc5.y, src->tc3.y);
										}
										else
										{
											src->tc0.x = std::lerp(src->tc6.z, src->tc6.x, src->tc3.x);
											src->tc0.y = std::lerp(src->tc6.w, src->tc6.y, src->tc3.y);
										}
									}
									/*else
									{
										int x = 1;
									}*/

									//src->tc0.x = std::lerp(src->tc5.z, src->tc5.x, src->tc3.x);
									//src->tc0.y = std::lerp(src->tc5.w, src->tc5.y, src->tc3.y);

									// 2nd tex
									//src->tc0.x = std::lerp(src->tc6.z, src->tc6.x, src->tc3.x); 
									//src->tc0.y = std::lerp(src->tc6.w, src->tc6.y, src->tc3.y);

									if (modulate_alpha)
									{
										//float r = static_cast<float>((src->color >> 16) & 0xFF) / 255.0f * 1.0f;
										//float g = static_cast<float>((src->color >> 8) & 0xFF) / 255.0f * 1.0f;
										//float b = static_cast<float>((src->color >> 0) & 0xFF) / 255.0f * 1.0f;
										float a = static_cast<float>((src->color >> 24) & 0xFF) / 255.0f * 0.05f;
										//src->color = D3DCOLOR_COLORVALUE(r, g, b, a);
										src->color = (src->color & 0x00FFFFFF) | (static_cast<unsigned char>(a * 255.0f) << 24);
									}
								}

								vb->Unlock();
							}
						}
					}
				}
#endif
				/*ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2);
				dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
				dev->SetTransform(D3DTS_VIEW, &ctx.info.buffer_state.m_Transform[1]);
				dev->SetTransform(D3DTS_PROJECTION, &ctx.info.buffer_state.m_Transform[2]);*/
			}

			// paint blobs (blob only, not the painted surfs)
			// maybe use r_paintblob_wireframe to force wireframe mode later
			else if (mesh->m_VertexFormat == 0x100003)
			{
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3);
				dev->SetTransform(D3DTS_WORLD, &ctx.info.buffer_state.m_Transform[0]);
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

#if defined(BENCHMARK)
		if (model_render::m_benchmark.enabled)
		{
			if (bench.now(&model_render::m_benchmark.ms))
			{
				model_render::m_benchmark.material_name = ctx.info.material_name;
				model_render::m_benchmark.vertex_format = mesh ? mesh->m_VertexFormat : 0xDEADBEEF;
			}

			model_render::m_benchmark.ms_total += bench.get_ms();

			{
				float now = 0.0f;
				bench.now(&now);

				const auto con = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(con, BACKGROUND_BLUE);

				printf("[ %.3f ms ]\t vertex format [ 0x%llx ] using material [ %s ]\n",
					now, mesh->m_VertexFormat, std::string(ctx.info.material_name).c_str());
			}
		}
#endif
	}

	HOOK_RETN_PLACE_DEF(cmeshdx8_renderpass_pre_draw_retn_addr);
	void __declspec(naked) cmeshdx8_renderpass_pre_draw_stub()
	{
		__asm
		{
			pushad;
			push	ebx; // CPrimList
			push	esi; // CMeshDX8
			call	cmeshdx8_renderpass_pre_draw;
			add		esp, 8;
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
		auto& ctx = model_render::primctx;

		// 0 = Gamma 1.0 (fixes dark albedo) :: 1 = Gamma 2.2
		dev->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, ctx.modifiers.with_high_gamma ? 1u : 0u);

		// do not render next surface if set
		if (!ctx.modifiers.do_not_render)
		{
			// dirty hack to fix 1 oob point
			if (ctx.modifiers.as_portalgun_pickup_beam) {
				prim_count -= 1;
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

		if (ctx.modifiers.as_emancipation_grill)
		{
			D3DXMATRIX current_transform = {};
			dev->GetTransform(D3DTS_TEXTURE0, &current_transform);

			D3DXMATRIX scale_matrix;
			D3DXMatrixScaling(&scale_matrix, ctx.modifiers.emancipation_scale.x, ctx.modifiers.emancipation_scale.y, 1.0f);

			current_transform *= scale_matrix;
			current_transform(3, 0) = ctx.modifiers.emancipation_offset.x;
			current_transform(3, 1) = ctx.modifiers.emancipation_offset.y;

			dev->SetTransform(D3DTS_TEXTURE0, &current_transform);

			ctx.save_texture(dev, 0); 
			dev->SetTexture(0, tex_addons::emancipation_grill_bg);

			const auto& cs = ctx.modifiers.emancipation_color_scale;
			dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(0.2f * cs, 0.4f * cs, 0.52f * cs, 0.3f * cs));
			dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

			// draw surface a second time
			dev->DrawIndexedPrimitive(type, base_vert_index, min_vert_index, num_verts, start_index, prim_count);
		}

		if (ctx.modifiers.dual_render_with_specified_texture)
		{
			// save og texture
			IDirect3DBaseTexture9* og_tex0 = nullptr;
			dev->GetTexture(0, &og_tex0);

			// set new texture
			dev->SetTexture(0, ctx.modifiers.dual_render_texture);

			// BLEND ADD mode
			if (ctx.modifiers.dual_render_with_specified_texture_blend_add)
			{
				ctx.save_rs(dev, D3DRS_ALPHABLENDENABLE);
				dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

				ctx.save_rs(dev, D3DRS_BLENDOP);
				dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

				ctx.save_rs(dev, D3DRS_SRCBLEND);
				dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);

				ctx.save_rs(dev, D3DRS_DESTBLEND);
				dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

				ctx.save_rs(dev, D3DRS_ZWRITEENABLE);
				dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

				ctx.save_rs(dev, D3DRS_ZENABLE);
				dev->SetRenderState(D3DRS_ZENABLE, FALSE);

				// set remix texture categories
				ctx.save_rs(dev, (D3DRENDERSTATETYPE)42);
				dev->SetRenderState((D3DRENDERSTATETYPE)42, WorldMatte | IgnoreOpacityMicromap);
			}

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

		add_light_to_texture_color_restore();

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

	void cmeshdx8_renderpassforinstances_post_draw()
	{
		const auto dev = game::get_d3d_device();

		// restore shader if we set it to null right before drawing in the func above
		if (ff_brushmodels::s_shader)
		{
			dev->SetVertexShader(ff_brushmodels::s_shader);
			dev->SetFVF(NULL);
			ff_brushmodels::s_shader = nullptr;
		}
	}

	// -----

	void prop_portal_client_think_hk(C_Prop_Portal* portal)
	{
		// portal pointers and settings are reset at the end of a frame (main_module::endscene_cb)
		// resetting them here messes up the link to portal 2
		// resetting them at the start of a frame does not work because client think is called after rendering the portals?

		auto& p = model_render::game_portals;

		if (portal && portal->m_bActivated)
		{
			// portal 0 or 2
			if (!portal->m_bIsPortal2) 
			{
#/*ifdef DEBUG
				if (portal->m_pLinkedPortal) {
					int break_me = 1;
				}

				if (portal->m_bIsMobile) {
					int break_me = 1;
				}
#endif
				if (portal->portal_index != 0 && portal->portal_index != 2 || portal->portal_index > 3)
				{
					__debugbreak();
				}

				p[portal->portal_index].portal = portal;
				p[portal->portal_index].portal_owner = portal->m_hFiredByPlayer;
				p[portal->portal_index].index = portal->portal_index;
				p[portal->portal_index].open_amount = portal->m_fOpenAmount;
				p[portal->portal_index].is_linked = portal->m_pLinkedPortal ? true : false;*/

				/*model_render::portal1_is_linked = portal->m_pLinkedPortal ? true : false;
				model_render::portal1_open_amount = portal->m_fOpenAmount;
				model_render::portal1_ptr = portal;*/

				// first pair (can be player 1 or 2)
				if (!portal->team_num  || portal->team_num == 3)
				{
					p[0].portal = portal;
					p[0].portal_owner = portal->m_hFiredByPlayer;
					p[0].open_amount = portal->m_fOpenAmount;
					p[0].is_linked = portal->m_pLinkedPortal ? true : false;
				}

				// second pair
				else // if second portal of pair is matching owning player
				{
					p[2].portal = portal;
					p[2].portal_owner = portal->m_hFiredByPlayer;
					p[2].open_amount = portal->m_fOpenAmount;
					p[2].is_linked = portal->m_pLinkedPortal ? true : false;
				}
			}
			else
			{
/*#ifdef DEBUG
				if (portal->m_pLinkedPortal) {
					int break_me = 1;
				}

				if (portal->m_bIsMobile) {
					int break_me = 1;
				}
#endif
				if (portal->portal_index != 1 && portal->portal_index != 3 || portal->portal_index > 3)
				{
					__debugbreak();
				}

				p[portal->portal_index].portal = portal;
				p[portal->portal_index].portal_owner = portal->m_hFiredByPlayer;
				p[portal->portal_index].index = portal->portal_index;
				p[portal->portal_index].open_amount = portal->m_fOpenAmount;
				p[portal->portal_index].is_linked = portal->m_pLinkedPortal ? true : false;*/

				// first pair (can be player 1 or 2)
				if (!portal->team_num || portal->team_num == 3) // if second portal of pair is matching owning player
				{
					p[1].portal = portal;
					p[1].portal_owner = portal->m_hFiredByPlayer;
					p[1].open_amount = portal->m_fOpenAmount;
					p[1].is_linked = portal->m_pLinkedPortal ? true : false;
				}

				// second pair
				else // if second portal of pair is matching owning player
				{
					p[3].portal = portal;
					p[3].portal_owner = portal->m_hFiredByPlayer;
					p[3].open_amount = portal->m_fOpenAmount;
					p[3].is_linked = portal->m_pLinkedPortal ? true : false;
				}

				//// first pair (can be player 1 or 2)
				//if ((!parr[0].portal_owner && !parr[1].portal_owner) // first nor second portal set
				//	|| parr[0].portal_owner == portal->m_hFiredByPlayer // if first portal of pair is matching owning player
				//	|| parr[1].portal_owner == portal->m_hFiredByPlayer) // if second portal of pair is matching owning player
				//{
				//	parr[1].portal = portal;
				//	parr[1].portal_owner = portal->m_hFiredByPlayer;
				//	parr[1].index = 1;
				//	parr[1].open_amount = portal->m_fOpenAmount;
				//	parr[1].is_linked = portal->m_pLinkedPortal ? true : false;
				//}

				//// second pair
				//else if ((!parr[2].portal_owner && !parr[3].portal_owner) // first nor second portal of set
				//	|| parr[2].portal_owner == portal->m_hFiredByPlayer // if first portal of pair is matching owning player
				//	|| parr[3].portal_owner == portal->m_hFiredByPlayer) // if second portal of pair is matching owning player
				//{
				//	parr[3].portal = portal;
				//	parr[3].portal_owner = portal->m_hFiredByPlayer;
				//	parr[3].index = 3;
				//	parr[3].open_amount = portal->m_fOpenAmount;
				//	parr[3].is_linked = portal->m_pLinkedPortal ? true : false;
				//}

				/*model_render::portal2_is_linked = portal->m_pLinkedPortal ? true : false; 
				model_render::portal2_open_amount = portal->m_fOpenAmount;
				model_render::portal2_ptr = portal;*/
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

	// Grabs area portal - could be used to spawn api portals
	// Could also be used to auto. set vis for area portals but performance can be really bad
	void grab_linked_area_portal(void* cportalrenderable)
	{
		// CPortalRenderable + 0x96 = linked portal
		auto p = reinterpret_cast<CPortalRenderable_FlatBasic*>(*((DWORD*)cportalrenderable + 0x96));

		if (std::ranges::find(model_render::linked_area_portals.begin(), model_render::linked_area_portals.end(), p) == model_render::linked_area_portals.end())
		{
			// Pointer does not exist, so add it
			model_render::linked_area_portals.push_back(p);
		}
	}

	HOOK_RETN_PLACE_DEF(render_portal_view_to_backbuffer_retn);
	void __declspec(naked) render_portal_view_to_backbuffer_stub()
	{
		__asm
		{
			pushad;
			push	ecx; // CPortalRenderable*
			call	grab_linked_area_portal;
			add		esp, 4;
			popad;

			// og
			mov     eax, [ebp - 8];
			jmp		render_portal_view_to_backbuffer_retn;
		}
	}


	// #
	// #

	HOOK_RETN_PLACE_DEF(draw_painted_surfaces_og_func);
	HOOK_RETN_PLACE_DEF(draw_painted_surfaces_retn_addr);
	void __declspec(naked) draw_painted_surfaces_stub()
	{
		__asm
		{
			mov		is_rendering_paint, 1;
			call	draw_painted_surfaces_og_func;
			mov		is_rendering_paint, 0;
			jmp		draw_painted_surfaces_retn_addr;
		}
	}


	HOOK_RETN_PLACE_DEF(draw_painted_bmodel_surfaces_retn_addr);
	void __declspec(naked) draw_painted_bmodel_surfaces_stub()
	{
		__asm
		{
			mov		is_rendering_bmodel_paint, 1;

			// og
			push    edi;
			push    0xFFFFFFFF;
			call    edx; // mesh->Draw()
			
			mov		is_rendering_bmodel_paint, 0;
			jmp		draw_painted_bmodel_surfaces_retn_addr;
		}
	}


	// #
	// #

	model_render::model_render()
	{
		tbl_hk::model_renderer::_interface = utils::module_interface.get<tbl_hk::model_renderer::IVModelRender*>("engine.dll", "VEngineModel016");

		XASSERT(tbl_hk::model_renderer::table.init(tbl_hk::model_renderer::_interface) == false);
		XASSERT(tbl_hk::model_renderer::table.hook(&tbl_hk::model_renderer::DrawModelExecute::Detour, tbl_hk::model_renderer::DrawModelExecute::Index) == false);

		utils::hook(RENDERER_BASE + USE_OFFSET(0xB1B3, 0xAD23), cmeshdx8_renderpass_pre_draw_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(cmeshdx8_renderpass_pre_draw_retn_addr, RENDERER_BASE + USE_OFFSET(0xB1B8, 0xAD28));

		utils::hook(RENDERER_BASE + USE_OFFSET(0xB285, 0xADF5), cmeshdx8_renderpass_post_draw_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(cmeshdx8_renderpass_post_draw_retn_addr, RENDERER_BASE + USE_OFFSET(0xB28C, 0xADFC));


		// brushmodels - cubes - etc
		tbl_hk::bmodel_renderer::_interface = utils::module_interface.get<tbl_hk::bmodel_renderer::IVRenderView*>("engine.dll", "VEngineRenderView013");
		XASSERT(tbl_hk::bmodel_renderer::table.init(tbl_hk::bmodel_renderer::_interface) == false);
		XASSERT(tbl_hk::bmodel_renderer::table.hook(&tbl_hk::bmodel_renderer::DrawBrushModelEx::Detour, tbl_hk::bmodel_renderer::DrawBrushModelEx::Index) == false);

		// enable mat_wireframe on portals
		//utils::hook::nop(CLIENT_BASE + 0x2BD41C, 6);
#if 0
		// 0xA685
		utils::hook(RENDERER_BASE + 0xA685, cmeshdx8_renderpasswithvertexindexbuffer_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(cmeshdx8_renderpasswithvertexindexbuffer_retn_addr, RENDERER_BASE + 0xA68D);
#endif

		// C_FuncAreaPortalWindow::DrawModel :: disable drawing Area Portal Brushmodels
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0xA15FE, 0x9DE9E), 2);

		// C_Prop_Portal::ClientThink :: hook to get portal 1/2 m_fOpenAmount member var
		utils::hook(CLIENT_BASE + USE_OFFSET(0x285AD2, 0x280012), prop_portal_client_think_stub, HOOK_JUMP).install()->quick();

		// #
		// Window portals

#if 0	// not helping with entity culling
		// disable 'r_portal_stencil_depth' check in 'CPortalRender::DrawPortalsUsingStencils_Old'
		utils::hook::set<BYTE>(CLIENT_BASE + USE_OFFSET(0x2BE21B, 0x2B778B), 0xEB); // jl to jmp

		// disable 'RenderPortalViewToBackBuffer' call in 'CPortalRender::DrawPortalsUsingStencils_Old'
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x2BE7E8, 0x2B7D58),7);
		utils::hook(CLIENT_BASE + USE_OFFSET(0x2BE7E8, 0x2B7D58), render_portal_view_to_backbuffer_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(render_portal_view_to_backbuffer_retn, CLIENT_BASE + USE_OFFSET(0x2BE7EF, 0x2B7D5F));
#endif

		// Shader_DrawSurfaceDynamic -> BuildMSurfaceVertexArrays :: change texcoords when building the vertexbuffer
		// so that we do not need to lock and unlock for each BSP surface when rendering
		utils::hook(ENGINE_BASE + USE_OFFSET(0xF7D16, 0xF7193), BuildMSurfaceVertexArrays_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(BuildMSurfaceVertexArrays_retn_addr, ENGINE_BASE + USE_OFFSET(0xF7D1B, 0xF7198));

		utils::hook(ENGINE_BASE + USE_OFFSET(0xE95BD, 0xE8C7D), draw_painted_surfaces_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(draw_painted_surfaces_retn_addr, ENGINE_BASE + USE_OFFSET(0xE95C2, 0xE8C82));
		HOOK_RETN_PLACE(draw_painted_surfaces_og_func, ENGINE_BASE + USE_OFFSET(0xE2580, 0xE1C20));

		// CBrushBatchRender::DrawOpaqueBrushModel :: hook around mesh->Draw to detect paint rendering
		utils::hook(ENGINE_BASE + USE_OFFSET(0x7271C, 0x7231C), draw_painted_bmodel_surfaces_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(draw_painted_bmodel_surfaces_retn_addr, ENGINE_BASE + USE_OFFSET(0x72721, 0x72321));

		// ----

		// modify trail vertices upon creation, right before the mesh gets unlocked
#ifdef SPRITE_TRAIL_TEST
		// C_OP_RenderRope::RenderSpriteCard_Internal<FastRopeVertex_t>
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x61EA74, 0x6165E4), 6);
		utils::hook(CLIENT_BASE + USE_OFFSET(0x61EA74, 0x6165E4), RenderSpriteCardFastRopeVertex_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(RenderSpriteCardFastRopeVertex_retn_addr, CLIENT_BASE + USE_OFFSET(0x61EA7A, 0x6165EA));

		// C_OP_RenderRope::RenderSpriteCard_Internal<FastRopeVertexNormal_t>
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x620056, 0x617BC6), 6);
		utils::hook(CLIENT_BASE + USE_OFFSET(0x620056, 0x617BC6), RenderSpriteCardFastRopeVertexNormal_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(RenderSpriteCardFastRopeVertexNormal_retn_addr, CLIENT_BASE + USE_OFFSET(0x62005C, 0x617BCC));

		// C_OP_RenderRope::RenderSpriteCard_Internal<FastRopeVertexNormalCacheAligned_t>
		// not in use?
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x621649, 0x6191B9), 6);
		utils::hook(CLIENT_BASE + USE_OFFSET(0x621649, 0x6191B9), RenderSpriteCardFastRopeVertexNormalCache_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(RenderSpriteCardFastRopeVertexNormalCache_retn_addr, CLIENT_BASE + USE_OFFSET(0x62164F, 0x6191BF));

		// C_OP_RenderSpritesTrail::Render
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x62257E, 0x61A0EE), 6);
		utils::hook(CLIENT_BASE + USE_OFFSET(0x62257E, 0x61A0EE), RenderSpritesTrail_Render_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(RenderSpritesTrail_Render_retn_addr, CLIENT_BASE + USE_OFFSET(0x622584, 0x61A0F4));
#endif

		// C_OP_RenderSprites::Render :: fix SpriteCard UV's
		utils::hook::nop(CLIENT_BASE + USE_OFFSET(0x622030, 0x619BA0), 6);
		utils::hook(CLIENT_BASE + USE_OFFSET(0x622030, 0x619BA0), RenderSpriteCardNew_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(RenderSpriteCardNew_retn_addr, CLIENT_BASE + USE_OFFSET(0x622036, 0x619BA6));
	}
}

