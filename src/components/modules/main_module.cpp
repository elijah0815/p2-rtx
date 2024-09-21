#include "std_include.hpp"

// -novid -disable_d3d9_hacks -limitvsconst -disallowhwmorph -no_compressed_verts -maxdxlevel 90 -dxlevel 90 +sv_cheats 1 +mat_fullbright 1  +mat_queue_mode 0 +mat_softwarelighting 1 +mat_softwareskin 1 +mat_phong 1 +mat_parallaxmap 0 +mat_frame_sync_enable 0 +mat_fastnobump 1 +mat_disable_bloom 1 +mat_dof_enabled 0 +mat_displacementmap 0 +mat_drawflat 1 +mat_normalmaps 0 +mat_normals 0 +sv_lan 1 +map devtest

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

	const float identity[4][4] =
	{
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};

	// CViewRender::RenderView
	void xx(/*void* renderer*/)
	{
		void* enginerender = reinterpret_cast<void*>(ENGINE_BASE + 0x60F880);

		const VMatrix* view = call_virtual<12, const VMatrix*>(enginerender);

		// Increment the pointer to get to the projectionMatrix
		const VMatrix* pProjectionMatrix = view + 1;

		//static DWORD* backEndDataOut_ptr = (DWORD*)(0x174F970);  // backendEndDataOut pointer
		//const auto out = reinterpret_cast<game::GfxBackEndData*>(*game::backEndDataOut_ptr);
		IDirect3DDevice9* dev = reinterpret_cast<IDirect3DDevice9*>(*(DWORD*)(RENDERER_BASE + 0x179F38));

		float colView[4][4] = {};
		rowMajorToColumnMajor(view->m[0], colView[0]);

		float colProj[4][4] = {};
		rowMajorToColumnMajor(pProjectionMatrix->m[0], colProj[0]);

		dev->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&identity));
		dev->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(colView));
		dev->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(colProj));

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

	main_module::main_module()
	{
		/*HOOK_RETN_PLACE(pl_view_stub_retn, CLIENT_BASE + 0x4DA02);
		utils::hook(CLIENT_BASE + 0x4D9FD, calc_player_view_stub).install()->quick();*/

		// works but no gunmodel and broken stuff
#ifdef XXX
		HOOK_RETN_PLACE(xxx_retn, ENGINE_BASE + 0xE6CFB);
		utils::hook(ENGINE_BASE + 0xE6CF6, xxx_stub).install()->quick();
#endif

		HOOK_RETN_PLACE(yyy_retn, CLIENT_BASE + 0x1ECB8A);
		utils::hook(CLIENT_BASE + 0x1ECB85, yyy_stub).install()->quick();
	}
}
