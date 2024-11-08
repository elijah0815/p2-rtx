#pragma once

namespace components
{
	namespace tbl_hk::model_renderer
	{
		inline utils::vtable table;

		namespace DrawModelExecute
		{
			constexpr uint32_t Index = 19u;
			using FN = void(__fastcall*)(void*, void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
			void __fastcall Detour(void* ecx, void* edx, void* oo, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld);
		}

		typedef unsigned short ModelInstanceHandle_t;
		class IVModelRender
		{
		public:
			virtual int						DrawModel(int flags, IClientRenderable* pRenderable, ModelInstanceHandle_t instance, int entity_index, const model_t* model, Vector const& origin, Vector const& angles, int skin, int body, int hitboxset, const matrix3x4_t* modelToWorld = nullptr, const matrix3x4_t* pLightingOffset = nullptr) = 0;
			virtual void					ForcedMaterialOverride(IMaterial* newMaterial, OverrideType_t nOverrideType = OVERRIDE_NORMAL) = 0;
			virtual void					SetViewTarget(const CStudioHdr* pStudioHdr, int nBodyIndex, const Vector& target) = 0;
			virtual ModelInstanceHandle_t	CreateInstance(IClientRenderable* pRenderable, void* pCache = nullptr) = 0;
			virtual void					DestroyInstance(ModelInstanceHandle_t handle) = 0;
			virtual void					SetStaticLighting(ModelInstanceHandle_t handle, void* pHandle) = 0;
			virtual void*					GetStaticLighting(ModelInstanceHandle_t handle) = 0;
			virtual bool					ChangeInstance(ModelInstanceHandle_t handle, IClientRenderable* pRenderable) = 0;
			virtual void					AddDecal(ModelInstanceHandle_t handle, Ray_t const& ray, Vector const& decalUp, int decalIndex, int body, bool noPokeThru = false, int maxLODToDecal = ADDDECAL_TO_ALL_LODS) = 0;
			virtual void					RemoveAllDecals(ModelInstanceHandle_t handle) = 0;
			virtual void					RemoveAllDecalsFromAllModels() = 0;
			virtual matrix3x4_t*			DrawModelShadowSetup(IClientRenderable* pRenderable, int body, int skin, DrawModelInfo_t* pInfo, matrix3x4_t* pCustomBoneToWorld = nullptr) = 0;
			virtual void					DrawModelShadow(IClientRenderable* pRenderable, const DrawModelInfo_t& info, matrix3x4_t* pCustomBoneToWorld = nullptr) = 0;
			virtual bool					RecomputeStaticLighting(ModelInstanceHandle_t handle) = 0;
			virtual void					ReleaseAllStaticPropColorData(void) = 0;
			virtual void					RestoreAllStaticPropColorData(void) = 0;
			virtual int						DrawModelEx(ModelRenderInfo_t& pInfo) = 0;
			virtual int						DrawModelExStaticProp(ModelRenderInfo_t& pInfo) = 0;
			virtual bool					DrawModelSetup(ModelRenderInfo_t& pInfo, DrawModelState_t* pState, matrix3x4_t* pCustomBoneToWorld, matrix3x4_t** ppBoneToWorldOut) = 0;
			virtual void					DrawModelExecute(const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld = nullptr) = 0;
			virtual void					SetupLighting(const Vector& vecCenter) = 0;
			virtual int						DrawStaticPropArrayFast(StaticPropRenderInfo_t* pProps, int count, bool bShadowDepth) = 0;
			virtual void					SuppressEngineLighting(bool bSuppress) = 0;
			virtual void					SetupColorMeshes(int nTotalVerts) = 0;
			virtual void					SetupLightingEx(const Vector*, unsigned __int16);
			virtual bool					GetBrightestShadowingLightSource(const Vector*, Vector*, Vector*, bool);
			virtual void					ComputeLightingState(int, const void*, MaterialLightingState_t*, ITexture**); // LightingQuery_t
			virtual void					GetModelDecalHandles(StudioDecalHandle_t__**, int, int, const unsigned __int16*);
			virtual void					ComputeStaticLightingState(int, const void*, MaterialLightingState_t*, MaterialLightingState_t*, ColorMeshInfo_t**, ITexture**, memhandle_t__**); // StaticLightingQuery_t
			virtual void					CleanupStaticLightingState(int, memhandle_t__**);
		};

		inline IVModelRender* _interface = nullptr;
	}

	namespace tbl_hk::bmodel_renderer
	{
		inline utils::vtable table;

		namespace DrawBrushModelEx
		{
			constexpr uint32_t Index = 53u;
			using FN = void(__fastcall*)(void*, void*, IClientEntity*, model_t*, const Vector*, const QAngle*, DrawBrushModelMode_t);
			void __fastcall Detour(void* ecx, void* o1, IClientEntity* baseentity, model_t* model, const Vector* origin, const QAngle* angles, DrawBrushModelMode_t mode);
		}

		typedef unsigned short ModelInstanceHandle_t;
		class IVRenderView
		{
		public:
			virtual void					DrawBrushModel(IClientEntity*, model_t*, const Vector*, const QAngle*, bool) = 0;
			virtual void					DrawIdentityBrushModel(void*, model_t*) = 0; // IWorldRenderList
			virtual void					TouchLight(dlight_t*) = 0;
			virtual void					Draw3DDebugOverlays(void) = 0;
			virtual void					SetBlend(float) = 0;
			virtual float					GetBlend(void) = 0;
			virtual void					SetColorModulation(const float*) = 0;
			virtual void					GetColorModulation(float*) = 0;
			virtual void					SceneBegin(void) = 0;
			virtual void					SceneEnd(void) = 0;
			virtual void					GetVisibleFogVolume(const Vector*, const VisOverrideData_t*, VisibleFogVolumeInfo_t*) = 0;
			virtual void*					CreateWorldList(void) = 0; // ret IWorldRenderList
			virtual	void					BuildWorldLists(void*, WorldListInfo_t*, int, const VisOverrideData_t*, bool, float*) = 0; // IWorldRenderList
			virtual	void					DrawWorldLists(void*, void*, unsigned int, float) = 0;// IMatRenderContext - IWorldRenderList
			virtual	void					GetWorldListIndicesInfo(void*, void*, unsigned int) = 0; // WorldListIndicesInfo_t - IWorldRenderList
			virtual	void					DrawTopView(bool) = 0;
			virtual	void					TopViewBounds(const Vector2D*, const Vector2D*) = 0;
			virtual	void					DrawLights(void) = 0;
			virtual	void					DrawMaskEntities(void) = 0;
			virtual	void					DrawTranslucentSurfaces(void*, void*, int*, int, unsigned int) = 0; // IMatRenderContext - IWorldRenderList
			virtual	void					DrawLineFile(void) = 0;
			virtual	void					DrawLightmaps(void*, int) = 0; // IWorldRenderList
			virtual	void					ViewSetupVis(bool, int, const Vector*) = 0;
			virtual	bool					AreAnyLeavesVisible(int*, int) = 0;
			virtual	void					VguiPaint(void) = 0;
			virtual	void					ViewDrawFade(unsigned __int8*, IMaterial*) = 0;
			virtual	void					OLD_SetProjectionMatrix(float, float, float) = 0;
			virtual colorVec*				GetLightAtPoint(colorVec* result, Vector*) = 0;
			virtual int						GetViewEntity(void) = 0;
			virtual bool					IsViewEntity(int) = 0;
			virtual float					GetFieldOfView(void) = 0;
			virtual unsigned __int8**		GetAreaBits(void) = 0;
			virtual void					SetFogVolumeState(int, bool) = 0;
			virtual void					InstallBrushSurfaceRenderer(void*) = 0; // IBrushRenderer
			virtual void					DrawBrushModelShadow(IClientRenderable*) = 0;
			virtual bool					LeafContainsTranslucentSurfaces(void*, int, unsigned int) = 0; // IWorldRenderList
			virtual bool					DoesBoxIntersectWaterVolume(const Vector*, const Vector*, int) = 0;
			virtual void					SetAreaState(unsigned __int8*, unsigned __int8*) = 0;
			virtual void					VGui_Paint(int) = 0;
			virtual void					Push3DView(void*, const CViewSetup*, int, ITexture*, VPlane*, ITexture*) = 0; // IMatRenderContext
			virtual void					Push3DView(void*, const CViewSetup*, int, ITexture*, VPlane*) = 0; // IMatRenderContext
			virtual void					Push2DView(void*, const CViewSetup*, int, ITexture*, VPlane*) = 0; // IMatRenderContext
			virtual void					PopView(void*, VPlane*) = 0; // IMatRenderContext
			virtual void					SetMainView(const Vector*, const QAngle*) = 0;
			virtual void					ViewSetupVisEx(bool, int, const Vector*, unsigned int*) = 0;
			virtual void					OverrideViewFrustum(VPlane*) = 0;
			virtual void					DrawBrushModelShadowDepth(IClientEntity*, model_t*, const Vector*, const QAngle*, bool) = 0;
			virtual void					UpdateBrushModelLightmap(model_t*, IClientRenderable*) = 0;
			virtual void					BeginUpdateLightmaps(void) = 0;
			virtual void					EndUpdateLightmaps(void) = 0;
			virtual void					OLD_SetOffCenterProjectionMatrix(float, float, float, float, float, float, float, float) = 0;
			virtual void					OLD_SetProjectionMatrixOrtho(float, float, float, float, float, float) = 0;
			virtual void					GetMatricesForView(const CViewSetup*, VMatrix*, VMatrix*, VMatrix*, VMatrix*) = 0;
			virtual void					DrawBrushModelEx(IClientEntity*, model_t*, const Vector*, const QAngle*, DrawBrushModelMode_t) = 0;
			virtual bool					DoesBrushModelNeedPowerOf2Framebuffer(const model_t*) = 0;
			virtual void					DrawBrushModelArray(void*, int, const BrushArrayInstanceData_t*, int) = 0; // IMatRenderContext
		};

		inline IVRenderView* _interface = nullptr;
	}


	class prim_fvf_context {
	public:

		// retrieve information about the current pass - returns true if successful
		bool get_info_for_pass(IShaderAPIDX8* shaderapi)
		{
			if (shaderapi)
			{
				shaderapi->vtbl->GetBufferedState(shaderapi, nullptr, &info.buffer_state);

				if (info.material = shaderapi->vtbl->GetBoundMaterial(shaderapi, nullptr); 
					info.material)
				{
					info.material_name = info.material->vftable->GetName(info.material);
					info.shader_name = info.material->vftable->GetShaderName(info.material);

					return true;
				}
			}

			return false;
		}

		// set texture 0 transform
		void set_texture_transform(IDirect3DDevice9* device, const D3DXMATRIX* matrix)
		{
			if (matrix)
			{
				device->SetTransform(D3DTS_TEXTURE0, matrix);
				tex0_transform_set = true;
			}
		}

		// save vertex shader
		void save_vs(IDirect3DDevice9* device)
		{
			device->GetVertexShader(&vs_);
			vs_set = true;
		}

		// save texture at stage 0 or 1
		void save_texture(IDirect3DDevice9* device, const bool stage)
		{
			if (!stage)
			{
#if DEBUG
				if (tex0_set) {
					OutputDebugStringA("save_texture:: tex0 was already saved\n"); return;
				}
#endif

				device->GetTexture(0, &tex0_);
				tex0_set = true;
			}
			else
			{
#if DEBUG
				if (tex1_set) {
					OutputDebugStringA("save_texture:: tex1 was already saved\n"); return;
				}
#endif

				device->GetTexture(1, &tex1_);
				tex1_set = true;
			}
		}

		// save render state (e.g. D3DRS_TEXTUREFACTOR)
		void save_rs(IDirect3DDevice9* device, const D3DRENDERSTATETYPE& state)
		{
			DWORD temp;
			device->GetRenderState(state, &temp);
			saved_render_state_[state] = temp;
		}

		// save sampler state (D3DSAMPLERSTATETYPE)
		void save_ss(IDirect3DDevice9* device, const D3DSAMPLERSTATETYPE& state)
		{
			DWORD temp;
			device->GetSamplerState(0, state, &temp);
			saved_sampler_state_[state] = temp;
		}

		// save texture stage 0 state (e.g. D3DTSS_ALPHAARG1)
		void save_tss(IDirect3DDevice9* device, const D3DTEXTURESTAGESTATETYPE& type)
		{
			DWORD temp;
			device->GetTextureStageState(0, type, &temp);
			saved_texture_stage_state_[type] = temp;
		}

		//// save steamsource data
		//void save_streamsource_data(IDirect3DVertexBuffer9* buffer, UINT offset, UINT stride)
		//{
		//	streamsource_ = buffer;
		//	streamsource_offset_ = offset;
		//	streamsource_stride_ = stride;
		//}

		// restore vertex shader
		void restore_vs(IDirect3DDevice9* device)
		{
			if (vs_set)
			{
				device->SetVertexShader(vs_);
				vs_set = false;
			}
		}

		// restore texture at stage 0 or 1
		void restore_texture(IDirect3DDevice9* device, const bool stage)
		{
			if (!stage)
			{
				if (tex0_set)
				{
					device->SetTexture(0, tex0_);
					tex0_set = false;
				}
			}
			else
			{
				if (tex1_set)
				{
					device->SetTexture(1, tex1_);
					tex1_set = false;
				}
			}
		}

		// restore a specific render state (e.g. D3DRS_TEXTUREFACTOR)
		void restore_render_state(IDirect3DDevice9* device, const D3DRENDERSTATETYPE& state)
		{
			if (saved_render_state_.contains(state)) {
				device->SetRenderState(state, saved_render_state_[state]);
			}
		}

		// restore a specific sampler state (D3DSAMPLERSTATETYPE)
		void restore_sampler_state(IDirect3DDevice9* device, const D3DSAMPLERSTATETYPE& state)
		{
			if (saved_sampler_state_.contains(state)) {
				device->SetSamplerState(0, state, saved_sampler_state_[state]);
			}
		}

		// restore a specific texture stage 0 state (e.g. D3DTSS_ALPHAARG1)
		void restore_texture_stage_state(IDirect3DDevice9* device, const D3DTEXTURESTAGESTATETYPE& type)
		{
			if (saved_texture_stage_state_.contains(type)) {
				device->SetTextureStageState(0, type, saved_texture_stage_state_[type]);
			}
		}

		// restore texture 0 transform to identity
		void restore_texture_transform(IDirect3DDevice9* device)
		{
			device->SetTransform(D3DTS_TEXTURE0, &game::IDENTITY);
			tex0_transform_set = false;
		}

		// restore all changes
		void restore_all(IDirect3DDevice9* device)
		{
			restore_vs(device);
			restore_texture(device, 0);
			restore_texture(device, 1);
			restore_texture_transform(device);

			for (auto& rs : saved_render_state_) {
				device->SetRenderState(rs.first, rs.second);
			}

			for (auto& ss : saved_sampler_state_) {
				device->SetSamplerState(0, ss.first, ss.second);
			}

			for (auto& tss : saved_texture_stage_state_) {
				device->SetTextureStageState(0, tss.first, tss.second);
			}
		}

		// reset the stored context data
		void reset_context()
		{
			vs_ = nullptr; vs_set = false;
			tex0_ = nullptr; tex0_set = false;
			tex1_ = nullptr; tex1_set = false;
			tex0_transform_set = false;
			saved_render_state_.clear();
			saved_sampler_state_.clear();
			saved_texture_stage_state_.clear();
			modifiers.reset();
			info.reset();
		}

		struct modifiers_s
		{
			bool do_not_render = false;
			bool with_high_gamma = false;
			bool as_sky = false;
			bool as_water = false;
			bool as_transport_beam = false;
			bool as_emancipation_grill = false;
			Vector2D emancipation_offset = {};
			Vector2D emancipation_scale = { 1.0f, 1.0f };
			float emancipation_color_scale = 1.0f;
			bool dual_render_with_basetexture2 = false; // render prim a second time with tex2 set as tex1
			bool dual_render_with_specified_texture = false; // render prim a second time with tex defined in 'dual_render_texture'
			bool dual_render_with_specified_texture_blend_add = false; // renders second prim using blend mode ADD
			IDirect3DBaseTexture9* dual_render_texture = nullptr;

			void reset()
			{
				do_not_render = false;
				with_high_gamma = false;
				as_sky = false;
				as_water = false;
				as_transport_beam = false;
				as_emancipation_grill = false;
				emancipation_offset = { 0.0f, 0.0f };
				emancipation_scale = { 1.0f, 1.0f };
				emancipation_color_scale = 1.0f;
				dual_render_with_basetexture2 = false;
				dual_render_with_specified_texture = false;
				dual_render_texture = nullptr;
			}
		};

		// special handlers for the next prim/s
		modifiers_s modifiers;

		struct info_s
		{
			IMaterialInternal* material = nullptr;
			std::string_view material_name;
			std::string_view shader_name;
			BufferedState_t buffer_state {};

			void reset()
			{
				material = nullptr;
				material_name = "";
				shader_name = "";
				memset(&buffer_state, 0, sizeof(BufferedState_t));
			}
		};

		// holds information about the current pass
		// use 'get_info_for_pass()' to populate struct
		info_s info;

		// constructor for singleton
		prim_fvf_context() = default;

	private:
		// Render states to save
		IDirect3DVertexShader9* vs_ = nullptr;
		bool vs_set = false;
		IDirect3DBaseTexture9* tex0_ = nullptr;
		bool tex0_set = false;
		IDirect3DBaseTexture9* tex1_ = nullptr;
		bool tex1_set = false;
		bool tex0_transform_set = false;

		// store saved render states (with the type as the key)
		std::unordered_map<D3DRENDERSTATETYPE, DWORD> saved_render_state_;

		// store saved render states (with the type as the key)
		std::unordered_map<D3DSAMPLERSTATETYPE, DWORD> saved_sampler_state_;

		// store saved texture stage states (with type as the key)
		std::unordered_map<D3DTEXTURESTAGESTATETYPE, DWORD> saved_texture_stage_state_;
	};

	namespace tex_addons
	{
		extern LPDIRECT3DTEXTURE9 portal_mask;
		extern LPDIRECT3DTEXTURE9 portal_blue;
		extern LPDIRECT3DTEXTURE9 portal_blue_overlay;
		extern LPDIRECT3DTEXTURE9 portal_blue_closed;
		extern LPDIRECT3DTEXTURE9 portal_orange;
		extern LPDIRECT3DTEXTURE9 portal_orange_overlay;
		extern LPDIRECT3DTEXTURE9 portal_orange_closed;
		extern LPDIRECT3DTEXTURE9 glass_shards;
		extern LPDIRECT3DTEXTURE9 glass_window_lamps;
		extern LPDIRECT3DTEXTURE9 glass_window_observ;
		extern LPDIRECT3DTEXTURE9 black_shader;
		extern LPDIRECT3DTEXTURE9 blue_laser_dualrender;
		extern LPDIRECT3DTEXTURE9 sky_gray_ft;
		extern LPDIRECT3DTEXTURE9 sky_gray_bk;
		extern LPDIRECT3DTEXTURE9 sky_gray_lf;
		extern LPDIRECT3DTEXTURE9 sky_gray_rt;
		extern LPDIRECT3DTEXTURE9 sky_gray_up;
		extern LPDIRECT3DTEXTURE9 sky_gray_dn;
		extern LPDIRECT3DTEXTURE9 emancipation_grill;
		extern LPDIRECT3DTEXTURE9 white;
	}

	class model_render : public component
	{
	public:
		model_render();
		~model_render() = default;

		static void init_texture_addons(bool release = false);

		static inline prim_fvf_context primctx {};

#if defined(BENCHMARK)
		struct benchmark_data
		{
			bool enabled = false;

			float ms = 0.0f;
			float ms_total = 0.0f;
			std::string material_name;
			std::uint64_t vertex_format = 0u;

			void clear()
			{
				ms = 0.0f;
				ms_total = 0.0f;
				material_name = "";
				vertex_format = 0u;
			}
		};

		static inline benchmark_data m_benchmark = {};
#endif

		static inline float vgui_progress_board_scalar = 1.0f;

		static inline float portal1_open_amount = 0.0f;
		static inline bool  portal1_is_linked = false;
		static inline const C_Prop_Portal* portal1_ptr = nullptr;

		static inline float portal2_open_amount = 0.0f;
		static inline bool  portal2_is_linked = false;
		static inline const C_Prop_Portal* portal2_ptr = nullptr;

		static inline std::vector<CPortalRenderable_FlatBasic*> linked_area_portals;
	};
}
