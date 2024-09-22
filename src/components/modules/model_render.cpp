#include "std_include.hpp"

namespace components
{
	IDirect3DVertexShader9* og_model_shader = nullptr;

	void __fastcall table_hooks::DrawModelExecute::Detour(void* ecx, void* edx, void* oo, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		const auto dev = game::get_d3d_device();
		dev->GetVertexShader(&og_model_shader);
		dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));

		// MODELFLAG_MATERIALPROXY | MODELFLAG_STUDIOHDR_AMBIENT_BOOST
		if (pInfo.flags == 0x80000011)
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

		//else if (pInfo.flags != 9 && pInfo.flags != 2049 && pInfo.flags != 0x80000009 && pInfo.flags != 0x1)
		//{
		//	// 0x1 surv manager
		//	// 2049 other surv?

		//	VMatrix mat = {};
		//	mat.m[0][0] = pInfo.pModelToWorld->m_flMatVal[0][0];
		//	mat.m[1][0] = pInfo.pModelToWorld->m_flMatVal[0][1];
		//	mat.m[2][0] = pInfo.pModelToWorld->m_flMatVal[0][2];

		//	mat.m[0][1] = pInfo.pModelToWorld->m_flMatVal[1][0];
		//	mat.m[1][1] = pInfo.pModelToWorld->m_flMatVal[1][1];
		//	mat.m[2][1] = pInfo.pModelToWorld->m_flMatVal[1][2];

		//	mat.m[0][2] = pInfo.pModelToWorld->m_flMatVal[2][0];
		//	mat.m[1][2] = pInfo.pModelToWorld->m_flMatVal[2][1];
		//	mat.m[2][2] = pInfo.pModelToWorld->m_flMatVal[2][2];

		//	mat.m[3][0] = pInfo.pModelToWorld->m_flMatVal[0][3];
		//	mat.m[3][1] = pInfo.pModelToWorld->m_flMatVal[1][3];
		//	mat.m[3][2] = pInfo.pModelToWorld->m_flMatVal[2][3];
		//	mat.m[3][3] = game::identity[3][3];

		//	dev->SetTransform(D3DTS_WORLD, reinterpret_cast<D3DMATRIX*>(&mat.m)); 
		//}
		//else if (pInfo.flags == 2049 || pInfo.flags == 0x80000009 || pInfo.flags == 9)
		if (pInfo.pModel->radius == 37.3153992f) // models/weapons/v_portalgun.mdl
		{
			VMatrix mat = {};
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

			//D3DXMATRIX out = {};
			//D3DXMatrixMultiply(&out, reinterpret_cast<const D3DXMATRIX*>(&game::identity), reinterpret_cast<const D3DXMATRIX*>(mat.m));
			dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mat.m)); 

			//dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mat.m));


			//dev->SetVertexShader(nullptr);
			// this will render the viewmodel using shaders but its stuck at 0 0 0
			//BasePlayer::og_shader = nullptr;
		}

		table_hooks::table.original<FN>(Index)(ecx, edx, oo, state, pInfo, pCustomBoneToWorld);

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

	namespace ff_billboard
	{
		IDirect3DVertexShader9* s_shader = nullptr;
	}

	namespace ff_fxrelated
	{
		IDirect3DVertexShader9* s_shader = nullptr;
		IDirect3DBaseTexture9* s_texture;
	}

	IDirect3DVertexShader9* saved_shader_unk = nullptr;
	D3DMATRIX saved_world_mtx_unk = {};

	

	void cmeshdx8_renderpass_pre_draw(CMeshDX8* mesh)
	{
		const auto dev = game::get_d3d_device();

		IDirect3DVertexBuffer9* b = nullptr;
		UINT ofs = 0, stride = 0;
		dev->GetStreamSource(0, &b, &ofs, &stride);

 		if (og_model_shader && mesh->m_VertexFormat == 0xa0003 /*stride == 48*/) // player model - gun - grabable stuff
		{
			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX6); 
			dev->SetVertexShader(nullptr); // vertexformat 0x00000000000a0003

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
			mtx[3][2] = sinf((float)main_module::framecount * 0.05f) * 10.0f;
			mtx[3][3] = game::identity[3][3];
			dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));*/

		}
		else if (og_model_shader) // should be stride 30
		{
			dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3);
			dev->SetVertexShader(nullptr); // vertexformat 0x00000000000a0003

			D3DMATRIX mtx;
			dev->GetTransform(D3DTS_WORLD, &mtx);

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
			mtx[3][1] = game::identity[3][1];*/
			//mtx.m[3][2] = sinf((float)main_module::framecount * 0.05f) * 10.0f;
			//mtx[3][3] = game::identity[3][3];
			dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));

			int zz = 1;
		}
		else
		{
			// world geo? - floor / walls
			if (mesh->m_VertexFormat == 0x2480033) 
			{
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
				// tc @ 12
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(1)); // missing 4 bytes at the end here - fixed with tc2 size 3?
				dev->GetVertexShader(&ff_laserplatform::s_shader);
				dev->SetVertexShader(nullptr);

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

				mtx[3][0] = game::identity[3][0];
				mtx[3][1] = game::identity[3][1];
				mtx[3][2] = 0.0f; //sinf((float)main_module::framecount * 0.05f) * 40.0f;
				mtx[3][3] = game::identity[3][3];

				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity));

				dev->GetTransform(D3DTS_TEXTURE0, &ff_laserplatform::s_tc_transform);
				//dev->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, &laserplatform::s_tc_stage);

				// tc scroll
				D3DXMATRIX ret = ff_laserplatform::s_tc_transform;
				ret(3, 1) = (float)main_module::framecount * 0.01f;

				dev->SetTransform(D3DTS_TEXTURE0, &ret);
				dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			}

			// lasers
			else if (mesh->m_VertexFormat == 0x80003)
			{
				dev->GetVertexShader(&ff_laser::s_shader);
				bool swing = false;

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

				mtx[3][0] = game::identity[3][0];
				mtx[3][1] = game::identity[3][1];
				mtx[3][2] = swing ? (sinf((float)main_module::framecount * 0.05f) * 2.0f) : 0.0f;
				mtx[3][3] = game::identity[3][3];

				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));

				//dev->GetTransform(D3DTS_TEXTURE0, &laserplatform::s_tc_transform);
				//
				//// tc scroll
				//D3DXMATRIX ret = laserplatform::s_tc_transform;
				//ret(3, 1) = (float)main_module::framecount * 0.01f;

				//dev->SetTransform(D3DTS_TEXTURE0, &ret);
				//dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			}

			else if (mesh->m_VertexFormat == 0xa0007) // portal fx
			{
				dev->SetFVF(D3DFVF_XYZB1 | D3DFVF_NORMAL | D3DFVF_TEX5 | D3DFVF_TEXCOORDSIZE1(4)); // 68 - 4 as last tc is one float
				dev->GetVertexShader(&ff_billboard::s_shader);
				dev->SetVertexShader(nullptr);
				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&game::identity)); // 0x6c0005 influences this one here???

			}

			// no clue what 0x6c0005 is
			// same for 0x80007

			else if (mesh->m_VertexFormat == 0x4a0003 || mesh->m_VertexFormat == 0xa0003) // stuff behind portals - unstable
			{
				// tc @ 24
				dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64
				//dev->SetFVF(NULL);
				dev->GetVertexShader(&ff_fxrelated::s_shader);
				dev->SetVertexShader(nullptr);

				dev->GetTexture(0, &ff_fxrelated::s_texture);
				//dev->SetTexture(0, nullptr);

				
			}
			//else if (mesh->m_VertexFormat == 0x6c0005) // ??
			//{
			//	int zz = 1;
			//}
			//else if (mesh->m_VertexFormat == 0xa0003) // ??
			//{
			//	int xx = 0;
			//}
			else if (mesh->m_VertexFormat == 0x80007) // HUD
			{
				//dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX5); // 64
				//dev->SetFVF(NULL);
				//dev->GetVertexShader(&saved_shader_unk);
				//dev->SetVertexShader(nullptr);

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
				mtx[3][2] = (sinf((float)main_module::framecount * 0.05f) * 40.0f);
				mtx[3][3] = game::identity[3][3];

				dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));*/
			}
			//else if (mesh->m_VertexFormat == 0x92480005)
			//{
			//	int zz = 1;
			//}
			//else if (mesh->m_VertexFormat == 0x924900005)
			//{
			//	int zz = 1;
			//}

			// 0x924900005 on portal opening

			//else if (mesh->m_VertexFormat == 0x114900005 || mesh->m_VertexFormat == 0x914900005 || mesh->m_VertexFormat == 0x1b924900005 || mesh->m_VertexFormat == 0x80037
			//	/*|| mesh->m_VertexFormat == 0x80007*/ || mesh->m_VertexFormat == 0x6c0005 || mesh->m_VertexFormat == 0xa0003 || mesh->m_VertexFormat == 0x92480005) //
			//{
			//	dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX5); // 64
			//	//dev->SetFVF(NULL);
			//	dev->GetVertexShader(&saved_shader_unk);
			//	dev->SetVertexShader(nullptr);

			//	float mtx[4][4] = {};
			//	mtx[0][0] = game::identity[0][0];
			//	mtx[0][1] = game::identity[0][1];
			//	mtx[0][2] = game::identity[0][2];
			//	mtx[0][3] = game::identity[0][3];

			//	mtx[1][0] = game::identity[1][0];
			//	mtx[1][1] = game::identity[1][1];
			//	mtx[1][2] = game::identity[1][2];
			//	mtx[1][3] = game::identity[1][3];

			//	mtx[2][0] = game::identity[2][0];
			//	mtx[2][1] = game::identity[2][1];
			//	mtx[2][2] = game::identity[2][2];
			//	mtx[2][3] = game::identity[2][3];

			//	mtx[3][0] = game::identity[3][0];
			//	mtx[3][1] = game::identity[3][1];
			//	mtx[3][2] = (sinf((float)main_module::framecount * 0.05f) * 40.0f);
			//	mtx[3][3] = game::identity[3][3];

			//	dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&mtx));
			//}
			else
			{
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
			push	esi; // CMeshDX8
			call	cmeshdx8_renderpass_pre_draw;
			add		esp, 4;
			popad;

			// og code
			mov     ecx, [esi + 0x4C]; 
			test    ecx, ecx;
			jmp     cmeshdx8_renderpass_pre_draw_retn_addr;
		}
	}


	// #

	void cmeshdx8_renderpass_post_draw()
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

		if (ff_fxrelated::s_shader)
		{
			dev->SetVertexShader(ff_fxrelated::s_shader);
			dev->SetFVF(NULL);
			ff_fxrelated::s_shader = nullptr;

			dev->SetTexture(0, ff_fxrelated::s_texture);
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
			dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);//beamtransport::s_tc_stage);
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

		if (ff_billboard::s_shader)
		{
			dev->SetVertexShader(ff_billboard::s_shader);
			dev->SetFVF(NULL);
			ff_billboard::s_shader = nullptr;
		}
	}

	HOOK_RETN_PLACE_DEF(cmeshdx8_renderpass_post_draw_retn_addr);
	void __declspec(naked) cmeshdx8_renderpass_post_draw_stub()
	{
		__asm
		{
			mov     ecx, [esi + 0x50];
			push    ecx;
			push    eax;
			call    edx; // DrawIndexedPrimitive

			pushad;
			call	cmeshdx8_renderpass_post_draw;
			popad;

			jmp     cmeshdx8_renderpass_post_draw_retn_addr;
		}
	}



	model_render::model_render()
	{
		table_hooks::_interface = utils::module_interface.get<table_hooks::IVModelRender*>("engine.dll", "VEngineModel016");

		XASSERT(table_hooks::table.init(table_hooks::_interface) == false);
		XASSERT(table_hooks::table.hook(&table_hooks::DrawModelExecute::Detour, table_hooks::DrawModelExecute::Index) == false);

		utils::hook(RENDERER_BASE + 0xAD23, cmeshdx8_renderpass_pre_draw_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(cmeshdx8_renderpass_pre_draw_retn_addr, RENDERER_BASE + 0xAD28);

		utils::hook(RENDERER_BASE + 0xADF5, cmeshdx8_renderpass_post_draw_stub, HOOK_JUMP).install()->quick();
		HOOK_RETN_PLACE(cmeshdx8_renderpass_post_draw_retn_addr, RENDERER_BASE + 0xADFC);
	}
}
