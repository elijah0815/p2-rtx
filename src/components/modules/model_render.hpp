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

		namespace DrawBrushModelArray
		{
			constexpr uint32_t Index = 55u;
			using FN = void(__fastcall*)(void*, void*, void*, int, const BrushArrayInstanceData_t&, int);
			void __fastcall Detour(void* ecx, void* o1, void* o2, int nCount, const BrushArrayInstanceData_t& pInstanceData, int nModelTypeFlags);
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
		//// Access the singleton instance
		//static prim_fvf_context& inst() {
		//	static prim_fvf_context instance;
		//	return instance;
		//}

		//// Disable copying and assignment
		//prim_fvf_context(const prim_fvf_context&) = delete;
		//void operator=(const prim_fvf_context&) = delete;

		// save vertex shader
		void save_vs(IDirect3DDevice9* device)
		{
			device->GetVertexShader(&vertex_shader_);
		}

		// save texture at stage 0 or 1
		void save_texture(IDirect3DDevice9* device, const bool stage)
		{
			if (!stage)
			{
				device->GetTexture(0, &tex0_);
				tex0_set = true;
			}
			else
			{
				device->GetTexture(1, &tex1_);
				tex1_set = true;
			}
		}

		// save texture transform
		/*void save_texcoord_matrix(IDirect3DDevice9* device)
		{
			device->GetTransform(D3DTS_WORLD, &texcoord_matrix_);
		}*/

		// save render state (e.g. D3DRS_TEXTUREFACTOR)
		void save_rs(IDirect3DDevice9* device, const D3DRENDERSTATETYPE& state)
		{
			DWORD temp;
			device->GetRenderState(state, &temp);
			saved_render_state_[state] = temp;
		}

		// save texture stage 0 state (e.g. D3DTSS_ALPHAARG1)
		void save_tss(IDirect3DDevice9* device, const D3DTEXTURESTAGESTATETYPE& type)
		{
			DWORD temp;
			device->GetTextureStageState(0, type, &temp);
			saved_texture_stage_state_[type] = temp;
		}

		// restore vertex shader
		void restore_vs(IDirect3DDevice9* device)
		{
			device->SetVertexShader(vertex_shader_);
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

		// Restore world matrix
		/*void RestoreWorldMatrix(IDirect3DDevice9* device) {
			device->SetTransform(D3DTS_WORLD, &texcoord_matrix_);
		}*/

		// restore a specific render state (e.g. D3DRS_TEXTUREFACTOR)
		void restore_render_state(IDirect3DDevice9* device, const D3DRENDERSTATETYPE& state)
		{
			if (saved_render_state_.contains(state))
			{
				device->SetRenderState(state, saved_render_state_[state]);
			}
		}

		// restore a specific texture stage 0 state (e.g. D3DTSS_ALPHAARG1)
		void restore_texture_stage_state(IDirect3DDevice9* device, const D3DTEXTURESTAGESTATETYPE& type)
		{
			if (saved_texture_stage_state_.contains(type))
			{
				device->SetTextureStageState(0, type, saved_texture_stage_state_[type]);
			}
		}

		void restore_all(IDirect3DDevice9* device)
		{
			restore_vs(device);
			restore_texture(device, 0);
			restore_texture(device, 1);

			for (auto& rs : saved_render_state_)
			{
				device->SetRenderState(rs.first, rs.second);
			}

			for (auto& tss : saved_texture_stage_state_)
			{
				device->SetTextureStageState(0, tss.first, tss.second);
			}
		}

		// Reset the stored data
		void reset_context()
		{
			vertex_shader_ = nullptr;
			tex0_ = nullptr;
			tex1_ = nullptr;
			saved_render_state_.clear();
			saved_texture_stage_state_.clear();
			modifiers.reset();
		}

		struct modifiers_s
		{
			bool with_high_gamma = false;
			bool as_sky = false;
			bool as_transport_beam = false;
			bool dual_render_with_specified_texture = false;
			IDirect3DTexture9* dual_render_texture = nullptr;

			void reset()
			{
				with_high_gamma = false;
				as_sky = false;
				as_transport_beam = false;
				dual_render_with_specified_texture = false;
				dual_render_texture = nullptr;
			}
		};

		// special handlers for the next prim/s
		modifiers_s modifiers;

		// constructor for singleton
		prim_fvf_context() : vertex_shader_(nullptr), tex0_(nullptr), tex0_set(false), tex1_(nullptr), tex1_set(false) {}

	private:
		// Render states to save
		IDirect3DVertexShader9* vertex_shader_;
		IDirect3DBaseTexture9* tex0_;
		bool tex0_set;
		IDirect3DBaseTexture9* tex1_;
		bool tex1_set;
		//D3DMATRIX texcoord_matrix_;

		// store saved render states (with the type as the key)
		std::unordered_map<D3DRENDERSTATETYPE, DWORD> saved_render_state_;

		// store saved texture stage states (with type as the key)
		std::unordered_map<D3DTEXTURESTAGESTATETYPE, DWORD> saved_texture_stage_state_;
	};

	class model_render : public component
	{
	public:
		model_render();
		~model_render() = default;
		const char* get_name() override { return "model_render"; }

		static void init_texture_addons();

		static inline prim_fvf_context primctx {};

		static inline remixapi_MeshHandle portal0_mdl = nullptr;
		static inline remixapi_MaterialHandle portal0_mtl = nullptr;

		static inline remixapi_MeshHandle portal1_mdl = nullptr;
		static inline remixapi_MaterialHandle portal1_mtl = nullptr;

		static inline uint32_t portal1_render_count = 0;
		static inline uint32_t portal2_render_count = 0;

		static inline float portal1_open_amount = 0.0f;
		static inline bool  portal1_is_linked = false;
		static inline float portal2_open_amount = 0.0f;
		static inline bool  portal2_is_linked = false;

		static inline bool rendered_first_sky_surface = false;
	private:
	};
}
