#pragma once

namespace components
{
	namespace api
	{
		enum DEBUG_REMIX_LINE_COLOR
		{
			RED = 0u,
			GREEN = 1u,
			TEAL = 2u,
		};

		extern void init();
		extern void create_quad(remixapi_HardcodedVertex* v_out, uint32_t* i_out, const float scale);
		extern void add_debug_line(const Vector& p1, const Vector& p2, const float width, DEBUG_REMIX_LINE_COLOR color);

		extern bool m_initialized;
		extern remixapi_Interface bridge;

		extern remixapi_MaterialHandle remix_debug_line_materials[3];
		extern remixapi_MeshHandle remix_debug_line_list[128];
		extern std::uint32_t remix_debug_line_amount;
		extern std::uint64_t remix_debug_last_line_hash;

		

		enum PORTAL_PAIR : std::uint8_t
		{
			PORTAL_PAIR_1 = 0,
			PORTAL_PAIR_2 = 1,
		};

		// variables directly influencing the class below 
		extern bool rayportal_show_debug_info;

		class rayportal_context
		{
		public:
			class portal_single
			{
			public:
				portal_single() = default;
				portal_single(const Vector& pos, const Vector& rot, const Vector& scale, bool square_mask)
					: m_pos(pos), m_rot(rot), m_scale(scale), m_square_mask(square_mask) {}

				/**
				 * Initiates a full rayportal via the api (material and mesh) (1x1 Unit)
				 * @param index	The actual rayportal index (0/1 are reserverd for player/level portals)
				 * @return		Returns true if successful
				 */
				bool init(uint8_t index)
				{
					this->assign_index(index);

					bool result;
					result = this->create_material() == REMIXAPI_ERROR_CODE_SUCCESS;
					result = this->create_mesh() == REMIXAPI_ERROR_CODE_SUCCESS ? result : false;
					return result;
				}

				void assign_index(uint8_t index) {
					m_index = index;
				}

				uint8_t get_index() const {
					return m_index;
				}

				remixapi_MeshHandle get_mesh() const {
					return m_hmesh;
				}

				remixapi_ErrorCode create_mesh()
				{
					if (!m_hmaterial) {
						this->create_material();
					}

					if (m_hmesh) {
						this->destroy_mesh();
					}

					remixapi_HardcodedVertex verts[4] = {};
					uint32_t indices[6] = {};
					api::create_quad(verts, indices, 0.5f);

					remixapi_MeshInfoSurfaceTriangles triangles =
					{
					  .vertices_values = verts,
					  .vertices_count = ARRAYSIZE(verts),
					  .indices_values = indices,
					  .indices_count = 6,
					  .skinning_hasvalue = FALSE,
					  .skinning_value = {},
					  .material = m_hmaterial,
					};

					remixapi_MeshInfo i =
					{
					  .sType = REMIXAPI_STRUCT_TYPE_MESH_INFO,
					  .hash = 20 + (uint64_t)m_index,
					  .surfaces_values = &triangles,
					  .surfaces_count = 1,
					};

					return api::bridge.CreateMesh(&i, &m_hmesh);
				}

				remixapi_ErrorCode destroy_mesh()
				{
					const auto res = api::bridge.DestroyMesh(m_hmesh);
					m_hmesh = nullptr;
					return res;
				}

				remixapi_MaterialHandle get_material() const {
					return m_hmaterial;
				}

				remixapi_ErrorCode create_material()
				{
					if (m_hmaterial) {
						this->destroy_material();
					}

					std::wstring mask_path;
					if (m_square_mask)
					{
						mask_path = std::wstring(game::root_path.begin(), game::root_path.end());
						mask_path += L"portal2-rtx\\textures\\white.dds";
					}

					remixapi_MaterialInfo info = {};
					info.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO;
					info.hash = 10 + (uint64_t)m_index;
					info.emissiveIntensity = 0.0f;
					info.emissiveColorConstant = { 0.0f, 0.0f, 0.0f };
					info.albedoTexture = !m_square_mask ? L"" : mask_path.data(); // requires changes to dxvk-remix (PR#80)
					info.normalTexture = L"";
					info.tangentTexture = L"";
					info.emissiveTexture = L"";
					info.spriteSheetFps = 1;
					info.spriteSheetCol = 1;
					info.spriteSheetRow = 1;
					info.filterMode = 1u;
					info.wrapModeU = 1u;
					info.wrapModeV = 1u;

					remixapi_MaterialInfoPortalEXT ext = {};
					ext.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO_PORTAL_EXT;
					ext.rayPortalIndex = m_index;
					ext.rotationSpeed = 1.0f;

					info.pNext = &ext;
					return api::bridge.CreateMaterial(&info, &m_hmaterial);
				}

				remixapi_ErrorCode destroy_material()
				{
					const auto res = api::bridge.DestroyMaterial(m_hmaterial);
					m_hmaterial = nullptr;
					return res;
				}


				/**
				 * Calculates the surface normal of the portal
				 * @param matrix	row-major transform matrix
				 * @return			the new normal
				 */
				const Vector& calculate_normal(const utils::vector::matrix3x3& matrix)
				{
					Vector initial_normal = { 0.0f, -1.0f, 0.0f };

					Vector new_normal;
					new_normal[0] = matrix.m[0][0] * initial_normal.x + matrix.m[1][0] * initial_normal.y + matrix.m[2][0] * initial_normal.z;
					new_normal[1] = matrix.m[0][1] * initial_normal.x + matrix.m[1][1] * initial_normal.y + matrix.m[2][1] * initial_normal.z;
					new_normal[2] = matrix.m[0][2] * initial_normal.x + matrix.m[1][2] * initial_normal.y + matrix.m[2][2] * initial_normal.z;
					new_normal.NormalizeChecked();
					m_normal = new_normal;
					return m_normal;
				}

				const Vector& get_normal() const {
					return m_normal;
				}

				const remixapi_Transform& get_remix_transform() const {
					return m_transform;
				}

				/**
				 * Caches the portal transform and normal
				 * @param transform the remixapi_Transform for the portal 
				 * @param normal	normal of the portal
				 */
				void cache(const remixapi_Transform& transform, const Vector& normal)
				{
					m_transform = transform;
					m_normal = normal;
					m_cached = true;
				}

				void uncache() {
					m_cached = false;
				}

				bool is_cached() const  {
					return m_cached;
				}


				Vector m_pos = {};
				Vector m_rot = {};
				Vector m_scale = {};
				bool m_square_mask = false;

			private:
				uint8_t m_index = 0u;

				bool m_cached = false; // is transform/normal cached
				Vector m_normal = { 0.0f, -1.0f, 0.0f };
				remixapi_Transform m_transform = {};

				remixapi_MeshHandle m_hmesh = nullptr;
				remixapi_MaterialHandle m_hmaterial = nullptr;
			};

			class portal_pair
			{
			public:
				portal_pair(const PORTAL_PAIR& pair) : m_pair(pair)
				{
					switch (pair)
					{
					case PORTAL_PAIR_1:
						m_portal0.init(2);
						m_portal1.init(3);
						break;
					case PORTAL_PAIR_2:
						m_portal0.init(4);
						m_portal1.init(5);
						break;
					}
				}

				portal_pair(const PORTAL_PAIR& pair,
					const Vector& pos1, const Vector& rot1, const Vector& scale1, bool square_mask1,
					const Vector& pos2, const Vector& rot2, const Vector& scale2, bool square_mask2)
					: m_pair(pair)
					, m_portal0(pos1, rot1, scale1, square_mask1)
					, m_portal1(pos2, rot2, scale2, square_mask2)
				{
					switch (pair)
					{
					case PORTAL_PAIR_1:
						m_portal0.init(2);
						m_portal1.init(3);
						break;
					case PORTAL_PAIR_2:
						m_portal0.init(4);
						m_portal1.init(5);
						break;
					}
				}

				//// del copy and copy assignment operator
				//portal_pair(const portal_pair&) = delete;
				//portal_pair& operator=(const portal_pair&) = delete;

				//// del move and move assignment operator
				//portal_pair(portal_pair&&) = delete;
				//portal_pair& operator=(portal_pair&&) = delete;

				/**
				 * This destroys all remixAPI objects associated with the pair
				 */
				void destroy_pair()
				{
					m_portal0.destroy_mesh();
					m_portal0.destroy_material();
					m_portal1.destroy_mesh();
					m_portal1.destroy_material();
				}

				portal_single& get_portal(uint8_t index)
				{
					switch (index)
					{
						default:
						case 0: return m_portal0;
						case 1: return m_portal1;
					}
					
				}
				portal_single& get_portal0() { return m_portal0; }
				portal_single& get_portal1() { return m_portal1; }

				PORTAL_PAIR get_pair_num() const { return m_pair; }

				/**
				 * Draws the rayportal pair
				 * @return True if no errors occured
				 */
				bool draw_pair()
				{
					bool res = true;
					for (uint8_t i = 0u; i < 2; i++)
					{
						auto& p = get_portal(i);
						if (p.get_mesh())
						{
							/*D3DXMATRIX scale;
							D3DXMatrixScaling(&scale, p.m_scale.x, p.m_scale.y, p.m_scale.z);

							D3DXMATRIX rot_x, rot_y, rot_z;
							D3DXMatrixRotationX(&rot_x, utils::deg_to_rad(p.m_rot.x));
							D3DXMatrixRotationY(&rot_y, utils::deg_to_rad(p.m_rot.y));
							D3DXMatrixRotationZ(&rot_z, utils::deg_to_rad(p.m_rot.z));

							D3DXMATRIX translation = {};
							D3DXMatrixTranslation(&translation, p.m_pos.x, p.m_pos.y, p.m_pos.z);

							D3DXMATRIX final;
							final = scale * rot_z * rot_y * rot_x * translation;

							D3DXMATRIX transp;
							D3DXMatrixTranspose(&transp, &final);

							remixapi_Transform t0 = {};
							t0.matrix[0][0] = transp.m[0][0];
							t0.matrix[0][1] = transp.m[0][1];
							t0.matrix[0][2] = transp.m[0][2];
							t0.matrix[1][0] = transp.m[1][0];
							t0.matrix[1][1] = transp.m[1][1];
							t0.matrix[1][2] = transp.m[1][2];
							t0.matrix[2][0] = transp.m[2][0];
							t0.matrix[2][1] = transp.m[2][1];
							t0.matrix[2][2] = transp.m[2][2];
							t0.matrix[0][3] = transp.m[0][3];
							t0.matrix[1][3] = transp.m[1][3];
							t0.matrix[2][3] = transp.m[2][3];*/

							utils::vector::matrix3x3 mtx;
							mtx.scale(p.m_scale.x, p.m_scale.y, p.m_scale.z);
							mtx.rotate_z(utils::deg_to_rad(p.m_rot.z));
							mtx.rotate_y(utils::deg_to_rad(p.m_rot.y));
							mtx.rotate_x(utils::deg_to_rad(p.m_rot.x));

							// update portal normal
							const auto& normal = p.calculate_normal(mtx);

							// transpose because remix transforms are column major
							mtx.transpose();

							// cache transform so it's not recalculated every frame
							if (!p.is_cached()) {
								p.cache(mtx.to_remixapi_transform(p.m_pos), normal);
							}

							if (api::rayportal_show_debug_info)
							{
								Vector debug_pos = p.m_pos;
								float scaled_offset = debug_pos.DistTo(game::get_current_view_origin_as_vector()) * 0.025f;
								game::debug_add_text_overlay(&debug_pos.x, 0.0f, utils::va("Pair: %d --- Rayportal Index: %d\n", this->get_pair_num(), p.get_index())); debug_pos.z -= scaled_offset;
								game::debug_add_text_overlay(&debug_pos.x, 0.0f, utils::va("Position: %.2f %.2f %.2f", p.m_pos.x, p.m_pos.y, p.m_pos.z)); debug_pos.z -= scaled_offset;
								game::debug_add_text_overlay(&debug_pos.x, 0.0f, utils::va("Normal: %.2f %.2f %.2f", normal.x, normal.y, normal.z));
							}

							const remixapi_InstanceInfo info =
							{
								.sType = REMIXAPI_STRUCT_TYPE_MESH_INFO,
								.pNext = nullptr,
								.categoryFlags = 0,
								.mesh = p.get_mesh(),
								.transform = p.get_remix_transform(),
								.doubleSided = false
							};
							res = api::bridge.DrawInstance(&info) == REMIXAPI_ERROR_CODE_SUCCESS ? res : false;
						}
					}

					return res;
				}

			private:
				PORTAL_PAIR m_pair;
				portal_single m_portal0;
				portal_single m_portal1;
			};

			// constructor for singleton
			rayportal_context() = default;

			/**
			 * Constructs a portal pair that is ready to be drawn. Only adds the pair if it not already exists.\n
			 * A maximum of two pairs is currently supported.
			 * @param pair			Portalpair num
			 * @param pos1			Position of the first portal
			 * @param rot1			Rotation of the first portal
			 * @param scale1		Scale of the first portal
			 * @param square_mask1	Square portal or circle
			 * @param pos2			Position of the second portal
			 * @param rot2			Rotation of the second portal
			 * @param scale2		Scale of the second portal
			 * @param square_mask2	Square portal or circle
			 */
			void add_pair(const PORTAL_PAIR& pair,
			              const Vector& pos1, const Vector& rot1, const Vector& scale1, bool square_mask1,
			              const Vector& pos2, const Vector& rot2, const Vector& scale2, bool square_mask2)
			{
				if (pairs.size() < 2)
				{
					if (!pairs.contains(pair))
					{
						pairs.emplace(pair, portal_pair(pair, 
							pos1, rot1, scale1, square_mask1,
							pos2, rot2, scale2, square_mask2));
					}
				}
			}

			/**
			 * Fully destroy all portal pairs
			 */
			void destroy_all_pairs()
			{

				for (auto& p : pairs) {
					p.second.destroy_pair();
				}

				pairs.clear();
			}

			/**
			 * Fully destroy a specific pair
			 * @param pair The pair destroy
			 */
			void destroy_pair(const PORTAL_PAIR& pair)
			{
				for (auto it = pairs.begin(); it != pairs.end(); ++it)
				{
					if (it->second.get_pair_num() == pair)
					{
						pairs.erase(it);
						break;
					}
				}
			}

			/**
			 * Draws the specified pair if it exists
			 * @param pair	The pair to draw
			 * @return		True if successful
			 */
			bool draw_pair(const PORTAL_PAIR& pair)
			{
				if (pairs.contains(pair))
				{
					return pairs.at(pair).draw_pair();
				}

				return false;
			}

			/**
			 * Draws all portal pairs
			 * @return True if successful
			 */
			bool draw_all_pairs()
			{
				bool res = true;
				for (auto it = pairs.begin(); it != pairs.end(); ++it)
				{
					res = it->second.draw_pair() == REMIXAPI_ERROR_CODE_SUCCESS ? res : false;
				}

				return res;
			}

			struct settings_s
			{
				bool show_debug_information = false;
			};

			settings_s settings = {};

		private:
			std::map<PORTAL_PAIR, portal_pair> pairs;
		};

		extern rayportal_context rayportal_ctx;
	}

	class main_module : public component
	{
	public:
		main_module();
		~main_module();
		const char* get_name() override { return "main_module"; }

		static inline std::uint64_t framecount = 0u;
		static inline LPD3DXFONT d3d_font = nullptr;

		static void debug_draw_box(const VectorAligned& center, const VectorAligned& half_diagonal, const float width, const api::DEBUG_REMIX_LINE_COLOR& color);
		static void setup_required_cvars();

	private:
	};
}
