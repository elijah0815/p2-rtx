#pragma once
#include "utils/vector.h"

namespace components
{
	enum LightType_t : int
	{
		MATERIAL_LIGHT_DISABLE = 0x0,
		MATERIAL_LIGHT_POINT = 0x1,
		MATERIAL_LIGHT_DIRECTIONAL = 0x2,
		MATERIAL_LIGHT_SPOT = 0x3,
	};

	enum VertexCompressionType_t : int
	{
		VERTEX_COMPRESSION_INVALID = 0xFFFFFFFF,
		VERTEX_COMPRESSION_NONE = 0x0,
		VERTEX_COMPRESSION_ON = 0x1,
	};

	enum ShadowType_t : int
	{
		SHADOWS_NONE = 0x0,
		SHADOWS_SIMPLE = 0x1,
		SHADOWS_RENDER_TO_TEXTURE = 0x2,
		SHADOWS_RENDER_TO_TEXTURE_DYNAMIC = 0x3,
		SHADOWS_RENDER_TO_DEPTH_TEXTURE = 0x4,
		SHADOWS_RENDER_TO_TEXTURE_DYNAMIC_CUSTOM = 0x5,
	};

	enum MaterialVarFlags_t : int
	{
		MATERIAL_VAR_DEBUG = 0x1,
		MATERIAL_VAR_NO_DEBUG_OVERRIDE = 0x2,
		MATERIAL_VAR_NO_DRAW = 0x4,
		MATERIAL_VAR_USE_IN_FILLRATE_MODE = 0x8,
		MATERIAL_VAR_VERTEXCOLOR = 0x10,
		MATERIAL_VAR_VERTEXALPHA = 0x20,
		MATERIAL_VAR_SELFILLUM = 0x40,
		MATERIAL_VAR_ADDITIVE = 0x80,
		MATERIAL_VAR_ALPHATEST = 0x100,
		MATERIAL_VAR_PSEUDO_TRANSLUCENT = 0x200,
		MATERIAL_VAR_ZNEARER = 0x400,
		MATERIAL_VAR_MODEL = 0x800,
		MATERIAL_VAR_FLAT = 0x1000,
		MATERIAL_VAR_NOCULL = 0x2000,
		MATERIAL_VAR_NOFOG = 0x4000,
		MATERIAL_VAR_IGNOREZ = 0x8000,
		MATERIAL_VAR_DECAL = 0x10000,
		MATERIAL_VAR_ENVMAPSPHERE = 0x20000,
		MATERIAL_VAR_ENVMAPCAMERASPACE = 0x80000,
		MATERIAL_VAR_BASEALPHAENVMAPMASK = 0x100000,
		MATERIAL_VAR_TRANSLUCENT = 0x200000,
		MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = 0x400000,
		MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = 0x800000,
		MATERIAL_VAR_OPAQUETEXTURE = 0x1000000,
		MATERIAL_VAR_ENVMAPMODE = 0x2000000,
		MATERIAL_VAR_SUPPRESS_DECALS = 0x4000000,
		MATERIAL_VAR_HALFLAMBERT = 0x8000000,
		MATERIAL_VAR_WIREFRAME = 0x10000000,
		MATERIAL_VAR_ALLOWALPHATOCOVERAGE = 0x20000000,
		MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = 0x40000000,
		MATERIAL_VAR_VERTEXFOG = 0x80000000,
	};

	enum MaterialPropertyTypes_t : int
	{
		MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0x0,
		MATERIAL_PROPERTY_OPACITY = 0x1,
		MATERIAL_PROPERTY_REFLECTIVITY = 0x2,
		MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS = 0x3,
	};

	enum PreviewImageRetVal_t : int
	{
		MATERIAL_PREVIEW_IMAGE_BAD = 0x0,
		MATERIAL_PREVIEW_IMAGE_OK = 0x1,
		MATERIAL_NO_PREVIEW_IMAGE = 0x2,
	};

	enum MaterialPrimitiveType_t : __int32
	{
		MATERIAL_POINTS = 0x0,
		MATERIAL_LINES = 0x1,
		MATERIAL_TRIANGLES = 0x2,
		MATERIAL_TRIANGLE_STRIP = 0x3,
		MATERIAL_LINE_STRIP = 0x4,
		MATERIAL_LINE_LOOP = 0x5,
		MATERIAL_POLYGON = 0x6,
		MATERIAL_QUADS = 0x7,
		MATERIAL_SUBD_QUADS_EXTRA = 0x8,
		MATERIAL_SUBD_QUADS_REG = 0x9,
		MATERIAL_INSTANCED_QUADS = 0xA,
		MATERIAL_HETEROGENOUS = 0xB,
	};

	enum ImageFormat : int
	{
		IMAGE_FORMAT_DEFAULT = 0xFFFFFFFE,
		IMAGE_FORMAT_UNKNOWN = 0xFFFFFFFF,
		IMAGE_FORMAT_RGBA8888 = 0x0,
		IMAGE_FORMAT_ABGR8888 = 0x1,
		IMAGE_FORMAT_RGB888 = 0x2,
		IMAGE_FORMAT_BGR888 = 0x3,
		IMAGE_FORMAT_RGB565 = 0x4,
		IMAGE_FORMAT_I8 = 0x5,
		IMAGE_FORMAT_IA88 = 0x6,
		IMAGE_FORMAT_P8 = 0x7,
		IMAGE_FORMAT_A8 = 0x8,
		IMAGE_FORMAT_RGB888_BLUESCREEN = 0x9,
		IMAGE_FORMAT_BGR888_BLUESCREEN = 0xA,
		IMAGE_FORMAT_ARGB8888 = 0xB,
		IMAGE_FORMAT_BGRA8888 = 0xC,
		IMAGE_FORMAT_DXT1 = 0xD,
		IMAGE_FORMAT_DXT3 = 0xE,
		IMAGE_FORMAT_DXT5 = 0xF,
		IMAGE_FORMAT_BGRX8888 = 0x10,
		IMAGE_FORMAT_BGR565 = 0x11,
		IMAGE_FORMAT_BGRX5551 = 0x12,
		IMAGE_FORMAT_BGRA4444 = 0x13,
		IMAGE_FORMAT_DXT1_ONEBITALPHA = 0x14,
		IMAGE_FORMAT_BGRA5551 = 0x15,
		IMAGE_FORMAT_UV88 = 0x16,
		IMAGE_FORMAT_UVWQ8888 = 0x17,
		IMAGE_FORMAT_RGBA16161616F = 0x18,
		IMAGE_FORMAT_RGBA16161616 = 0x19,
		IMAGE_FORMAT_UVLX8888 = 0x1A,
		IMAGE_FORMAT_R32F = 0x1B,
		IMAGE_FORMAT_RGB323232F = 0x1C,
		IMAGE_FORMAT_RGBA32323232F = 0x1D,
		IMAGE_FORMAT_RG1616F = 0x1E,
		IMAGE_FORMAT_RG3232F = 0x1F,
		IMAGE_FORMAT_RGBX8888 = 0x20,
		IMAGE_FORMAT_NULL = 0x21,
		IMAGE_FORMAT_ATI2N = 0x22,
		IMAGE_FORMAT_ATI1N = 0x23,
		IMAGE_FORMAT_RGBA1010102 = 0x24,
		IMAGE_FORMAT_BGRA1010102 = 0x25,
		IMAGE_FORMAT_R16F = 0x26,
		IMAGE_FORMAT_D16 = 0x27,
		IMAGE_FORMAT_D15S1 = 0x28,
		IMAGE_FORMAT_D32 = 0x29,
		IMAGE_FORMAT_D24S8 = 0x2A,
		IMAGE_FORMAT_LINEAR_D24S8 = 0x2B,
		IMAGE_FORMAT_D24X8 = 0x2C,
		IMAGE_FORMAT_D24X4S4 = 0x2D,
		IMAGE_FORMAT_D24FS8 = 0x2E,
		IMAGE_FORMAT_D16_SHADOW = 0x2F,
		IMAGE_FORMAT_D24X8_SHADOW = 0x30,
		IMAGE_FORMAT_LINEAR_BGRX8888 = 0x31,
		IMAGE_FORMAT_LINEAR_RGBA8888 = 0x32,
		IMAGE_FORMAT_LINEAR_ABGR8888 = 0x33,
		IMAGE_FORMAT_LINEAR_ARGB8888 = 0x34,
		IMAGE_FORMAT_LINEAR_BGRA8888 = 0x35,
		IMAGE_FORMAT_LINEAR_RGB888 = 0x36,
		IMAGE_FORMAT_LINEAR_BGR888 = 0x37,
		IMAGE_FORMAT_LINEAR_BGRX5551 = 0x38,
		IMAGE_FORMAT_LINEAR_I8 = 0x39,
		IMAGE_FORMAT_LINEAR_RGBA16161616 = 0x3A,
		IMAGE_FORMAT_LE_BGRX8888 = 0x3B,
		IMAGE_FORMAT_LE_BGRA8888 = 0x3C,
		NUM_IMAGE_FORMATS = 0x3D,
	};

	enum MaterialIndexFormat_t : int
	{
		MATERIAL_INDEX_FORMAT_UNKNOWN = 0xFFFFFFFF,
		MATERIAL_INDEX_FORMAT_16BIT = 0x0,
		MATERIAL_INDEX_FORMAT_32BIT = 0x1,
	};

	enum modtype_t : std::int32_t
	{
		mod_bad = 0x0,
		mod_brush = 0x1,
		mod_sprite = 0x2,
		mod_studio = 0x3,
	};

	enum OverrideType_t : int
	{
		OVERRIDE_NORMAL = 0x0,
		OVERRIDE_BUILD_SHADOWS = 0x1,
		OVERRIDE_DEPTH_WRITE = 0x2,
	};

	enum $D6791CFE666A571E1681338A952F9E69 : int
	{
		ADDDECAL_TO_ALL_LODS = 0xFFFFFFFF,
	};

	struct matrix3x4_t
	{
		float m_flMatVal[3][4];
	};

	struct VMatrix
	{
		float m[4][4];
	};

	struct VPlane
	{
		Vector m_Normal;
		float m_Dist;
	};

	struct QAngle
	{
		float x;
		float y;
		float z;
	};

	struct CUtlSymbol
	{
		unsigned __int16 m_Id;
	};

	struct memhandle_t__
	{
		int unused;
	};

	/*struct __declspec(align(2)) LightingQuery_t
	{
		Vector m_LightingOrigin;
		unsigned __int16 m_InstanceHandle;
		bool m_bAmbientBoost;
	};*/

	struct __declspec(align(16)) Ray_t
	{
		VectorAligned m_Start;
		VectorAligned m_Delta;
		VectorAligned m_StartOffset;
		VectorAligned m_Extents;
		const matrix3x4_t* m_pWorldAxisTransform;
		bool m_IsRay;
		bool m_IsSwept;
	};

	union $123B8C34E442A509E2700B23F73AA3E7
	{
		int m_iValue;
		float m_flValue;
		void* m_pValue;
		unsigned __int8 m_Color[4];
	};

	struct KeyValues
	{
		unsigned __int32 m_iKeyName : 24;
		unsigned __int32 m_iKeyNameCaseSensitive1 : 8;
		char* m_sValue;
		wchar_t* m_wsValue;
		$123B8C34E442A509E2700B23F73AA3E7 u4;
		char m_iDataType;
		char m_bHasEscapeSequences;
		unsigned __int16 m_iKeyNameCaseSensitive2;
		KeyValues* m_pPeer;
		KeyValues* m_pSub;
		KeyValues* m_pChain;
		bool(__cdecl* m_pExpressionGetSymbolProc)(const char*);
	};

	struct cplane_t
	{
		Vector normal;
		float dist;
		unsigned __int8 type;
		unsigned __int8 signbits;
		unsigned __int8 pad[2];
	};

	struct mleaf_t
	{
		int contents;
		int visframe;
		void* parent; // mnode_t
		__int16 area;
		__int16 flags;
		VectorAligned m_vecCenter;
		VectorAligned m_vecHalfDiagonal;
		__int16 cluster;
		__int16 leafWaterDataID;
		unsigned __int16 firstmarksurface;
		unsigned __int16 nummarksurfaces;
		__int16 nummarknodesurfaces;
		__int16 unused;
		unsigned __int16 dispListStart;
		unsigned __int16 dispCount;
	};

	struct mleafwaterdata_t
	{
		float surfaceZ;
		float minZ;
		__int16 surfaceTexInfoID;
		__int16 firstLeafIndex;
	};

	struct mvertex_t
	{
		Vector position;
	};

	struct doccluderdata_t
	{
		int flags;
		int firstpoly;
		int polycount;
		Vector mins;
		Vector maxs;
		int area;
	};

	struct doccluderpolydata_t
	{
		int firstvertexindex;
		int vertexcount;
		int planenum;
	};

	struct mtexinfo_t
	{
		Vector4D textureVecsTexelsPerWorldUnits[2];
		Vector4D lightmapVecsLuxelsPerWorldUnits[2];
		float luxelsPerWorldUnit;
		float worldUnitsPerLuxel;
		unsigned __int16 flags;
		unsigned __int16 texinfoFlags;
		void* material; // IMaterial
	};

	struct csurface_t
	{
		const char* name;
		__int16 surfaceProps;
		unsigned __int16 flags;
	};

	struct msurfacenormal_t
	{
		unsigned int firstvertnormal;
	};

	struct dfacebrushlist_t
	{
		unsigned __int16 m_nFaceBrushCount;
		unsigned __int16 m_nFaceBrushStart;
	};

	struct mprimitive_t
	{
		int type;
		unsigned __int16 firstIndex;
		unsigned __int16 indexCount;
		unsigned __int16 firstVert;
		unsigned __int16 vertCount;
	};

	struct mprimvert_t
	{
		Vector pos;
		float texCoord[2];
		float lightCoord[2];
	};

	struct spritedata_t
	{
		int numframes;
		int width;
		int height;
		void* sprite; // CEngineSprite
	};

	struct worldbrushdata_t
	{
		int numsubmodels;
		int nWorldFaceCount;
		int numplanes;
		cplane_t* planes;
		int numleafs;
		mleaf_t* leafs;
		int numleafwaterdata;
		mleafwaterdata_t* leafwaterdata;
		int numvertexes;
		mvertex_t* vertexes;
		int numoccluders;
		doccluderdata_t* occluders;
		int numoccluderpolys;
		doccluderpolydata_t* occluderpolys;
		int numoccludervertindices;
		int* occludervertindices;
		int numvertnormalindices;
		unsigned __int16* vertnormalindices;
		int numvertnormals;
		Vector* vertnormals;
		int numnodes;
		void* nodes; // mnode_t
		unsigned __int16* m_LeafMinDistToWater;
		int numtexinfo;
		mtexinfo_t* texinfo;
		int numtexdata;
		csurface_t* texdata;
		int numDispInfos;
		void* hDispInfos;
		int numsurfaces;
		void* surfaces1; // msurface1_t
		void* surfaces2; // msurface2_t
		void* surfacelighting; // msurfacelighting_t
		msurfacenormal_t* surfacenormals;
		unsigned __int16* m_pSurfaceBrushes;
		dfacebrushlist_t* m_pSurfaceBrushList;
		int numvertindices;
		unsigned __int16* vertindices;
		int nummarksurfaces;
		void** marksurfaces; // msurface2_t
		void* lightdata; // ColorRGBExp32
		int m_nLightingDataSize;
		int numworldlights;
		void* worldlights; // dworldlight_t
		void* shadowzbuffers;
		int numprimitives;
		mprimitive_t* primitives;
		int numprimverts;
		mprimvert_t* primverts;
		int numprimindices;
		unsigned __int16* primindices;
		int m_nAreas;
		void* m_pAreas; // darea_t
		int m_nAreaPortals;
		void* m_pAreaPortals; // dareaportal_t
		int m_nClipPortalVerts;
		Vector* m_pClipPortalVerts;
		void* m_pCubemapSamples; // mcubemapsample_t
		int m_nCubemapSamples;
		int m_nDispInfoReferences;
		unsigned __int16* m_pDispInfoReferences;
		void* m_pLeafAmbient; // dleafambientindex_t
		void* m_pAmbientSamples; // dleafambientlighting_t
		bool m_bUnloadedAllLightmaps;
		void* m_pLightingDataStack; // CMemoryStack
		int m_nBSPFileSize;
	};

	struct brushdata_t
	{
		worldbrushdata_t* pShared;
		int firstmodelsurface;
		int nummodelsurfaces;
		int nLightstyleLastComputedFrame;
		unsigned __int16 nLightstyleIndex;
		unsigned __int16 nLightstyleCount;
		unsigned __int16 renderHandle;
		unsigned __int16 firstnode;
	};

	union $827FC955A655E715E2ACE31D316F483B
	{
		brushdata_t brush;
		unsigned __int16 studio;
		spritedata_t sprite;
	};

	struct model_t
	{
		void* fnHandle;
		char szPathName[260];
		int nLoadFlags;
		int nServerCount;
		modtype_t type;
		int flags;
		Vector mins;
		Vector maxs;
		float radius;
		KeyValues* m_pKeyValues;
		$827FC955A655E715E2ACE31D316F483B ___u10;
	};

	struct studiohdr_t
	{
		int id;
		int version;
		int checksum;
		char name[64];
		int length;
		Vector eyeposition;
		Vector illumposition;
		Vector hull_min;
		Vector hull_max;
		Vector view_bbmin;
		Vector view_bbmax;
		int flags;
		int numbones;
		int boneindex;
		int numbonecontrollers;
		int bonecontrollerindex;
		int numhitboxsets;
		int hitboxsetindex;
		int numlocalanim;
		int localanimindex;
		int numlocalseq;
		int localseqindex;
		int activitylistversion;
		int eventsindexed;
		int numtextures;
		int textureindex;
		int numcdtextures;
		int cdtextureindex;
		int numskinref;
		int numskinfamilies;
		int skinindex;
		int numbodyparts;
		int bodypartindex;
		int numlocalattachments;
		int localattachmentindex;
		int numlocalnodes;
		int localnodeindex;
		int localnodenameindex;
		int numflexdesc;
		int flexdescindex;
		int numflexcontrollers;
		int flexcontrollerindex;
		int numflexrules;
		int flexruleindex;
		int numikchains;
		int ikchainindex;
		int nummouths;
		int mouthindex;
		int numlocalposeparameters;
		int localposeparamindex;
		int surfacepropindex;
		int keyvalueindex;
		int keyvaluesize;
		int numlocalikautoplaylocks;
		int localikautoplaylockindex;
		float mass;
		int contents;
		int numincludemodels;
		int includemodelindex;
		void* virtualModel;
		int szanimblocknameindex;
		int numanimblocks;
		int animblockindex;
		void* animblockModel;
		int bonetablebynameindex;
		void* pVertexBase;
		void* pIndexBase;
		unsigned __int8 constdirectionallightdot;
		unsigned __int8 rootLOD;
		unsigned __int8 numAllowedRootLODs;
		unsigned __int8 unused[1];
		int unused4;
		int numflexcontrollerui;
		int flexcontrolleruiindex;
		float flVertAnimFixedPointScale;
		int surfacepropLookup;
		int studiohdr2index;
		int unused2[1];
	};

	struct IndexDesc_t
	{
		unsigned __int16* m_pIndices;
		unsigned int m_nOffset;
		unsigned int m_nFirstIndex;
		unsigned int m_nIndexSize;
	};

	struct VertexDesc_t
	{
		int m_VertexSize_Position;
		int m_VertexSize_BoneWeight;
		int m_VertexSize_BoneMatrixIndex;
		int m_VertexSize_Normal;
		int m_VertexSize_Color;
		int m_VertexSize_Specular;
		int m_VertexSize_TexCoord[8];
		int m_VertexSize_TangentS;
		int m_VertexSize_TangentT;
		int m_VertexSize_Wrinkle;
		int m_VertexSize_UserData;
		int m_ActualVertexSize;
		VertexCompressionType_t m_CompressionType;
		int m_NumBoneWeights;
		float* m_pPosition;
		float* m_pBoneWeight;
		unsigned __int8* m_pBoneMatrixIndex;
		float* m_pNormal;
		unsigned __int8* m_pColor;
		unsigned __int8* m_pSpecular;
		float* m_pTexCoord[8];
		float* m_pTangentS;
		float* m_pTangentT;
		float* m_pWrinkle;
		float* m_pUserData;
		int m_nFirstVertex;
		unsigned int m_nOffset;
	};

	struct IMesh;
	struct IIndexBuffer_vtbl;
	struct IVertexBuffer_vtbl;
	struct IMaterial;
	struct IMaterial_vtbl;
	struct IMaterialVar_vtbl;

	struct IVertexBuffer
	{
		IVertexBuffer_vtbl* vftable;
	};

	struct IVertexBuffer_vtbl
	{
		int(__thiscall* VertexCount)(IVertexBuffer*);
		unsigned __int64(__thiscall* GetVertexFormat)(IVertexBuffer*);
		bool(__thiscall* IsDynamic)(IVertexBuffer*);
		void(__thiscall* BeginCastBuffer)(IVertexBuffer*, unsigned __int64);
		void(__thiscall* EndCastBuffer)(IVertexBuffer*);
		int(__thiscall* GetRoomRemaining)(IVertexBuffer*);
		bool(__thiscall* Lock)(IVertexBuffer*, int, bool, VertexDesc_t*);
		void(__thiscall* Unlock)(IVertexBuffer*, int, VertexDesc_t*);
		void(__thiscall* Spew)(IVertexBuffer*, int, const VertexDesc_t*);
		void(__thiscall* ValidateData)(IVertexBuffer*, int, const VertexDesc_t*);
	};

	struct IIndexBuffer
	{
		IIndexBuffer_vtbl* vftable;
	};

	struct IIndexBuffer_vtbl
	{
		int(__thiscall* IndexCount)(IIndexBuffer*);
		MaterialIndexFormat_t(__thiscall* IndexFormat)(IIndexBuffer*);
		bool(__thiscall* IsDynamic)(IIndexBuffer*);
		void(__thiscall* BeginCastBuffer)(IIndexBuffer*, MaterialIndexFormat_t);
		void(__thiscall* EndCastBuffer)(IIndexBuffer*);
		int(__thiscall* GetRoomRemaining)(IIndexBuffer*);
		bool(__thiscall* Lock)(IIndexBuffer*, int, bool, IndexDesc_t*);
		void(__thiscall* Unlock)(IIndexBuffer*, int, IndexDesc_t*);
		void(__thiscall* ModifyBegin)(IIndexBuffer*, bool, int, int, IndexDesc_t*);
		void(__thiscall* ModifyEnd)(IIndexBuffer*, IndexDesc_t*);
		void(__thiscall* Spew)(IIndexBuffer*, int, const IndexDesc_t*);
		void(__thiscall* ValidateData)(IIndexBuffer*, int, const IndexDesc_t*);
		IMesh* (__thiscall* GetMesh)(IIndexBuffer*);
	};

	struct IMaterialVar
	{
		IMaterialVar_vtbl* vftable;
		char* m_pStringVal;
		int m_intVal;
		Vector4D m_VecVal;
		unsigned __int8 m_Type : 4;
		unsigned __int8 m_nNumVectorComps : 3;
		unsigned __int8 m_bFakeMaterialVar : 1;
		unsigned __int8 m_nTempIndex;
		CUtlSymbol m_Name;
	};

	struct ITexture
	{
		void* vtbl;
	};

	struct IMaterialVar_vtbl
	{
		ITexture* (__thiscall* GetTextureValue)(IMaterialVar*);
		bool(__thiscall* IsTextureValueInternalEnvCubemap)(IMaterialVar*);
		const char* (__thiscall* GetName)(IMaterialVar*);
		unsigned __int16(__thiscall* GetNameAsSymbol)(IMaterialVar*);
		void(__thiscall* SetFloatValue)(IMaterialVar*, float);
		void(__thiscall* SetIntValue)(IMaterialVar*, int);
		void(__thiscall* SetStringValue)(IMaterialVar*, const char*);
		const char* (__thiscall* GetStringValue)(IMaterialVar*);
		void(__thiscall* SetFourCCValue)(IMaterialVar*, unsigned int, void*);
		void(__thiscall* GetFourCCValue)(IMaterialVar*, unsigned int*, void**);
		void(__thiscall* SetVecValue0)(IMaterialVar*, float, float, float, float);
		void(__thiscall* SetVecValue1)(IMaterialVar*, float, float, float);
		void(__thiscall* SetVecValue2)(IMaterialVar*, float, float);
		void(__thiscall* SetVecValue3)(IMaterialVar*, const float*, int);
		void(__thiscall* GetLinearVecValue)(IMaterialVar*, float*, int);
		void(__thiscall* SetTextureValue)(IMaterialVar*, ITexture*);
		IMaterial* (__thiscall* GetMaterialValue)(IMaterialVar*);
		void(__thiscall* SetMaterialValue)(IMaterialVar*, IMaterial*);
		bool(__thiscall* IsDefined)(IMaterialVar*);
		void(__thiscall* SetUndefined)(IMaterialVar*);
		void(__thiscall* SetMatrixValue)(IMaterialVar*, const VMatrix*);
		const VMatrix* (__thiscall* GetMatrixValue)(IMaterialVar*);
		bool(__thiscall* MatrixIsIdentity)(IMaterialVar*);
		void(__thiscall* CopyFrom)(IMaterialVar*, IMaterialVar*);
		void(__thiscall* SetValueAutodetectType)(IMaterialVar*, const char*);
		IMaterial* (__thiscall* GetOwningMaterial)(IMaterialVar*);
		void(__thiscall* SetVecComponentValue)(IMaterialVar*, float, int);
		int(__thiscall* GetIntValueInternal)(IMaterialVar*);
		float(__thiscall* GetFloatValueInternal)(IMaterialVar*);
		void(__thiscall* GetVecValueInternal0)(IMaterialVar*, float*, int);
		const float* (__thiscall* GetVecValueInternal1)(IMaterialVar*);
		int(__thiscall* VectorSizeInternal)(IMaterialVar*);

	};

	struct IMaterial
	{
		IMaterial_vtbl* vftable;
	};

	struct IMaterial_vtbl
	{
		const char* (__thiscall* GetName)(IMaterial*);
		const char* (__thiscall* GetTextureGroupName)(IMaterial*);
		PreviewImageRetVal_t(__thiscall* GetPreviewImageProperties)(IMaterial*, int*, int*, ImageFormat*, bool*);
		PreviewImageRetVal_t(__thiscall* GetPreviewImage)(IMaterial*, unsigned __int8*, int, int, ImageFormat);
		int(__thiscall* GetMappingWidth)(IMaterial*);
		int(__thiscall* GetMappingHeight)(IMaterial*);
		int(__thiscall* GetNumAnimationFrames)(IMaterial*);
		bool(__thiscall* InMaterialPage)(IMaterial*);
		void(__thiscall* GetMaterialOffset)(IMaterial*, float*);
		void(__thiscall* GetMaterialScale)(IMaterial*, float*);
		IMaterial* (__thiscall* GetMaterialPage)(IMaterial*);
		IMaterialVar* (__thiscall* FindVar)(IMaterial*, const char*, bool*, bool);
		void(__thiscall* IncrementReferenceCount)(IMaterial*);
		void(__thiscall* DecrementReferenceCount)(IMaterial*);
		int(__thiscall* GetEnumerationID)(IMaterial*);
		void(__thiscall* GetLowResColorSample)(IMaterial*, float, float, float*);
		void(__thiscall* RecomputeStateSnapshots)(IMaterial*);
		bool(__thiscall* IsTranslucent)(IMaterial*);
		bool(__thiscall* IsAlphaTested)(IMaterial*);
		bool(__thiscall* IsVertexLit)(IMaterial*);
		unsigned __int64(__thiscall* GetVertexFormat)(IMaterial*);
		bool(__thiscall* HasProxy)(IMaterial*);
		bool(__thiscall* UsesEnvCubemap)(IMaterial*);
		bool(__thiscall* NeedsTangentSpace)(IMaterial*);
		bool(__thiscall* NeedsPowerOfTwoFrameBufferTexture)(IMaterial*, bool);
		bool(__thiscall* NeedsFullFrameBufferTexture)(IMaterial*, bool);
		bool(__thiscall* NeedsSoftwareSkinning)(IMaterial*);
		void(__thiscall* AlphaModulate)(IMaterial*, float);
		void(__thiscall* ColorModulate)(IMaterial*, float, float, float);
		void(__thiscall* SetMaterialVarFlag)(IMaterial*, MaterialVarFlags_t, bool);
		bool(__thiscall* GetMaterialVarFlag)(IMaterial*, MaterialVarFlags_t);
		void(__thiscall* GetReflectivity)(IMaterial*, Vector*);
		bool(__thiscall* GetPropertyFlag)(IMaterial*, MaterialPropertyTypes_t);
		bool(__thiscall* IsTwoSided)(IMaterial*);
		void(__thiscall* SetShader)(IMaterial*, const char*);
		int(__thiscall* GetNumPasses)(IMaterial*);
		int(__thiscall* GetTextureMemoryBytes)(IMaterial*);
		void(__thiscall* Refresh)(IMaterial*);
		bool(__thiscall* NeedsLightmapBlendAlpha)(IMaterial*);
		bool(__thiscall* NeedsSoftwareLighting)(IMaterial*);
		int(__thiscall* ShaderParamCount)(IMaterial*);
		IMaterialVar** (__thiscall* GetShaderParams)(IMaterial*);
		bool(__thiscall* IsErrorMaterial)(IMaterial*);
		void(__thiscall* Unused)(IMaterial*);
		float(__thiscall* GetAlphaModulation)(IMaterial*);
		void(__thiscall* GetColorModulation)(IMaterial*, float*, float*, float*);
		bool(__thiscall* IsTranslucentUnderModulation)(IMaterial*, float);
		IMaterialVar* (__thiscall* FindVarFast)(IMaterial*, const char*, unsigned int*);
		void(__thiscall* SetShaderAndParams)(IMaterial*, KeyValues*);
		const char* (__thiscall* GetShaderName)(IMaterial*);
		void(__thiscall* DeleteIfUnreferenced)(IMaterial*);
		bool(__thiscall* IsSpriteCard)(IMaterial*);
		void(__thiscall* CallBindProxy)(IMaterial*, void*, void*); // ICallQueue
		void(__thiscall* RefreshPreservingMaterialVars)(IMaterial*);
		bool(__thiscall* WasReloadedFromWhitelist)(IMaterial*);
	};

	struct IMesh : IVertexBuffer, IIndexBuffer
	{
	};

	/*struct __declspec(align(1)) OptimizedModel_StripHeader_t
	{
	  int numIndices;
	  int indexOffset;
	  int numVerts;
	  int vertOffset;
	  __int16 numBones;
	  unsigned __int8 flags;
	  int numBoneStateChanges;
	  int boneStateChangeOffset;
	  int numTopologyIndices;
	  int topologyOffset;
	};*/

	struct studiomeshgroup_t
	{
		IMesh* m_pMesh;
		int m_NumStrips;
		int m_Flags;
		void* m_pStripData; // OptimizedModel_StripHeader_t
		unsigned __int16* m_pGroupIndexToMeshIndex;
		int m_NumVertices;
		int* m_pUniqueFaces;
		unsigned __int16* m_pIndices;
		unsigned __int16* m_pTopologyIndices;
		bool m_MeshNeedsRestore;
		__int16 m_ColorMeshID;
		struct IMorph* m_pMorph;
	};

	struct studiomeshdata_t
	{
		int m_NumGroup;
		studiomeshgroup_t* m_pMeshGroup;
	};

	struct studioloddata_t
	{
		studiomeshdata_t* m_pMeshData;
		float m_SwitchPoint;
		int numMaterials;
		IMaterial** ppMaterials;
		int* pMaterialFlags;
		int m_NumFaces;
		int* m_pHWMorphDecalBoneRemap;
		int m_nDecalBoneCount;
	};

	struct studiohwdata_t
	{
		int m_RootLOD;
		int m_NumLODs;
		studioloddata_t* m_pLODs;
		int m_NumStudioMeshes;
		int m_NumFacesRenderedThisFrame;
		int m_NumTimesRenderedThisFrame;
		studiohdr_t* m_pStudioHdr;
	};

	struct StudioDecalHandle_t__
	{
		int unused;
	};

	struct IClientRenderable_vtbl;
	struct IClientRenderable
	{
		IClientRenderable_vtbl* vftable;
	};

	const struct RenderableInstance_t
	{
		unsigned __int8 m_nAlpha;
	};

	struct IClientRenderable_vtbl
	{
		void* (__thiscall* GetIClientUnknown)(IClientRenderable*); // IClientUnknown
		const Vector* (__thiscall* GetRenderOrigin)(IClientRenderable*);
		const QAngle* (__thiscall* GetRenderAngles)(IClientRenderable*);
		bool(__thiscall* ShouldDraw)(IClientRenderable*);
		int(__thiscall* GetRenderFlags)(IClientRenderable*);
		void(__thiscall* Unused)(IClientRenderable*);
		unsigned __int16(__thiscall* GetShadowHandle)(IClientRenderable*);
		unsigned __int16* (__thiscall* RenderHandle)(IClientRenderable*);
		const model_t* (__thiscall* GetModel)(IClientRenderable*);
		int(__thiscall* DrawModel)(IClientRenderable*, int, const RenderableInstance_t*);
		int(__thiscall* GetBody)(IClientRenderable*);
		void(__thiscall* GetColorModulation)(IClientRenderable*, float*);
		bool(__thiscall* LODTest)(IClientRenderable*);
		bool(__thiscall* SetupBones)(IClientRenderable*, matrix3x4_t*, int, int, float);
		void(__thiscall* SetupWeights)(IClientRenderable*, const matrix3x4_t*, int, float*, float*);
		void(__thiscall* DoAnimationEvents)(IClientRenderable*);
		void* (__thiscall* GetPVSNotifyInterface)(IClientRenderable*); // IPVSNotify
		void(__thiscall* GetRenderBounds)(IClientRenderable*, Vector*, Vector*);
		void(__thiscall* GetRenderBoundsWorldspace)(IClientRenderable*, Vector*, Vector*);
		void(__thiscall* GetShadowRenderBounds)(IClientRenderable*, Vector*, Vector*, ShadowType_t);
		bool(__thiscall* ShouldReceiveProjectedTextures)(IClientRenderable*, int);
		bool(__thiscall* GetShadowCastDistance)(IClientRenderable*, float*, ShadowType_t);
		bool(__thiscall* GetShadowCastDirection)(IClientRenderable*, Vector*, ShadowType_t);
		bool(__thiscall* IsShadowDirty)(IClientRenderable*);
		void(__thiscall* MarkShadowDirty)(IClientRenderable*, bool);
		IClientRenderable* (__thiscall* GetShadowParent)(IClientRenderable*);
		IClientRenderable* (__thiscall* FirstShadowChild)(IClientRenderable*);
		IClientRenderable* (__thiscall* NextShadowPeer)(IClientRenderable*);
		ShadowType_t(__thiscall* ShadowCastType)(IClientRenderable*);
		void(__thiscall* Unused2)(IClientRenderable*);
		void(__thiscall* CreateModelInstance)(IClientRenderable*);
		unsigned __int16(__thiscall* GetModelInstance)(IClientRenderable*);
		const matrix3x4_t* (__thiscall* RenderableToWorldTransform)(IClientRenderable*);
		int(__thiscall* LookupAttachment)(IClientRenderable*, const char*);
		bool(__thiscall* GetAttachment0)(IClientRenderable*, int, matrix3x4_t*);
		bool(__thiscall* GetAttachment1)(IClientRenderable*, int, Vector*, QAngle*);
		float* (__thiscall* GetRenderClipPlane)(IClientRenderable*);
		int(__thiscall* GetSkin)(IClientRenderable*);
		void(__thiscall* OnThreadedDrawSetup)(IClientRenderable*);
		bool(__thiscall* UsesFlexDelayedWeights)(IClientRenderable*);
		void(__thiscall* RecordToolMessage)(IClientRenderable*);
		bool(__thiscall* ShouldDrawForSplitScreenUser)(IClientRenderable*, int);
		unsigned __int8(__thiscall* OverrideAlphaModulation)(IClientRenderable*, unsigned __int8);
		unsigned __int8(__thiscall* OverrideShadowAlphaModulation)(IClientRenderable*, unsigned __int8);
		void* (__thiscall* GetClientModelRenderable)(IClientRenderable*); // IClientModelRenderable
	};

	struct __declspec(align(4)) ModelRenderInfo_t
	{
		Vector origin;
		QAngle angles;
		IClientRenderable* pRenderable;
		const model_t* pModel;
		const matrix3x4_t* pModelToWorld;
		const matrix3x4_t* pLightingOffset;
		const Vector* pLightingOrigin;
		int flags;
		int entity_index;
		int skin;
		int body;
		int hitboxset;
		unsigned __int16 instance;
	};

	struct ColorMeshInfo_t
	{
		IMesh* m_pMesh;
		void* m_pPooledVBAllocator; // IPooledVBAllocator
		int m_nVertOffsetInBytes;
		int m_nNumVerts;
	};

	struct LightDesc_t
	{
		LightType_t m_Type;
		Vector m_Color;
		Vector m_Position;
		Vector m_Direction;
		float m_Range;
		float m_Falloff;
		float m_Attenuation0;
		float m_Attenuation1;
		float m_Attenuation2;
		float m_Theta;
		float m_Phi;
		float m_ThetaDot;
		float m_PhiDot;
		float m_OneOverThetaDotMinusPhiDot;
		unsigned int m_Flags;
		float m_RangeSquared;
	};

	struct MaterialLightingState_t
	{
		Vector m_vecAmbientCube[6];
		Vector m_vecLightingOrigin;
		int m_nLocalLightCount;
		LightDesc_t m_pLocalLightDesc[4];
	};

	struct DrawModelInfo_t
	{
		studiohdr_t* m_pStudioHdr;
		studiohwdata_t* m_pHardwareData;
		StudioDecalHandle_t__* m_Decals;
		int m_Skin;
		int m_Body;
		int m_HitboxSet;
		void* m_pClientEntity;
		int m_Lod;
		ColorMeshInfo_t* m_pColorMeshes;
		bool m_bStaticLighting;
		MaterialLightingState_t m_LightingState;
	};

	struct DrawModelState_t
	{
		studiohdr_t* m_pStudioHdr;
		studiohwdata_t* m_pStudioHWData;
		IClientRenderable* m_pRenderable;
		const matrix3x4_t* m_pModelToWorld;
		StudioDecalHandle_t__* m_decals;
		int m_drawFlags;
		int m_lod;
	};

	struct StaticPropRenderInfo_t
	{
		const matrix3x4_t* pModelToWorld;
		const model_t* pModel;
		IClientRenderable* pRenderable;
		Vector* pLightingOrigin;
		unsigned __int16 instance;
		unsigned __int8 skin;
		unsigned __int8 alpha;
	};

	struct CStudioHdr
	{
		const studiohdr_t* m_pStudioHdr;
		void* m_pVModel; // virtualmodel_t
		// ...
	};

	struct WorldListLeafData_t
	{
		unsigned __int16 leafIndex;
		__int16 waterData;
		unsigned __int16 firstTranslucentSurface;
		unsigned __int16 translucentSurfaceCount;
	};

	struct WorldListInfo_t
	{
		int m_ViewFogVolume;
		int m_LeafCount;
		bool m_bHasWater;
		WorldListLeafData_t* m_pLeafDataList;
	};

	struct VisOverrideData_t
	{
		Vector m_vecVisOrigin;
		float m_fDistToAreaPortalTolerance;
		Vector m_vPortalCorners[4];
		bool m_bTrimFrustumToPortalCorners;
		Vector m_vPortalOrigin;
		Vector m_vPortalForward;
		float m_flPortalRadius;
	};

	enum MotionBlurMode_t : __int32
	{
		MOTION_BLUR_DISABLE = 0x1,
		MOTION_BLUR_GAME = 0x2,
		MOTION_BLUR_SFM = 0x3,
	};

	struct __declspec(align(4)) CViewSetup
	{
		int x;
		int m_nUnscaledX;
		int y;
		int m_nUnscaledY;
		int width;
		int m_nUnscaledWidth;
		int height;
		int m_nUnscaledHeight;
		bool m_bOrtho;
		float m_OrthoLeft;
		float m_OrthoTop;
		float m_OrthoRight;
		float m_OrthoBottom;
		bool m_bCustomViewMatrix;
		matrix3x4_t m_matCustomViewMatrix;
		float fov;
		float fovViewmodel;
		Vector origin;
		QAngle angles;
		float zNear;
		float zFar;
		float zNearViewmodel;
		float zFarViewmodel;
		float m_flAspectRatio;
		float m_flNearBlurDepth;
		float m_flNearFocusDepth;
		float m_flFarFocusDepth;
		float m_flFarBlurDepth;
		float m_flNearBlurRadius;
		float m_flFarBlurRadius;
		int m_nDoFQuality;
		MotionBlurMode_t m_nMotionBlurMode;
		float m_flShutterTime;
		Vector m_vShutterOpenPosition;
		QAngle m_shutterOpenAngles;
		Vector m_vShutterClosePosition;
		QAngle m_shutterCloseAngles;
		float m_flOffCenterTop;
		float m_flOffCenterBottom;
		float m_flOffCenterLeft;
		float m_flOffCenterRight;
		__int8 m_bOffCenter : 1;
		__int8 m_bRenderToSubrectOfLargerScreen : 1;
		__int8 m_bDoBloomAndToneMapping : 1;
		__int8 m_bDoDepthOfField : 1;
		__int8 m_bHDRTarget : 1;
		__int8 m_bDrawWorldNormal : 1;
		__int8 m_bCullFrontFaces : 1;
		__int8 m_bCacheFullSceneState : 1;
	};

	struct IRender_vtbl;
	struct IRender
	{
		IRender_vtbl* vftable;
	};

	struct IRender_vtbl
	{
		void(__thiscall* FrameBegin)(IRender*);
		void(__thiscall* FrameEnd)(IRender*);
		void(__thiscall* ViewSetupVis)(IRender*, bool, int, const Vector*);
		void(__thiscall* ViewDrawFade)(IRender*, unsigned __int8*, IMaterial*);
		void(__thiscall* DrawSceneBegin)(IRender*);
		void(__thiscall* DrawSceneEnd)(IRender*);
		void* (__thiscall* CreateWorldList)(IRender*); // IWorldRenderList
		void(__thiscall* BuildWorldLists)(IRender*, void*, WorldListInfo_t*, int, const VisOverrideData_t*, bool, float*); // IWorldRenderList
		void(__thiscall* DrawWorldLists)(IRender*, void*, void*, unsigned int, float); // IMatRenderContext - IWorldRenderList
		const Vector* (__thiscall* ViewOrigin)(IRender*);
		const QAngle* (__thiscall* ViewAngles)(IRender*);
		const CViewSetup* (__thiscall* ViewGetCurrent)(IRender*);
		const VMatrix* (__thiscall* ViewMatrix)(IRender*);
		const VMatrix* (__thiscall* WorldToScreenMatrix)(IRender*);
		float(__thiscall* GetFramerate)(IRender*);
		float(__thiscall* GetZNear)(IRender*);
		float(__thiscall* GetZFar)(IRender*);
		float(__thiscall* GetFov)(IRender*);
		float(__thiscall* GetFovY)(IRender*);
		float(__thiscall* GetFovViewmodel)(IRender*);
		bool(__thiscall* ClipTransform)(IRender*, const Vector*, Vector*);
		bool(__thiscall* ScreenTransform)(IRender*, const Vector*, Vector*);
		void(__thiscall* Push3DView0)(IRender*, void*, const CViewSetup*, int, ITexture*, VPlane*, ITexture*); // IMatRenderContext
		void(__thiscall* Push3DView1)(IRender*, void*, const CViewSetup*, int, ITexture*, VPlane*); // IMatRenderContext
		void(__thiscall* Push2DView)(IRender*, void*, const CViewSetup*, int, ITexture*, VPlane*); // IMatRenderContext
		void(__thiscall* PopView)(IRender*, void*, VPlane*); // IMatRenderContext
		void(__thiscall* SetMainView)(IRender*, const Vector*, const QAngle*);
		void(__thiscall* ViewSetupVisEx)(IRender*, bool, int, const Vector*, unsigned int*);
		void(__thiscall* OverrideViewFrustum)(IRender*, VPlane*);
		void(__thiscall* UpdateBrushModelLightmap)(IRender*, model_t*, IClientRenderable*);
		void(__thiscall* BeginUpdateLightmaps)(IRender*);
		void(__thiscall* EndUpdateLightmaps)(IRender*);
		bool(__thiscall* InLightmapUpdate)(IRender*);
	};

	struct CRender_vtbl;
	struct __declspec(align(8)) CRender : IRender
	{
		//CRender_vtbl* vftable;
		float m_yFOV;
		long double m_frameStartTime;
		float m_framerate;
		float m_zNear;
		float m_zFar;
		VMatrix m_matrixView;
		VMatrix m_matrixProjection;
		VMatrix m_matrixWorldToScreen;
		//CUtlStack<CRender::ViewStack_t, CUtlMemory<CRender::ViewStack_t, int> > m_ViewStack;
		//int m_iLightmapUpdateDepth;
	};

	struct CRender_vtbl
	{
		void(__thiscall* FrameBegin)(IRender*);
		void(__thiscall* FrameEnd)(IRender*);
		void(__thiscall* ViewSetupVis)(IRender*, bool, int, const Vector*);
		void(__thiscall* ViewDrawFade)(IRender*, unsigned __int8*, IMaterial*);
		void(__thiscall* DrawSceneBegin)(IRender*);
		void(__thiscall* DrawSceneEnd)(IRender*);
		void* (__thiscall* CreateWorldList)(IRender*); // IWorldRenderList
		void(__thiscall* BuildWorldLists)(IRender*, void*, WorldListInfo_t*, int, const VisOverrideData_t*, bool, float*); // IWorldRenderList
		void(__thiscall* DrawWorldLists)(IRender*, void*, void*, unsigned int, float); // IWorldRenderList - IWorldRenderList
		const Vector* (__thiscall* ViewOrigin)(IRender*);
		const QAngle* (__thiscall* ViewAngles)(IRender*);
		const CViewSetup* (__thiscall* ViewGetCurrent)(IRender*);
		const VMatrix* (__thiscall* ViewMatrix)(IRender*);
		const VMatrix* (__thiscall* WorldToScreenMatrix)(IRender*);
		float(__thiscall* GetFramerate)(IRender*);
		float(__thiscall* GetZNear)(IRender*);
		float(__thiscall* GetZFar)(IRender*);
		float(__thiscall* GetFov)(IRender*);
		float(__thiscall* GetFovY)(IRender*);
		float(__thiscall* GetFovViewmodel)(IRender*);
		bool(__thiscall* ClipTransform)(IRender*, const Vector*, Vector*);
		bool(__thiscall* ScreenTransform)(IRender*, const Vector*, Vector*);
		void(__thiscall* Push3DView0)(IRender*, void*, const CViewSetup*, int, ITexture*, VPlane*, ITexture*); // IMatRenderContext
		void(__thiscall* Push3DView1)(IRender*, void*, const CViewSetup*, int, ITexture*, VPlane*); // IMatRenderContext
		void(__thiscall* Push2DView)(IRender*, void*, const CViewSetup*, int, ITexture*, VPlane*); // IMatRenderContext
		void(__thiscall* PopView)(IRender*, void*, VPlane*); // IMatRenderContext
		void(__thiscall* SetMainView)(IRender*, const Vector*, const QAngle*);
		void(__thiscall* ViewSetupVisEx)(IRender*, bool, int, const Vector*, unsigned int*);
		void(__thiscall* OverrideViewFrustum)(IRender*, VPlane*);
		void(__thiscall* UpdateBrushModelLightmap)(IRender*, model_t*, IClientRenderable*);
		void(__thiscall* BeginUpdateLightmaps)(IRender*);
		void(__thiscall* EndUpdateLightmaps)(IRender*);
		bool(__thiscall* InLightmapUpdate)(IRender*);
	};

	enum StreamSpec_t : __int32
	{
		STREAM_DEFAULT = 0x0,
		STREAM_SPECULAR1 = 0x1,
		STREAM_FLEXDELTA = 0x2,
		STREAM_MORPH = 0x3,
		STREAM_UNIQUE_A = 0x4,
		STREAM_UNIQUE_B = 0x5,
		STREAM_UNIQUE_C = 0x6,
		STREAM_UNIQUE_D = 0x7,
		STREAM_SUBDQUADS = 0x8,
	};

	struct __declspec(align(8)) VertexStreamSpec_t
	{
		unsigned __int64 iVertexDataElement;
		StreamSpec_t iStreamSpec;
	};

	struct CPrimList
	{
		int m_FirstIndex;
		int m_NumIndices;
	};

	struct MeshDesc_t : VertexDesc_t, IndexDesc_t
	{
	};

	struct CMeshBase : IMesh
	{
	};

	struct CMeshBase_vtbl
	{
		int(__thiscall* VertexCount)(IVertexBuffer*);
		unsigned __int64(__thiscall* GetVertexFormat)(IVertexBuffer*);
		bool(__thiscall* IsDynamic)(IVertexBuffer*);
		void(__thiscall* BeginCastBuffer)(IVertexBuffer*, unsigned __int64);
		void(__thiscall* EndCastBuffer)(IVertexBuffer*);
		int(__thiscall* GetRoomRemaining)(IVertexBuffer*);
		bool(__thiscall* Lock)(IVertexBuffer*, int, bool, VertexDesc_t*);
		void(__thiscall* Unlock)(IVertexBuffer*, int, VertexDesc_t*);
		void(__thiscall* Spew0)(IVertexBuffer*, int, const VertexDesc_t*);
		void(__thiscall* ValidateData0)(IVertexBuffer*, int, const VertexDesc_t*);
		void(__thiscall* SetPrimitiveType)(IMesh*, MaterialPrimitiveType_t);
		void(__thiscall* Draw0)(IMesh*, CPrimList*, int);
		void(__thiscall* Draw1)(IMesh*, int, int);
		void(__thiscall* SetColorMesh)(IMesh*, IMesh*, int);
		void(__thiscall* CopyToMeshBuilder)(IMesh*, int, int, int, int, int, void*); // CMeshBuilder
		void(__thiscall* Spew1)(IMesh*, int, int, const MeshDesc_t*);
		void(__thiscall* ValidateData1)(IMesh*, int, int, const MeshDesc_t*);
		void(__thiscall* LockMesh)(IMesh*, int, int, MeshDesc_t*, void*); // MeshBuffersAllocationSettings_t
		void(__thiscall* ModifyBegin)(IMesh*, int, int, int, int, MeshDesc_t*);
		void(__thiscall* ModifyEnd)(IMesh*, MeshDesc_t*);
		void(__thiscall* UnlockMesh)(IMesh*, int, int, MeshDesc_t*);
		void(__thiscall* ModifyBeginEx)(IMesh*, bool, int, int, int, int, MeshDesc_t*);
		void(__thiscall* SetFlexMesh)(IMesh*, IMesh*, int);
		void(__thiscall* DisableFlexMesh)(IMesh*);
		void(__thiscall* MarkAsDrawn)(IMesh*);
		void(__thiscall* DrawModulated)(IMesh*, const Vector4D*, int, int);
		unsigned int(__thiscall* ComputeMemoryUsed)(IMesh*);
		void* (__thiscall* AccessRawHardwareDataStream)(IMesh*, unsigned __int8, unsigned int, unsigned int, void*);
		void* (__thiscall* GetCachedPerFrameMeshData)(IMesh*); // ICachedPerFrameMeshData
		void(__thiscall* ReconstructFromCachedPerFrameMeshData)(IMesh*, void*); // ICachedPerFrameMeshData
		void(__thiscall* BeginPass)(CMeshBase*);
		void(__thiscall* RenderPass)(CMeshBase*, const unsigned __int8*);
		bool(__thiscall* HasColorMesh)(CMeshBase*);
		bool(__thiscall* HasFlexMesh)(CMeshBase*);
		bool(__thiscall* IsUsingVertexID)(CMeshBase*);
		VertexStreamSpec_t* (__thiscall* GetVertexStreamSpec)(CMeshBase*);
		void(__thiscall * CMeshBase_destructor)(CMeshBase*);
	};

	struct CBaseMeshDX8 : CMeshBase
	{
		bool m_bMeshLocked;
		unsigned __int64 m_VertexFormat;
	};

	struct CBaseMeshDX8_vtbl
	{
		int(__thiscall* VertexCount)(IVertexBuffer*);
		unsigned __int64(__thiscall* GetVertexFormat)(IVertexBuffer*);
		bool(__thiscall* IsDynamic)(IVertexBuffer*);
		void(__thiscall* BeginCastBuffer)(IVertexBuffer*, unsigned __int64);
		void(__thiscall* EndCastBuffer)(IVertexBuffer*);
		int(__thiscall* GetRoomRemaining)(IVertexBuffer*);
		bool(__thiscall* Lock)(IVertexBuffer*, int, bool, VertexDesc_t*);
		void(__thiscall* Unlock)(IVertexBuffer*, int, VertexDesc_t*);
		void(__thiscall* Spew0)(IVertexBuffer*, int, const VertexDesc_t*);
		void(__thiscall* ValidateData0)(IVertexBuffer*, int, const VertexDesc_t*);
		void(__thiscall* SetPrimitiveType)(IMesh*, MaterialPrimitiveType_t);
		void(__thiscall* Draw0)(IMesh*, CPrimList*, int);
		void(__thiscall* Draw1)(IMesh*, int, int);
		void(__thiscall* SetColorMesh)(IMesh*, IMesh*, int);
		void(__thiscall* CopyToMeshBuilder)(IMesh*, int, int, int, int, int, void*); // CMeshBuilder
		void(__thiscall* Spew1)(IMesh*, int, int, const MeshDesc_t*);
		void(__thiscall* ValidateData1)(IMesh*, int, int, const MeshDesc_t*);
		void(__thiscall* LockMesh)(IMesh*, int, int, MeshDesc_t*, void*); // MeshBuffersAllocationSettings_t
		void(__thiscall* ModifyBegin)(IMesh*, int, int, int, int, MeshDesc_t*);
		void(__thiscall* ModifyEnd)(IMesh*, MeshDesc_t*);
		void(__thiscall* UnlockMesh)(IMesh*, int, int, MeshDesc_t*);
		void(__thiscall* ModifyBeginEx)(IMesh*, bool, int, int, int, int, MeshDesc_t*);
		void(__thiscall* SetFlexMesh)(IMesh*, IMesh*, int);
		void(__thiscall* DisableFlexMesh)(IMesh*);
		void(__thiscall* MarkAsDrawn)(IMesh*);
		void(__thiscall* DrawModulated)(IMesh*, const Vector4D*, int, int);
		unsigned int(__thiscall* ComputeMemoryUsed)(IMesh*);
		void* (__thiscall* AccessRawHardwareDataStream)(IMesh*, unsigned __int8, unsigned int, unsigned int, void*);
		void* (__thiscall* GetCachedPerFrameMeshData)(IMesh*); // ICachedPerFrameMeshData
		void(__thiscall* ReconstructFromCachedPerFrameMeshData)(IMesh*, void*); // ICachedPerFrameMeshData
		void(__thiscall* BeginPass)(CMeshBase*);
		void(__thiscall* RenderPass)(CMeshBase*, const unsigned __int8*);
		bool(__thiscall* HasColorMesh)(CMeshBase*);
		bool(__thiscall* HasFlexMesh)(CMeshBase*);
		bool(__thiscall* IsUsingVertexID)(CMeshBase*);
		VertexStreamSpec_t* (__thiscall* GetVertexStreamSpec)(CMeshBase*);
		void(__thiscall * CMeshBase_destructor)(CMeshBase*);
		void(__thiscall* SetVertexFormat)(CBaseMeshDX8*, unsigned __int64, bool, bool);
		bool(__thiscall* IsExternal)(CBaseMeshDX8*);
		void(__thiscall* SetMaterial)(CBaseMeshDX8*, IMaterial*);
		void(__thiscall* GetColorMesh)(CBaseMeshDX8*, const IVertexBuffer**, int*);
		void(__thiscall* HandleLateCreation)(CBaseMeshDX8*);
		MaterialPrimitiveType_t(__thiscall* GetPrimitiveType)(CBaseMeshDX8*);
		void* (__thiscall* GetVertexBuffer)(CBaseMeshDX8*); // CVertexBuffer
		void* (__thiscall* GetIndexBuffer)(CBaseMeshDX8*); // CIndexBuffer
		bool(__thiscall* NeedsVertexFormatReset)(CBaseMeshDX8*, unsigned __int64);
		bool(__thiscall* HasEnoughRoom)(CBaseMeshDX8*, int, int);
		void(__thiscall* PreLock)(CBaseMeshDX8*);
	};

	struct __declspec(align(8)) CMeshDX8 : CBaseMeshDX8
	{
		void* m_pVertexBuffer; // CVertexBuffer
		void* m_pIndexBuffer; // CIndexBuffer
		CMeshDX8* m_pColorMesh;
		int m_nColorMeshVertOffsetInBytes;
		unsigned __int64 m_fmtStreamSpec;
		void* m_pVertexStreamSpec; // CArrayAutoPtr
		void* m_pVbTexCoord1; // CVertexBuffer
		IDirect3DVertexBuffer9* m_arrRawHardwareDataStreams[1];
		void* m_pFlexVertexBuffer; // CVertexBuffer
		bool m_bHasRawHardwareDataStreams;
		bool m_bHasFlexVerts;
		int m_nFlexVertOffsetInBytes;
		int m_flexVertCount;
		MaterialPrimitiveType_t m_Type;
		_D3DPRIMITIVETYPE m_Mode;
		unsigned int m_NumIndices;
		unsigned __int16 m_NumVertices;
		bool m_IsVBLocked;
		bool m_IsIBLocked;
		int m_FirstIndex;
		const char* m_pTextureGroupName;
	};
	STATIC_ASSERT_OFFSET(CMeshDX8, m_Type, 0x4C);

	struct CMeshDX8_vtbl
	{
		int(__thiscall* VertexCount)(IVertexBuffer*);
		unsigned __int64(__thiscall* GetVertexFormat)(IVertexBuffer*);
		bool(__thiscall* IsDynamic)(IVertexBuffer*);
		void(__thiscall* BeginCastBuffer)(IVertexBuffer*, unsigned __int64);
		void(__thiscall* EndCastBuffer)(IVertexBuffer*);
		int(__thiscall* GetRoomRemaining)(IVertexBuffer*);
		bool(__thiscall* Lock)(IVertexBuffer*, int, bool, VertexDesc_t*);
		void(__thiscall* Unlock)(IVertexBuffer*, int, VertexDesc_t*);
		void(__thiscall* Spew0)(IVertexBuffer*, int, const VertexDesc_t*);
		void(__thiscall* ValidateData0)(IVertexBuffer*, int, const VertexDesc_t*);
		void(__thiscall* SetPrimitiveType)(IMesh*, MaterialPrimitiveType_t);
		void(__thiscall* Draw0)(IMesh*, CPrimList*, int);
		void(__thiscall* Draw1)(IMesh*, int, int);
		void(__thiscall* SetColorMesh)(IMesh*, IMesh*, int);
		void(__thiscall* CopyToMeshBuilder)(IMesh*, int, int, int, int, int, void*); // CMeshBuilder
		void(__thiscall* Spew1)(IMesh*, int, int, const MeshDesc_t*);
		void(__thiscall* ValidateData1)(IMesh*, int, int, const MeshDesc_t*);
		void(__thiscall* LockMesh)(IMesh*, int, int, MeshDesc_t*, void*); // MeshBuffersAllocationSettings_t
		void(__thiscall* ModifyBegin)(IMesh*, int, int, int, int, MeshDesc_t*);
		void(__thiscall* ModifyEnd)(IMesh*, MeshDesc_t*);
		void(__thiscall* UnlockMesh)(IMesh*, int, int, MeshDesc_t*);
		void(__thiscall* ModifyBeginEx)(IMesh*, bool, int, int, int, int, MeshDesc_t*);
		void(__thiscall* SetFlexMesh)(IMesh*, IMesh*, int);
		void(__thiscall* DisableFlexMesh)(IMesh*);
		void(__thiscall* MarkAsDrawn)(IMesh*);
		void(__thiscall* DrawModulated)(IMesh*, const Vector4D*, int, int);
		unsigned int(__thiscall* ComputeMemoryUsed)(IMesh*);
		void* (__thiscall* AccessRawHardwareDataStream)(IMesh*, unsigned __int8, unsigned int, unsigned int, void*);
		void* (__thiscall* GetCachedPerFrameMeshData)(IMesh*); // ICachedPerFrameMeshData
		void(__thiscall* ReconstructFromCachedPerFrameMeshData)(IMesh*, void*); // ICachedPerFrameMeshData
		void(__thiscall* BeginPass)(CMeshBase*);
		void(__thiscall* RenderPass)(CMeshBase*, const unsigned __int8*);
		bool(__thiscall* HasColorMesh)(CMeshBase*);
		bool(__thiscall* HasFlexMesh)(CMeshBase*);
		bool(__thiscall* IsUsingVertexID)(CMeshBase*);
		VertexStreamSpec_t* (__thiscall* GetVertexStreamSpec)(CMeshBase*);
		void(__thiscall * CMeshBase_destructor)(CMeshBase*);
		void(__thiscall* SetVertexFormat)(CBaseMeshDX8*, unsigned __int64, bool, bool);
		bool(__thiscall* IsExternal)(CBaseMeshDX8*);
		void(__thiscall* SetMaterial)(CBaseMeshDX8*, IMaterial*);
		void(__thiscall* GetColorMesh)(CBaseMeshDX8*, const IVertexBuffer**, int*);
		void(__thiscall* HandleLateCreation)(CBaseMeshDX8*);
		MaterialPrimitiveType_t(__thiscall* GetPrimitiveType)(CBaseMeshDX8*);
		void* (__thiscall* GetVertexBuffer)(CBaseMeshDX8*); // CVertexBuffer
		void* (__thiscall* GetIndexBuffer)(CBaseMeshDX8*); // CIndexBuffer
		bool(__thiscall* NeedsVertexFormatReset)(CBaseMeshDX8*, unsigned __int64);
		bool(__thiscall* HasEnoughRoom)(CBaseMeshDX8*, int, int);
		void(__thiscall* PreLock)(CBaseMeshDX8*);
	};

}

