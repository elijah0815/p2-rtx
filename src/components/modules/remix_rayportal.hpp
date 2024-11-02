#pragma once

namespace components::api
{
	class remix_rayportal : public component
	{
	public:
		remix_rayportal();
		const char* get_name() override { return "remix_rayportal"; }

		static inline remix_rayportal* p_this = nullptr;
		static remix_rayportal* get() { return p_this; }

		enum PORTAL_PAIR : std::uint8_t
		{
			PORTAL_PAIR_1 = 0,
			PORTAL_PAIR_2 = 1,
		};

		// variables directly influencing the class below 
		bool rayportal_show_debug_info = false;

		//class rayportal_context; // forward decl
		//extern rayportal_context rayportal_ctx;

	public:
		class portal_pair; // forward decl

		uint32_t& get_pair_creation_counter() {
			return m_portal_material_creation_counter;
		}

	private:
		uint32_t m_portal_material_creation_counter = 0u;


		// #
		// portal_single

	public:
		class portal_single
		{
		public:
			portal_single() = default;
			portal_single(const Vector& pos, const Vector& rot, const Vector2D& scale, bool square_mask, const portal_pair* parent)
				: m_pos(pos), m_rot(rot), m_square_mask(square_mask), m_parent(parent)
			{
				m_scale.x = scale.x;
				m_scale.y = 1.0f;
				m_scale.z = scale.y;
			}

			bool init(uint8_t index);
			
			void assign_index(uint8_t index) {
				m_index = index;
			}

			uint8_t get_index() const {
				return m_index;
			}

			const portal_pair* get_parent_pair() {
				return m_parent;
			}

			remixapi_MeshHandle get_mesh() const {
				return m_hmesh;
			}

			remixapi_ErrorCode create_mesh();
			remixapi_ErrorCode destroy_mesh();

			remixapi_MaterialHandle get_material() const {
				return m_hmaterial;
			}

			remixapi_ErrorCode create_material();
			remixapi_ErrorCode destroy_material();

			const Vector& calculate_normal(const utils::vector::matrix3x3& matrix);
			const Vector& get_normal() const {
				return m_normal;
			}

			const remixapi_Transform& get_remix_transform() const {
				return m_transform;
			}

			
			const Vector* calculate_corners();

			/**
			 * Get the corner points of the portal. Needs to be calculated before
			 * @return pointer to Vector[4]
			 */
			const Vector* get_corner_points() {
				return m_corner_points;
			}

			/**
			 * Caches the portal transform and normal
			 * @param transform the remixapi_Transform for the portal
			 * @param normal	normal of the portal
			 */
			void cache(const remixapi_Transform& transform)
			{
				m_transform = transform;
				m_cached = true;
			}

			void uncache() {
				m_cached = false;
			}

			bool is_cached() const {
				return m_cached;
			}


			Vector m_pos = {};
			Vector m_rot = {};
			Vector m_scale = {};
			bool m_square_mask = false;

		private:
			uint8_t m_index = 0u;
			const portal_pair* m_parent = nullptr;

			bool m_cached = false; // is transform / normal cached
			Vector m_normal = { 0.0f, -1.0f, 0.0f };
			remixapi_Transform m_transform = {};
			Vector m_corner_points[4] = {};

			remixapi_MeshHandle m_hmesh = nullptr;
			remixapi_MaterialHandle m_hmaterial = nullptr;
		};

		class portal_pair
		{
		public:
			portal_pair(const PORTAL_PAIR& pair,
				const Vector& pos1, const Vector& rot1, const Vector2D& scale1, bool square_mask1,
				const Vector& pos2, const Vector& rot2, const Vector2D& scale2, bool square_mask2)
				: m_pair(pair)
				, m_portal0(pos1, rot1, scale1, square_mask1, this)
				, m_portal1(pos2, rot2, scale2, square_mask2, this)
			{
				switch (pair)
				{
				case PORTAL_PAIR_1:
					m_portal0.init(2);
					m_portal1.init(3);
					get()->get_pair_creation_counter()++;
					break;
				case PORTAL_PAIR_2:
					m_portal0.init(4);
					m_portal1.init(5);
					get()->get_pair_creation_counter()++;
					break;
				}
			}

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

			bool draw_pair();

		private:
			PORTAL_PAIR m_pair;
			portal_single m_portal0;
			portal_single m_portal1;
		};


		// #
		// #


		void add_pair(const PORTAL_PAIR& pair,
			const Vector& pos1, const Vector& rot1, const Vector2D& scale1, bool square_mask1,
			const Vector& pos2, const Vector& rot2, const Vector2D& scale2, bool square_mask2);

		void destroy_all_pairs();
		void destroy_specific_pair(const PORTAL_PAIR& pair);

		bool draw_pair(const PORTAL_PAIR& pair);
		bool draw_all_pairs();

		portal_pair* get_portal_pair(PORTAL_PAIR pair);

		bool empty() const {
			return pairs.empty();
		}

	//private:
		std::map<PORTAL_PAIR, portal_pair> pairs;
	};
}
