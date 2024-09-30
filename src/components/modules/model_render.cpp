#include "std_include.hpp"

namespace components
{
	IDirect3DVertexShader9* og_model_shader = nullptr;
	bool is_portalgun_viewmodel = false;

	void __fastcall tbl_hk::model_renderer::DrawModelExecute::Detour(void* ecx, void* edx, void* oo, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		const auto dev = game::get_d3d_device();
		dev->GetVertexShader(&og_model_shader);
		dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));

	/*	if (std::string_view(pInfo.pModel->szPathName).contains("stair"))
		{
			int dbg = 0;
		}*/

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
			// use viewmodel_offset_ instead?

			/*VMatrix mat = {};
			mat.m[0][0] = game::identity[0][0];
			mat.m[0][1] = game::identity[0][1];
			mat.m[0][2] = game::identity[0][2];
			mat.m[0][3] = game::identity[0][3];

			mat.m[1][0] = game::identity[1][0];
			mat.m[1][1] = game::identity[1][1];
			mat.m[1][2] = game::identity[1][2];
			mat.m[1][3] = game::identity[1][3];

			mat.m[2][0] = game::identity[2][0];
			mat.m[2][1] = game::identity[2][1];
			mat.m[2][2] = game::identity[2][2];
			mat.m[2][3] = game::identity[2][3];

			mat.m[3][0] = game::identity[3][0];
			mat.m[3][1] = game::identity[3][1];
			mat.m[3][2] = game::identity[3][2];
			mat.m[3][3] = game::identity[3][3];

			Vector fwd = {};
			utils::vector::AngleVectors(&pInfo.angles.x, &fwd);
			fwd = fwd.Scale(-9.0f);

			mat.m[3][0] = fwd[0];
			mat.m[3][1] = fwd[1];
			mat.m[3][2] = fwd[2];

			is_portalgun_viewmodel = true;
			dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mat.m));*/
			dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));
		}
		else
		{
			int y = 0;
		}

		tbl_hk::model_renderer::table.original<FN>(Index)(ecx, edx, oo, state, pInfo, pCustomBoneToWorld);

		is_portalgun_viewmodel = false;
		dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));
		dev->SetFVF(NULL);

		if (og_model_shader)
		{
			dev->SetVertexShader(og_model_shader);
			og_model_shader = nullptr;
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

	LPDIRECT3DTEXTURE9 tex_portal_mask;
	LPDIRECT3DTEXTURE9 tex_portal_blue;
	LPDIRECT3DTEXTURE9 tex_portal_orange;

	LPDIRECT3DTEXTURE9 tex_glass_shards;

	int do_not_render_next_mesh = false;
	std::uint64_t tick_on_first_no_render = 0;
	

	void cmeshdx8_renderpass_pre_draw(CMeshDX8* mesh)
	{
		const auto dev = game::get_d3d_device();

		IDirect3DVertexBuffer9* b = nullptr;
		UINT ofs = 0, stride = 0;
		dev->GetStreamSource(0, &b, &ofs, &stride);

		Vector* model_org = reinterpret_cast<Vector*>(ENGINE_BASE + 0x50DA90);
		VMatrix* model_to_world_mtx = reinterpret_cast<VMatrix*>(ENGINE_BASE + 0x637158);

		// #TODO init textures elsewhere
		if (!tex_portal_mask)
		{
			D3DXCreateTextureFromFileA(dev, "portal_mask.png", &tex_portal_mask);
		}

		if (!tex_portal_blue)
		{
			D3DXCreateTextureFromFileA(dev, "portal_blue.png", &tex_portal_blue);
		}

		if (!tex_portal_orange)
		{
			D3DXCreateTextureFromFileA(dev, "portal_orange.png", &tex_portal_orange);
		}

		if (!tex_glass_shards)
		{
			D3DXCreateTextureFromFileA(dev, "glass_shards.png", &tex_glass_shards);
		}

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

		if (og_bmodel_shader && mesh->m_VertexFormat == 0x2480033)
		{
			//do_not_render_next_mesh = true;

			dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));

			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX7);
			dev->SetVertexShader(nullptr);
		}
		else if (og_model_shader && mesh->m_VertexFormat == 0xa0003 /*stride == 48*/) // player model - gun - grabable stuff - portal button - portal door
		{
			//do_not_render_next_mesh = true;

			if (auto shaderapi = game::get_shaderapi(); shaderapi)
			{
				if (auto cmat = shaderapi->vtbl->GetBoundMaterial(shaderapi, nullptr); cmat)
				{
					if (auto name = cmat->vftable->GetName(cmat); name)
					{
						if (std::string_view(name).contains("props_destruction/glass_"))
						{
							//do_not_render_next_mesh = true;

							if (tex_glass_shards)
							{
								dev->SetTexture(0, tex_glass_shards);
							}
							
						}
					}
				}
			}


			if (!is_portalgun_viewmodel)
			{
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
			}

			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX6);
			dev->SetVertexShader(nullptr); // vertexformat 0x00000000000a0003 

			//mtx[3][2] = sinf((float)main_module::framecount * 0.05f) * 10.0f;
		}

		// this also renders the glass? infront of white panel lamps
		// also renders some foliage (2nd level - emissive)
		else if (og_model_shader) // should be stride 30
		{
			//do_not_render_next_mesh = true;
			// glass/glasswindow_refract01_neutral

			if (auto shaderapi = game::get_shaderapi(); shaderapi) 
			{
				if (auto cmat = shaderapi->vtbl->GetBoundMaterial(shaderapi, nullptr); cmat)
				{
					if (auto name = cmat->vftable->GetName(cmat); name)
					{
						const auto sname = std::string_view(name);

						// TODO replace with unique texture
						/*if (std::string_view(name).contains("glasswindow"))
						{
							IDirect3DBaseTexture9* nml = nullptr;
							dev->GetTexture(1, &nml);

							if (nml)
							{
								dev->SetTexture(0, nml);
							}
						}*/

						// replace glass refract with wireframe
						if (auto shadername = cmat->vftable->GetShaderName(cmat); shadername)
						{
							if (sname.contains("Refract_DX90"))
							{
								cmat->vftable->SetShader(cmat, "Wireframe");
								int x = 1;
							}
							//else if (sname.contains("props_foliage/leaves")
							//	|| sname.contains("vine_cluster_loop01_dry_alphatest")
							//	|| sname.contains("props_foliage/vines_"))
							//{
							//	//dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
							//	//dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_XRGB(255, 0, 0));

							//	//dev->SetTextureStageState(0, (_D3DTEXTURESTAGESTATETYPE)1, D3DTOP_SELECTARG1);
							//	//dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);  // Use vertex color
							//	//do_not_render_next_mesh = true;
							//	int x = 1; 
							//}
							else
							{
								int x = 1;
							}
						}
					}
				}
			}

			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3);
			dev->SetVertexShader(nullptr); // vertexformat 0x00000000000a0003
		}
		else
		{
			bool was_portal_related = false;
			bool was_world = false;

			if (auto shaderapi = game::get_shaderapi(); shaderapi)
			{
				if (auto cmat = shaderapi->vtbl->GetBoundMaterial(shaderapi, nullptr); cmat)
				{
					if (auto name = cmat->vftable->GetName(cmat); name)
					{
						// r_portal_stencil_depth 0 heavy influence
						if (std::string_view(name).contains("portal_stencil"))
						{
							// is it better to render this with a null texture or disable rendering of the quad
							// using do_not_render_next_mesh
							/*if (tex_portal_mask)
							{
								dev->SetTexture(0, nullptr);
							}*/
							was_portal_related = true; // prevent all other else's
							do_not_render_next_mesh = true;
						}
						//else if (std::string_view(name).contains("dev/lumcompare"))
						//{
						//	was_portal_related = true; // prevent all other else's
						//	do_not_render_next_mesh = true;
						//}
						else if (std::string_view(name).contains("portalstaticoverlay_1"))
						{
							//if (!model_render::portal1_render_count)  
							{
								if (tex_portal_mask)
								{
									dev->GetTexture(1, &ff_portalfx_03::s_texture2);
									dev->SetTexture(1, tex_portal_mask);
								}

								if (tex_portal_blue)
								{
									dev->GetTexture(0, &ff_portalfx_03::s_texture1);
									dev->SetTexture(0, tex_portal_blue);
								}
							}

							//do_not_render_next_mesh = true;

							// replace with wireframe (makes life much easier)
							if (auto shadername = cmat->vftable->GetShaderName(cmat); shadername)
							{
								//if (std::string_view(shadername).contains("PortalRefract_dx9"))
								if (std::string_view(shadername) != "Wireframe_DX9")
								{
									cmat->vftable->SetShader(cmat, "Wireframe"); 
									int x = 1;
								}
								int x = 1;
							}

							was_portal_related = true;
							model_render::portal1_render_count++;
						}
						else if (std::string_view(name).contains("portalstaticoverlay_2"))
						{
							//if (!model_render::portal2_render_count)
							{
								if (tex_portal_mask)
								{
									dev->GetTexture(1, &ff_portalfx_03::s_texture2);
									dev->SetTexture(1, tex_portal_mask);
								}

								if (tex_portal_orange) 
								{
									dev->GetTexture(0, &ff_portalfx_03::s_texture1);
									dev->SetTexture(0, tex_portal_orange);
								}
							}
							/*else
							{
								do_not_render_next_mesh = true;
							}*/

							// replace with wireframe (makes life much easier)
							if (auto shadername = cmat->vftable->GetShaderName(cmat); shadername)
							{
								//if (std::string_view(shadername).contains("PortalRefract_dx9"))
								if (std::string_view(shadername) != "Wireframe_DX9")
								{
									cmat->vftable->SetShader(cmat, "Wireframe");
									int x = 1;
								}
								int x = 1;
							}

							was_portal_related = true;
							model_render::portal2_render_count++;
						}

						if (was_portal_related)
						{
							// through wall overlays
							if (mesh->m_VertexFormat == 0xa0007)
							{
								int x = 1;
								//do_not_render_next_mesh = true;

								//dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX5 | D3DFVF_TEXCOORDSIZE1(4)); // 68 - 4 as last tc is one float
								//dev->GetVertexShader(&ff_portalfx_01::s_shader);
								//dev->SetVertexShader(nullptr);
								//dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));
							}

							// actually draws the portals
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
								int x = 1; 
							}
						}
					}
				}
			}

			if (was_portal_related)
			{
				int x = 0;
				
			}
			// world geo? - floor / walls
			else if (mesh->m_VertexFormat == 0x2480033)
			{
				was_world = true;
				//do_not_render_next_mesh = true;

				// tc @ 24
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX7);
				dev->GetVertexShader(&ff_worldmodel::s_shader);
				dev->SetVertexShader(nullptr);
				//dev->GetTransform(D3DTS_WORLD, &saved_world_mtx_unk);

				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));
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

			// laser platforms
			else if (mesh->m_VertexFormat == 0x80001)
			{
				//do_not_render_next_mesh = true;

				// tc @ 12
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(1)); // missing 4 bytes at the end here - fixed with tc2 size 3?
				dev->GetVertexShader(&ff_laserplatform::s_shader);
				dev->SetVertexShader(nullptr);

				//float mtx1[4][4] = {};
				//mtx1[0][0] = game::identity[0][0];
				//mtx1[0][1] = game::identity[0][1];
				//mtx1[0][2] = game::identity[0][2];
				//mtx1[0][3] = game::identity[0][3];

				//mtx1[1][0] = game::identity[1][0];
				//mtx1[1][1] = game::identity[1][1];
				//mtx1[1][2] = game::identity[1][2];
				//mtx1[1][3] = game::identity[1][3];

				//mtx1[2][0] = game::identity[2][0];
				//mtx1[2][1] = game::identity[2][1];
				//mtx1[2][2] = game::identity[2][2];
				//mtx1[2][3] = game::identity[2][3];

				//mtx1[3][0] = game::identity[3][0];
				//mtx1[3][1] = game::identity[3][1];
				//mtx1[3][2] = 0.0f; //sinf((float)main_module::framecount * 0.05f) * 40.0f;
				//mtx1[3][3] = game::identity[3][3];

				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));

				dev->GetTransform(D3DTS_TEXTURE0, &ff_laserplatform::s_tc_transform);
				//dev->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, &laserplatform::s_tc_stage);

				// tc scroll
				D3DXMATRIX ret = ff_laserplatform::s_tc_transform;
				ret(3, 1) = (float)main_module::framecount * 0.01f;

				dev->SetTransform(D3DTS_TEXTURE0, &ret);
				dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			}

			// lasers - indicator dots - some of the white light stripes
			// this also renders the the wireframe portal if hook enabled to render wireframe for portals
			// also renders white stuff behind sliding doors
			else if (mesh->m_VertexFormat == 0x80003)
			{
				//do_not_render_next_mesh = true;
				
				if (auto shaderapi = game::get_shaderapi(); shaderapi)
				{
					if (auto cmat = shaderapi->vtbl->GetBoundMaterial(shaderapi, nullptr); cmat)
					{
						if (auto name = cmat->vftable->GetName(cmat); name)
						{
							// fix sliding door background
							if (std::string_view(name).contains("decals/portalstencildecal"))
							{
								//do_not_render_next_mesh = true;

								// todo set unique texture
								dev->SetTexture(0, tex_portal_mask);

								//if (auto shadername = cmat->vftable->GetShaderName(cmat); shadername)
								//{
								//	//if (std::string_view(shadername).contains("PortalRefract_dx9"))
								//	/*if (std::string_view(shadername) != "Wireframe_DX9")
								//	{
								//		cmat->vftable->SetShader(cmat, "Wireframe");
								//		int x = 1;
								//	}*/
								//	int x = 1;
								//}
								int yy = 1; 
							}
						}
					}
				}

				dev->GetVertexShader(&ff_laser::s_shader); 
				
				if (mesh->m_Mode == D3DPT_TRIANGLELIST)
				{
					// tc @ 12
					//dev->SetFVF(D3DFVF_XYZW | D3DFVF_NORMAL | D3DFVF_TEX1 |
					//	D3DFVF_TEXCOORDSIZE2(0) | // tc0 uses 2 floats
					//	//D3DFVF_TEXCOORDSIZE2(1) | // tc1 uses 2 floats
					//	D3DFVF_TEXCOORDSIZE1(1)); // tc2 uses 1 float

					dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));
					dev->SetVertexShader(nullptr);

					//swing = true;
				}
				if (mesh->m_Mode == D3DPT_TRIANGLESTRIP)
				{
					dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));
					dev->SetVertexShader(nullptr);
				}

				/*float mtx[4][4] = {};
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

				mtx[3][0] = game::identity[3][0];
				mtx[3][1] = game::identity[3][1];
				mtx[3][2] = swing ? (sinf((float)main_module::framecount * 0.05f) * 2.0f) : 0.0f;
				mtx[3][3] = game::identity[3][3];*/

				//dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));
			}

			// portal_draw_ghosting 0 disables this
			// this normally shows portals through walls
			else if (mesh->m_VertexFormat == 0xa0007) // portal fx
			{
				//do_not_render_next_mesh = true;

				// could be of use
				/*if (current_render_ent) 
				{
					if (current_render_ent->model && 
						std::string_view(current_render_ent->model->szPathName).contains("portal1.mdl"))
					{
						main_module::portal1.matrix[0][3] = current_render_ent->m_vecAbsOrigin.x;
						main_module::portal1.matrix[1][3] = current_render_ent->m_vecAbsOrigin.y;
						main_module::portal1.matrix[2][3] = current_render_ent->m_vecAbsOrigin.z;
					}

					int x = 1;
				}*/

				// tc at 16 + 12 
				//dev->SetFVF(D3DFVF_XYZB4 | D3DFVF_TEX5 | D3DFVF_TEXCOORDSIZE1(4)); // 68 - 4 as last tc is one float
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX5 | D3DFVF_TEXCOORDSIZE1(4)); // 68 - 4 as last tc is one float
				//dev->SetFVF(NULL);
				dev->GetVertexShader(&ff_portalfx_01::s_shader);

				//if (!ff_portalfx_01::s_texture)
				{
					//dev->GetTexture(0, &ff_portalfx_01::s_texture);
					//dev->GetTexture(1, &ff_portalfx_01::s_texture2);

					//dev->SetTexture(0, nullptr);
					//dev->SetTexture(1, nullptr);
				}
				/*else
				{
					int z = 0;
				}*/

				dev->SetVertexShader(nullptr);
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity)); // 0x6c0005 influences this one here???

				// first two are the static portal overlays (2 prims each)
				// recursive overlays after that

				// the remix runtime expects a portal mask texture in the second texture slot .... :D

				/*if (!tex_portal_mask)
				{
					D3DXCreateTextureFromFileA(dev, "portal_mask.png", &tex_portal_mask);
				}

				if (tex_portal_mask)
				{
					dev->SetTexture(1, tex_portal_mask);
				}*/

				// no change in tiny portal bug 
				//if (model_render::portal1_render_count >= 2)
				//{
				//	int x = 1;
				//	//do_not_render_next_mesh = true;
				//}

				//model_render::portal_meshes_rendered_count++;

				/*if (tick_on_first_no_render != main_module::framecount)
				{
					tick_on_first_no_render = main_module::framecount;
					do_not_render_next_mesh = true;
				}*/
			}

			// related to props like portalgun, pickup-ables
			// does not have a visibile texture set? (setting one manually makes it show up)
			else if (mesh->m_VertexFormat == 0x6c0005)
			{
				//do_not_render_next_mesh = true;

				if (current_render_ent)
				{
					int z = 1;
				}
				// stride 48
				int x = 1;
				dev->SetFVF(D3DFVF_XYZB1 | D3DFVF_TEX5);
				dev->GetVertexShader(&saved_shader_unk);
				//dev->SetTexture(0, nullptr);
				dev->SetVertexShader(nullptr);

				//if (ff_portalfx_01::s_texture2)
				//{
				//	//int x = 1;
				//	dev->SetTexture(0, ff_portalfx_01::s_texture);
				//}

				float mtx[4][4] = {};
				/*mtx[0][0] = game::identity[0][0];
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

				mtx[3][0] = game::identity[3][0];
				mtx[3][1] = game::identity[3][1];
				mtx[3][2] = 40.0f + sinf((float)main_module::framecount * 0.05f) * 6.0f;
				mtx[3][3] = game::identity[3][3];*/
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
			}

			// white overlay on portals? - getting tex1 and setting it onto tex0 shows
			// a texture (prob. lmap) that matches the tiny portals
			// does not look like its responsible for the tiny portals tho

			//  this draws the portal1 and portal2 mesh
			else if (mesh->m_VertexFormat == 0x4a0003)
			{
				//do_not_render_next_mesh = true;

				//struct IShaderAPIDX8_vtbl
				//{
				//	char pad[0x41C];
				//	IMaterial* (__fastcall* GetBoundMaterial)(void* shaderapi_ptr, void* ecx);
				//};
				//struct IShaderAPIDX8
				//{
				//	IShaderAPIDX8_vtbl* vtbl;
				//};
				//auto shaderapi = reinterpret_cast<IShaderAPIDX8*>(/**(DWORD*)*/(*(DWORD*)(RENDERER_BASE + 0x164C48)));

				auto shaderapi = game::get_shaderapi();

				//if (current_render_ent)
				//{
				//	if (current_render_ent->model /*&& std::string_view(current_render_ent->model->szPathName).contains("portal1.mdl")*/)
				//	{
				//		int x = 1;
				//	}
				//}

				//if (shaderapi)
				//{
				//	auto mat = shaderapi->vtbl->GetBoundMaterial(shaderapi, nullptr);
				//	if (mat)
				//	{
				//		auto name = mat->vftable->GetName(mat);

				//		if (name)
				//		{
				//			if (std::string_view(name).contains("portal_stencil"))
				//			{
				//				//do_not_render_next_mesh = true;
				//				/*if (tex_portal_mask) // nope
				//				{
				//					dev->SetTexture(1, tex_portal_mask);
				//				}*/
				//			}
				//			else if (std::string_view(name).contains("portalstaticoverlay_1"))
				//			{
				//				if (tex_portal_mask)
				//				{
				//					dev->SetTexture(1, tex_portal_mask);
				//				}

				//				//if (tex_portal_blue)
				//				{
				//					dev->SetTexture(0, tex_portal_blue);
				//				}

				//				model_render::portal1_render_count++;
				//			}
				//			else if (std::string_view(name).contains("portalstaticoverlay_2"))
				//			{
				//				if (tex_portal_mask)
				//				{
				//					dev->SetTexture(1, tex_portal_mask);
				//				}

				//				//if (tex_portal_orange)
				//				{
				//					dev->SetTexture(0, tex_portal_orange);
				//				}
				//			}
				//		}

				//		int x = 1;
				//	}
				//}

				// stride 0x40
				//do_not_render_next_mesh = true;

				struct CMaterialReference
				{
					IMaterial* m_pMaterial;
				};
				struct PropPortalRenderingMaterials_t
				{
					CMaterialReference m_PortalMaterials[2];
					CMaterialReference m_PortalRenderFixMaterials[2];
					CMaterialReference m_PortalDepthDoubler;
					CMaterialReference m_PortalStaticOverlay[2];
					CMaterialReference m_PortalStaticOverlay_Tinted;
					CMaterialReference m_PortalStaticGhostedOverlay[2];
					CMaterialReference m_PortalStaticGhostedOverlay_Tinted;
					CMaterialReference m_Portal_Stencil_Hole;
					CMaterialReference m_Portal_Refract;
					unsigned int m_nDepthDoubleViewMatrixVarCache;
					unsigned int m_nStaticOverlayTintedColorGradientLightVarCache;
					Vector m_coopPlayerPortalColors[2][2];
					Vector m_singlePlayerPortalColors[2];
				};
				struct CAutoInitBasicPropPortalDrawingMaterials
				{
					char pad[0xC];
					PropPortalRenderingMaterials_t m_Materials;
				};
				auto base_portal_mats = reinterpret_cast<CAutoInitBasicPropPortalDrawingMaterials*>(CLIENT_BASE + 0x9FE710); // CAutoInitBasicPropPortalDrawingMaterials

				//dev->GetTexture(0, &ff_portalfx_03::s_texture1);
				//dev->SetTexture(0, nullptr);

				

				

				//if (current_render_ent)
				//{
				//	if (current_render_ent->model && std::string_view(current_render_ent->model->szPathName).contains("portal2.mdl"))
				//	{
				//		if (tex_portal_blue)
				//		{
				//			dev->SetTexture(0, tex_portal_blue);
				//		}

				//		// always set mask
				//		if (tex_portal_mask)
				//		{
				//			//dev->SetTexture(0, tex_portal_mask);
				//			dev->SetTexture(1, tex_portal_mask);
				//		}
				//	}

				//}
				

				//if (current_render_ent)
				//{
				//	if (current_render_ent->model && std::string_view(current_render_ent->model->szPathName).contains("portal1.mdl"))
				//	{
				//		dev->SetTexture(0, tex_portal_orange);

				//		// always set mask
				//		if (tex_portal_mask)
				//		{
				//			//dev->SetTexture(0, tex_portal_mask);
				//			dev->SetTexture(1, tex_portal_mask);
				//		}
				//	}
				//	else if (current_render_ent->model && std::string_view(current_render_ent->model->szPathName).contains("portal2.mdl"))
				//	{
				//		dev->SetTexture(0, tex_portal_blue);

				//		// always set mask
				//		if (tex_portal_mask)
				//		{
				//			//dev->SetTexture(0, tex_portal_mask);
				//			dev->SetTexture(1, tex_portal_mask);
				//		}
				//	}
				//}

				

#if 0
				if (current_render_ent)
				{
					if (current_render_ent->model &&
						std::string_view(current_render_ent->model->szPathName).contains("portal1.mdl"))
					{
						if (!local_texture_test01)
						{
							D3DXCreateTextureFromFileA(dev, "portal1.bmp", &local_texture_test01);
						}

						dev->GetTexture(0, &ff_portalfx_03::s_texture1);

						if (local_texture_test01)
						{
							dev->SetTexture(0, local_texture_test01);
							/*dev->SetTexture(1, nullptr);
							dev->SetTexture(2, nullptr);
							dev->SetTexture(3, nullptr);
							dev->SetTexture(4, nullptr);*/
						}
					}
					else if (current_render_ent->model &&
							std::string_view(current_render_ent->model->szPathName).contains("portal2.mdl"))
						{
							if (!local_texture_test02)
							{
								D3DXCreateTextureFromFileA(dev, "portal2.bmp", &local_texture_test02);
							}

							dev->GetTexture(0, &ff_portalfx_03::s_texture2);

							if (local_texture_test02)
							{
								dev->SetTexture(0, local_texture_test02);
								/*dev->SetTexture(1, nullptr);
								dev->SetTexture(2, nullptr);
								dev->SetTexture(3, nullptr);
								dev->SetTexture(4, nullptr);*/
							}

							//IDirect3DBaseTexture9* t1 = nullptr;
							//dev->GetTexture(3, &t1);
							//dev->SetTexture(0, t1);
						}

					int z = 1;
				}
#endif

				dev->GetVertexShader(&ff_portalfx_03::s_shader); 
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64
				dev->SetVertexShader(nullptr);
			}

			// fizzle texture that breaks everything and is normally ignored?
			else if (mesh->m_VertexFormat == 0xa0003)
			{
				do_not_render_next_mesh = true;

				// stride 0x30
				dev->GetVertexShader(&ff_portalfx_02::s_shader);
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX5 | D3DFVF_TEXCOORDSIZE1(4));

				if (!ff_portalfx_02::s_texture)
				{
					dev->GetTexture(1, &ff_portalfx_02::s_texture);
					//dev->SetTexture(1, nullptr); 
				}

				// def. render using FF as the shader is causing heavy frametime drops
				dev->SetVertexShader(nullptr);

				// 0 scale gets rid of it (disable to make it show up)
				float mtx[4][4] = {};
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
			}

			// HUD
			else if (mesh->m_VertexFormat == 0x80007) 
			{
				//do_not_render_next_mesh = true;
				// IsDepthWriteEnabled
				// GetBufferedState
				// GetCullMode
				if (auto shaderapi = game::get_shaderapi(); shaderapi)
				{
					if (auto cmat = shaderapi->vtbl->GetBoundMaterial(shaderapi, nullptr); cmat)
					{
						if (auto name = cmat->vftable->GetName(cmat); name)
						{
							const auto sname = std::string_view(name);
							// get rid of all world-rendered text as its using the same glyph as HUD elements?!
							if (sname.contains("vgui__fontpage"))
							{
								BufferedState_t state = {};
								shaderapi->vtbl->GetBufferedState(shaderapi, nullptr, &state);

								if (state.m_Transform[0].m[3][0] != 0.0f)
								{
									do_not_render_next_mesh = true;
								}
							}
							else if (sname.contains("vgui_coop_progress_board")
								|| sname.contains("p2_lightboard_vgui")
								|| sname.contains("elevator_video_overlay3"))
							{
								//do_not_render_next_mesh = true;
								// gameinstructor_iconsheet1

								/*IDirect3DBaseTexture9* vid = nullptr;
								dev->GetTexture(1, &vid); 
								dev->SetTexture(0, vid);*/

								BufferedState_t state = {};
								shaderapi->vtbl->GetBufferedState(shaderapi, nullptr, &state);

								dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&state.m_Transform[0]));

								// no need to set fvf here?
								//dev->SetFVF(D3DFVF_XYZB3 | D3DFVF_TEX4);

								dev->GetVertexShader(&ff_vgui::s_shader01);
								dev->SetVertexShader(nullptr);
							}
							else if (sname.contains("elevator_video_lines"))
							{
								BufferedState_t state = {};
								shaderapi->vtbl->GetBufferedState(shaderapi, nullptr, &state);

								dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&state.m_Transform[0]));

								//IDirect3DBaseTexture9* vid = nullptr;
								//dev->GetTexture(1, &vid);
								//IDirect3DBaseTexture9* vid = shaderapi->vtbl->GetD3DTexture(shaderapi, nullptr, state.m_BoundTexture[0]);
								//dev->SetTexture(0, vid);

								dev->GetVertexShader(&ff_vgui::s_shader01);
								dev->SetVertexShader(nullptr);
							}
							else if (sname.contains("vgui_white"))
							{
								int x = 1;
							}
							else
							{
								int x = 1; 
							}
							
							int y = 1; 

							// replace glass refract with wireframe
							//if (auto shadername = cmat->vftable->GetShaderName(cmat); shadername)
							//{
							//	if (std::string_view(shadername).contains("Refract_DX90"))
							//	{
							//		//cmat->vftable->SetShader(cmat, "Wireframe");
							//		int x = 1;
							//	}
							//}
						}
					}
				}

				int z = 0; 
				//dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mat));
				//dev->SetFVF(D3DFVF_XYZW | D3DFVF_TEX4);
				//dev->GetVertexShader(&saved_shader_unk);
				//dev->SetVertexShader(nullptr);
			}

			// ?
			else if (mesh->m_VertexFormat == 0x92480005)
			{
				//do_not_render_next_mesh = true;
				int zz = 1; 
			}

			// on portal open - spark fx (center)
			// also portal clearing gate (blue sweeping beam)
			// can be rendered but also requires vertexshader + position
			else if (mesh->m_VertexFormat == 0x924900005)
			{
				//do_not_render_next_mesh = true;
				// stride 0x70 - 112
				
				int zz = 1;

#if 0
				if (auto shaderapi = game::get_shaderapi(); shaderapi)
				{
					BufferedState_t state = {};
					shaderapi->vtbl->GetBufferedState(shaderapi, nullptr, &state);

					float mtx[4][4] = {};
					mtx[0][0] = game::identity[0][0];
					mtx[1][1] = game::identity[1][1];
					mtx[2][2] = game::identity[2][2];
					
					mtx[3][0] = -706.0f;
					mtx[3][1] = 3830.0f;
					mtx[3][2] = 2750.0f /*+ sinf((float)main_module::framecount * 0.05f) * 6.0f*/;
					mtx[3][3] = game::identity[3][3];

					dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
				}

				//dev->SetFVF(D3DFVF_XYZB5 | D3DFVF_NORMAL | D3DFVF_TEX2); // 80
				dev->GetVertexShader(&ff_terrain::s_shader);
				dev->SetPixelShader(nullptr);
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

			// terrain
			else if (mesh->m_VertexFormat == 0x480007)
			{
				//do_not_render_next_mesh = true;

				// tc @ 28
				dev->SetFVF(D3DFVF_XYZB4 | D3DFVF_TEX3);
				dev->GetVertexShader(&ff_terrain::s_shader);
				dev->SetVertexShader(nullptr);
			}

			// hanging cables - requires vertex shader - verts not modified on the cpu
			else if (mesh->m_VertexFormat == 0x24900005)
			{
				//do_not_render_next_mesh = true;
				// tc @ 28

				int z = 0;
#if 0
				if (auto shaderapi = game::get_shaderapi(); shaderapi)
				{
					BufferedState_t state = {};
					shaderapi->vtbl->GetBufferedState(shaderapi, nullptr, &state);
					dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&state.m_Transform[0]));
				}

				//dev->SetFVF(D3DFVF_XYZB5 | D3DFVF_NORMAL | D3DFVF_TEX2); // 80
				dev->GetVertexShader(&ff_terrain::s_shader);
				dev->SetPixelShader(nullptr);
#endif
			}

			// ?
			else if (mesh->m_VertexFormat == 0x2900005)
			{
				//do_not_render_next_mesh = true;
				int z = 0;
			}

			// on portal open - no clue
			else if (mesh->m_VertexFormat == 0x114900005)
			{
				// stride 96
				int z = 0;
				//dev->GetVertexShader(&ff_portalfx_04::s_shader);
				//dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5);
				//dev->SetVertexShader(nullptr);
			}

			// on portal open - blob in the middle (impact)
			else if (mesh->m_VertexFormat == 0x80037)
			{
				//do_not_render_next_mesh = true;
				// this needs a position as it spawns on 0 0 0
				// stride 0x40

				int x = 0;

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
			else if (mesh->m_VertexFormat == 0x1b924900005)
			{
				//do_not_render_next_mesh = true;
				// stride 0x90 - 144
				// no fitting fvf
				//dev->GetVertexShader(&ff_portalfx_04::s_shader);
				//dev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX5);
				//dev->SetVertexShader(nullptr);
			}

			// portal clearing gate
			else if (mesh->m_VertexFormat == 0x80033)
			{
				//stride = 0x40 
				//do_not_render_next_mesh = true;

				if (auto shaderapi = game::get_shaderapi(); shaderapi)
				{
					BufferedState_t state = {};
					shaderapi->vtbl->GetBufferedState(shaderapi, nullptr, &state);
					dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&state.m_Transform[0]));
				}

				// tc @ 24
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64
				dev->GetVertexShader(&ff_vgui::s_shader04);
				dev->SetVertexShader(nullptr);
			}
			else
			{
				//do_not_render_next_mesh = true;
				//dev->SetVertexShader(nullptr);
				int xx = 1;  
			}

			int zz = 1; 
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

	void cmeshdx8_renderpass_post_draw(std::uint32_t num_prims_rendered)
	{
		const auto dev = game::get_d3d_device();

		// restore shader if we set it top null right before drawing in the func above
		if (saved_shader_unk)
		{
			dev->SetVertexShader(saved_shader_unk);
			dev->SetFVF(NULL);
			saved_shader_unk = nullptr;
			//dev->SetTransform(D3DTS_WORLD, &saved_world_mtx_unk);
			//dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));
		}

		if (ff_terrain::s_shader)
		{
			dev->SetVertexShader(ff_terrain::s_shader);
			dev->SetFVF(NULL);
			ff_terrain::s_shader = nullptr;
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
	}

	HOOK_RETN_PLACE_DEF(cmeshdx8_renderpass_post_draw_retn_addr);
	void __declspec(naked) cmeshdx8_renderpass_post_draw_stub()
	{
		__asm
		{
			mov		ecx, [esi + 0x50];
			push	ecx;
			push	eax;

			mov		eax, do_not_render_next_mesh;
			test	eax, eax;
			jnz		SKIP;

			call	edx; // DrawIndexedPrimitive
			jmp		OG;

		SKIP:
			add		esp, 0x1C;

		OG:
			pushad;
			push	edi;
			call	cmeshdx8_renderpass_post_draw;
			add		esp, 4;
			popad;

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
	void cmeshdx8_renderpassforinstances_pre_draw(CMeshDX8* mesh, MeshInstanceData_t* data)
	{
		const auto dev = game::get_d3d_device();

		IDirect3DVertexBuffer9* b = nullptr;
		UINT ofs = 0, stride = 0;
		dev->GetStreamSource(0, &b, &ofs, &stride);

		Vector* model_org = reinterpret_cast<Vector*>(ENGINE_BASE + 0x50DA90);
		VMatrix* model_to_world_mtx = reinterpret_cast<VMatrix*>(ENGINE_BASE + 0x637158);

		// g_pInstanceData
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
				int x = 0;
			}
		}
		else 
		{
			// metal door = 0xa2183
			if (mesh->m_VertexFormat == 0xa2183) // entities - not brushmodel (eg portal gun stand)
			{
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<D3DMATRIX*>(&mat.m));

#if 1
				if (auto shaderapi = game::get_shaderapi(); shaderapi)
				{
					if (auto cmat = shaderapi->vtbl->GetBoundMaterial(shaderapi, nullptr); cmat)
					{
						if (auto name = cmat->vftable->GetName(cmat); name)
						{
							const auto sname = std::string_view(name);
							/*if (auto shaderapi = game::get_shaderapi(); shaderapi) 
							{
								BufferedState_t state = {};
								shaderapi->vtbl->GetBufferedState(shaderapi, nullptr, &state);
								dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&state.m_Transform[0]));
							}*/

							// blending is a shader thing I guess?
							if (sname.contains("portal_door_glass"))
							{
								//mat.m[3][1] += sinf((float)main_module::framecount * 0.05f) * 6.0f;
								dev->SetTransform(D3DTS_WORLD, reinterpret_cast<D3DMATRIX*>(&mat.m));

								if (current_render_ent)
								{
									int x = 1;
								}

								// setting to VertexLitGeneric results in vertexformat '0xa0003' and gets rendered in 'cmeshdx8_renderpass_pre_draw'
								//cmat->vftable->SetShader(cmat, "VertexLitGeneric"); 
								
								if (auto shadername = cmat->vftable->GetShaderName(cmat); shadername)
								{
								//	//if (std::string_view(shadername).contains("PortalRefract_dx9"))
								//	/*if (std::string_view(shadername) != "Wireframe_DX9")
								//	{
								//		cmat->vftable->SetShader(cmat, "Wireframe");
								//		int x = 1;
								//	}*/
									int x = 1;
								}
							}
							else if (std::string_view(name).contains("props_destruction/glass_"))
							{
								IDirect3DBaseTexture9* aa = nullptr;
								dev->GetTexture(1, &aa);
								dev->SetTexture(0, aa);
							}
							else if (sname.contains("vacum_pipe_glass")) 
							{
								if (auto shadername = cmat->vftable->GetShaderName(cmat); shadername)
								{
									//if (std::string_view(shadername).contains("PortalRefract_dx9"))
									if (std::string_view(shadername) != "VertexLitGeneric")
									{
										// setting to VertexLitGeneric results in vertexformat '0xa0003' and gets rendered in 'cmeshdx8_renderpass_pre_draw'
										//cmat->vftable->SetShader(cmat, "VertexLitGeneric");

										int x = 1;
									}
									int x = 1;
								}
							}
						}
					}
				}
#endif

				
				dev->SetFVF(D3DFVF_XYZB3 | D3DFVF_NORMAL | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE1(3));
				dev->GetVertexShader(&ff_brushmodels::s_shader);
				dev->SetVertexShader(nullptr);
			}
			// somewhat broken
			else if (mesh->m_VertexFormat == 0xa0103) // glass shards
			{
				if (auto shaderapi = game::get_shaderapi(); shaderapi)
				{
					if (auto cmat = shaderapi->vtbl->GetBoundMaterial(shaderapi, nullptr); cmat)
					{
						if (auto name = cmat->vftable->GetName(cmat); name)
						{
							// blending is a shader thing I guess?
							//if (std::string_view(name).contains("portal_door_glass"))
							{
								// todo set unique texture
								dev->SetTexture(0, nullptr); //tex_portal_mask);

								if (auto shadername = cmat->vftable->GetShaderName(cmat); shadername)
								{
									//if (std::string_view(shadername).contains("PortalRefract_dx9"))
									if (std::string_view(shadername) != "VertexLitGeneric") 
									{
										// setting to VertexLitGeneric results in vertexformat '0xa0003' and gets rendered in 'cmeshdx8_renderpass_pre_draw'
										cmat->vftable->SetShader(cmat, "VertexLitGeneric");
										
										int x = 1;
									}
									int x = 1;
								}
								int yy = 1;
							}
						}
					}
				}

				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<D3DMATRIX*>(&mat.m));
				//dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2);
				dev->GetVertexShader(&ff_glass_shards::s_shader);
				dev->SetVertexShader(nullptr);
			}
			else
			{
				int xx = 1; 
			}
		}

		int zz = 1;
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
		//if (ent)
		{
			current_render_ent = ent;
		}
		
		int x = 1;
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

