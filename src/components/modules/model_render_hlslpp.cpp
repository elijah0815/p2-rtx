#include "std_include.hpp"

namespace components
{
	// Fixes sprite trail verts by changing verts within the vertexbuffer
	// Expensive - use with caution. A drawcall with lots of verts is okay. A lot of smaller ones are not
	void model_render_hlslpp::fix_sprite_trail_particles(prim_fvf_context& ctx, CPrimList* primlist)
	{
		using namespace hlslpp;
		const auto dev = game::get_d3d_device();

		ctx.save_tss(dev, D3DTSS_ALPHAARG1);
		ctx.save_tss(dev, D3DTSS_ALPHAARG2);
		ctx.save_tss(dev, D3DTSS_ALPHAOP);
		dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

		//ctx.modifiers.do_not_render = true; 
		//lookat_vertex_decl(dev, primlist);

#if 1
		auto CatmullRomSpline = [](const float4& a, const float4& b, const float4& c, const float4& d, const float t)
			{
				return b + 0.5f * t * (c - a + t * (2.0f * a - 5.0f * b + 4.0f * c - d + t * (-a + 3.0f * b - 3.0f * c + d)));
			};

		auto DCatmullRomSpline3 = [](const float4& aa, const float4& bb, const float4& cc, const float4& dd, const float t)
			{
				float3 a = aa.xyz; float3 b = bb.xyz; float3 c = cc.xyz; float3 d = dd.xyz;
				return 0.5f * (c - a + t * (2.0f * a - 5 * b + 4 * c - d + t * (3.0f * b - a - 3.0f * c + d))
					+ t * (2.0f * a - 5.0f * b + 4 * c - d + 2.0f * (t * (3 * b - a - 3.0f * c + d))));
			};

		float ALPHATFADE, RADIUSTFADE;
		{
			float v[4] = {}; dev->GetVertexShaderConstantF(57, v, 1);
			ALPHATFADE = v[2];
			RADIUSTFADE = v[3];
		}

		float3 eyePos;
		{
			float v[4] = {}; dev->GetVertexShaderConstantF(2, v, 1);
			eyePos = float3(v[0], v[1], v[2]);
		}

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
					for (auto i = 0u; i < (std::uint32_t)primlist->m_NumIndices; i++) {
						indices.insert(static_cast<std::uint16_t*>(ib_data)[primlist->m_FirstIndex + i]);
					} ib->Unlock();

					// get the range of vertices that we are going to work with
					UINT min_vert = 0u, max_vert = 0u;
					{
						auto [min_it, max_it] = std::minmax_element(indices.begin(), indices.end());
						min_vert = *min_it; max_vert = *max_it;
					}

					void* src_buffer_data; // lock vertex buffer from first used vertex (in total bytes) to X used vertices (in total bytes)
					if (SUCCEEDED(vb->Lock(min_vert * t_stride, max_vert * t_stride, &src_buffer_data, 0)))
					{
						// all hlslpp floats are 16 bytes :(
						struct src_vert {
							Vector vParms; D3DCOLOR vTint; float4 vSplinePt0; float4 vSplinePt1; float4 vSplinePt2; float4 vSplinePt3; float4 vTexCoordRange; float4 vEndPointColor;
						};

						struct dest_vert {
							Vector pos; D3DCOLOR vTint; Vector2D tc;
						};

						for (auto i : indices)
						{
							// we need to subtract min_vert because we locked @ min_vert which is the start of our lock
							i -= static_cast<std::uint16_t>(min_vert);

							const auto v_pos_in_src_buffer = i * t_stride;
							const auto src = reinterpret_cast<src_vert*>(((DWORD)src_buffer_data + v_pos_in_src_buffer));
							const auto dest = reinterpret_cast<dest_vert*>(src);

							// save vParms (because we will be overriding them when writing pos)
							const float parmsX = src->vParms.x;
							const float parmsY = src->vParms.y;
							const float parmsZ = src->vParms.z;

							float4 posrad = CatmullRomSpline(src->vSplinePt0, src->vSplinePt1, src->vSplinePt2, src->vSplinePt3, parmsX);
							posrad.w *= std::lerp(1.0f, RADIUSTFADE, parmsX);

							// screen aligned
							float3 v2p = posrad.xyz - eyePos.xyz;
							float3 tangent = DCatmullRomSpline3(src->vSplinePt0, src->vSplinePt1, src->vSplinePt2, src->vSplinePt3, parmsX);
							float3 ofs = normalize(cross(v2p, normalize(tangent)));

							posrad.xyz += (ofs * (posrad.w * (parmsZ - 0.5f)));
							posrad.w = 1.0f;

							// write position
							dest->pos.FromFloat3(posrad.xyz);

							// write texcoords
							dest->tc.FromFloat2(float2(lerp(src->vTexCoordRange.z, src->vTexCoordRange.x, parmsZ), lerp(src->vTexCoordRange.y, src->vTexCoordRange.w, parmsY)));

							// write color
							dest->vTint = D3DCOLOR_COLORVALUE(src->vEndPointColor.r, src->vEndPointColor.g, src->vEndPointColor.b, src->vEndPointColor.a * std::lerp(1.0f, ALPHATFADE, parmsX));
						}
						vb->Unlock();
					}
				}
			}
		}
#endif
	}

	model_render_hlslpp::model_render_hlslpp()
	{
		
	}
}
