#include "std_include.hpp"
#include "remix_vars.hpp"

namespace components::api
{
	/**
	 * Initiates a full rayportal via the api (material and mesh) (1x1 Unit)
	 * @param index	The actual rayportal index (0/1 are reserverd for player/level portals)
	 * @return		Returns true if successful
	 */
	bool remix_rayportal::portal_single::init(const uint8_t index)
	{
		this->assign_index(index);

		bool result;
		result = this->create_material() == REMIXAPI_ERROR_CODE_SUCCESS;
		result = this->create_mesh() == REMIXAPI_ERROR_CODE_SUCCESS ? result : false;
		return result;
	}

	remixapi_ErrorCode remix_rayportal::portal_single::create_mesh()
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

			.hash = utils::string_hash64(utils::va("pmesh%d%d", m_index, get()->get_pair_creation_counter())),
			.surfaces_values = &triangles,
			.surfaces_count = 1,
		};

		return api::bridge.CreateMesh(&i, &m_hmesh);
	}

	remixapi_ErrorCode remix_rayportal::portal_single::destroy_mesh()
	{
		const auto res = api::bridge.DestroyMesh(m_hmesh);
		m_hmesh = nullptr;
		return res;
	}

	remixapi_ErrorCode remix_rayportal::portal_single::create_material()
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
		// we have to use a new hash when we re-create the material or the mask setting wont apply (remix material is cached?)
		info.hash = utils::string_hash64(utils::va("pmat%d%d", m_index, get()->get_pair_creation_counter()));
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

	remixapi_ErrorCode remix_rayportal::portal_single::destroy_material()
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
	const Vector& remix_rayportal::portal_single::calculate_normal(const utils::vector::matrix3x3& matrix)
	{
		// local space normal of the portal quad
		const Vector initial_normal = { 0.0f, -1.0f, 0.0f };

		Vector new_normal;
		new_normal[0] = matrix.m[0][0] * initial_normal.x + matrix.m[1][0] * initial_normal.y + matrix.m[2][0] * initial_normal.z;
		new_normal[1] = matrix.m[0][1] * initial_normal.x + matrix.m[1][1] * initial_normal.y + matrix.m[2][1] * initial_normal.z;
		new_normal[2] = matrix.m[0][2] * initial_normal.x + matrix.m[1][2] * initial_normal.y + matrix.m[2][2] * initial_normal.z;
		new_normal.NormalizeChecked();
		m_normal = new_normal;
		return m_normal;
	}

	/**
	 * Calculates corner points of the portal using the portal normal, its position and the scale it was created with
	 * @return pointer to Vector[4]
	 */
	const Vector* remix_rayportal::portal_single::calculate_corners()
	{
		Vector right, up;
		const auto& plane_normal = get_normal();

		// find an arbitrary vector not parallel to the normal
		if (fabs(plane_normal.z) < 0.99f)
		{
			right = Vector(0.0f, 0.0f, 1.0f).Cross(plane_normal);
			right.Normalize();
		}
		else
		{
			right = Vector(1.0f, 0.0f, 0.0f).Cross(plane_normal);
			right.Normalize();
		}

		// calc the up vector perpendicular to both the normal and right vec
		up = plane_normal.Cross(right);
		up.Normalize();

		// scale the vectors to half the rectangle's width and height
		right *= m_scale.x * 0.5f;
		up *= m_scale.z * 0.5f;

		m_corner_points[0] = m_pos + right + up; // top right
		m_corner_points[1] = m_pos - right + up; // top left
		m_corner_points[2] = m_pos - right - up; // bottom left
		m_corner_points[3] = m_pos + right - up; // bottom right

		return m_corner_points;
	}


	// #
	// #

	/**
	 * Draws the rayportal pair
	 * @return True if no errors occured
	 */
	bool remix_rayportal::portal_pair::draw_pair()
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

				if (!p.is_cached())
				{
					utils::vector::matrix3x3 mtx;
					mtx.scale(p.m_scale.x, p.m_scale.y, p.m_scale.z);
					mtx.rotate_z(utils::deg_to_rad(p.m_rot.z));
					mtx.rotate_y(utils::deg_to_rad(p.m_rot.y));
					mtx.rotate_x(utils::deg_to_rad(p.m_rot.x));

					// calculate portal normal if not cached
					p.calculate_normal(mtx);

					// calculate portal corner positions if not cached
					p.calculate_corners();

					// transpose because remix transforms are column major
					mtx.transpose();

					// refactor this
					p.cache(mtx.to_remixapi_transform(p.m_pos));
				}

				if (get()->rayportal_show_debug_info)
				{
					const auto& normal = p.get_normal();

					Vector debug_pos = p.m_pos;
					float scaled_offset = debug_pos.DistTo(*game::get_current_view_origin()) * 0.025f;
					game::debug_add_text_overlay(&debug_pos.x, 0.0f, utils::va("Pair: %d --- Rayportal Index: %d\n", this->get_pair_num(), p.get_index())); debug_pos.z -= scaled_offset;
					game::debug_add_text_overlay(&debug_pos.x, 0.0f, utils::va("Position: %.2f %.2f %.2f", p.m_pos.x, p.m_pos.y, p.m_pos.z)); debug_pos.z -= scaled_offset;
					game::debug_add_text_overlay(&debug_pos.x, 0.0f, utils::va("Normal: %.2f %.2f %.2f", normal.x, normal.y, normal.z));

					const auto corner_points = p.get_corner_points();
					api::add_debug_line(corner_points[0], corner_points[1], 1.0f, api::DEBUG_REMIX_LINE_COLOR::RED);
					api::add_debug_line(corner_points[1], corner_points[2], 1.0f, api::DEBUG_REMIX_LINE_COLOR::RED);
					api::add_debug_line(corner_points[2], corner_points[3], 1.0f, api::DEBUG_REMIX_LINE_COLOR::RED);
					api::add_debug_line(corner_points[3], corner_points[0], 1.0f, api::DEBUG_REMIX_LINE_COLOR::RED);
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


	// #
	// #

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
	void remix_rayportal::add_pair(const PORTAL_PAIR& pair,
		const Vector& pos1, const Vector& rot1, const Vector2D& scale1, bool square_mask1,
		const Vector& pos2, const Vector& rot2, const Vector2D& scale2, bool square_mask2)
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
	void remix_rayportal::destroy_all_pairs()
	{
		for (auto& p : pairs) {
			p.second.destroy_pair();
		}

		pairs.clear();
	}

	/**
	 * Fully destroy a specific pair
	 * @param pair The pair to destroy
	 */
	void remix_rayportal::destroy_specific_pair(const PORTAL_PAIR& pair)
	{
		for (auto it = pairs.begin(); it != pairs.end(); ++it)
		{
			if (it->second.get_pair_num() == pair)
			{
				it->second.destroy_pair();
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
	bool remix_rayportal::draw_pair(const PORTAL_PAIR& pair)
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
	bool remix_rayportal::draw_all_pairs()
	{
		bool res = true;
		for (auto it = pairs.begin(); it != pairs.end(); ++it)
		{
			res = it->second.draw_pair() == REMIXAPI_ERROR_CODE_SUCCESS ? res : false;
		}

		return res;
	}

	/**
	 * Returns the specified portal pair if it exists
	 * @param pair	the portal pair
	 * @return		a pointer to the portal pair - nullptr if not found
	 */
	remix_rayportal::portal_pair* remix_rayportal::get_portal_pair(PORTAL_PAIR pair)
	{
		for (auto it = pairs.begin(); it != pairs.end(); ++it)
		{
			if (it->second.get_pair_num() == pair) {
				return &it->second;
			}
		}

		return nullptr;
	}


	// #
	// #

	ConCommand xo_debug_toggle_rayportal_info_cmd{};
	void xo_debug_toggle_rayportal_info_fn()
	{
		remix_rayportal::get()->rayportal_show_debug_info = !remix_rayportal::get()->rayportal_show_debug_info;
	}

	remix_rayportal::remix_rayportal()
	{
		p_this = this;
		rayportal_show_debug_info = false;

		// #
		// commands

		game::con_add_command(&xo_debug_toggle_rayportal_info_cmd, "xo_debug_toggle_rayportal_info", xo_debug_toggle_rayportal_info_fn, "Toggle debug information for rayportals spawned via the remix api");
	}
}
