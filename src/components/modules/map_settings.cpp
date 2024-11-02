#include "std_include.hpp"

namespace components
{
	void map_settings::set_settings_for_map(const std::string& map_name)
	{
		m_map_settings.mapname = !map_name.empty() ? map_name : game::get_map_name();
		utils::replace_all(m_map_settings.mapname, std::string("maps/"), "");		// if sp map
		utils::replace_all(m_map_settings.mapname, std::string(".bsp"), "");

		parse_ini();

		static bool disable_map_configs = flags::has_flag("xo_disable_map_conf");
		if (api::m_initialized && !disable_map_configs)
		{
			// resets all modified variables back to rtx.conf level
			remix_vars::reset_all_modified();

			// auto apply {map_name}.conf (if it exists)
			open_and_set_var_config(m_map_settings.mapname + ".conf");

			// apply other manually defined configs
			for (const auto& f : m_map_settings.api_var_configs) {
				open_and_set_var_config(f);
			}
		}
	}

	// cannot be called in the current on_map_load stub (too early)
	// called from 'once_per_frame_cb()' instead
	void map_settings::spawn_markers_once()
	{
		// only spawn markers once
		if (m_spawned_markers) {
			return;
		}

		m_spawned_markers = true;

		// spawn map markers
		for (auto i = 0u; i < m_map_settings.map_markers.size(); i++)
		{
			auto& m = m_map_settings.map_markers[i];
			if (m.active)
			{
				if (i > 99) {
					continue; // safety check
				}

				const auto mdl_num = i / 10u;
				const auto skin_num = i % 10u;

				const auto model_name = utils::va("models/props_xo/mapmarker%03d.mdl", mdl_num * 10);

				void* mdlcache = reinterpret_cast<void*>(*(DWORD*)(SERVER_BASE + USE_OFFSET(0x86B07C, 0x8618FC)));

				// mdlcache->BeginLock
				utils::hook::call_virtual<30, void>(mdlcache);

				// mdlcache->FindMDL
				const auto mdl_handle = utils::hook::call_virtual<9, std::uint16_t>(mdlcache, model_name);
				if (mdl_handle != 0xFFFF)
				{
					// save precache state - CBaseEntity::m_bAllowPrecache
					const bool old_precache_state = *reinterpret_cast<bool*>(SERVER_BASE + USE_OFFSET(0x7BC2B0, 0x7B2C58));

					// allow precaching - CBaseEntity::m_bAllowPrecache
					*reinterpret_cast<bool*>(SERVER_BASE + USE_OFFSET(0x7BC2B0, 0x7B2C58)) = true;

					// CreateEntityByName - CBaseEntity *__cdecl CreateEntityByName(const char *className, int iForceEdictIndex, bool bNotify)
					m.handle = utils::hook::call<void* (__cdecl)(const char* className, int iForceEdictIndex, bool bNotify)>(SERVER_BASE + USE_OFFSET(0x19F2C0, 0x19A090))
						("dynamic_prop", -1, true);

					if (m.handle)
					{
						// ent->KeyValue
						utils::hook::call_virtual<35, void>(m.handle, "origin", utils::va("%.10f %.10f %.10f", m.origin[0], m.origin[1], m.origin[2]));
						utils::hook::call_virtual<35, void>(m.handle, "model", model_name);
						utils::hook::call_virtual<35, void>(m.handle, "solid", "2");

						struct skin_offset
						{
							char pad[0x37C];
							int m_nSkin;
						}; STATIC_ASSERT_OFFSET(skin_offset, m_nSkin, 0x37C);

						auto skin_val = static_cast<skin_offset*>(m.handle);
						skin_val->m_nSkin = skin_num;

						// ent->Precache
						utils::hook::call_virtual<25, void>(m.handle);

						// DispatchSpawn
						utils::hook::call<void(__cdecl)(void* pEntity, bool bRunVScripts)>(SERVER_BASE + USE_OFFSET(0x27F520, 0x279480))
							(m.handle, true);

						// ent->Activate
						utils::hook::call_virtual<37, void>(m.handle);
					}
					else {
						m.active = false;
					}

					// restore precaching state - CBaseEntity::m_bAllowPrecache
					*reinterpret_cast<bool*>(SERVER_BASE + USE_OFFSET(0x7BC2B0, 0x7B2C58)) = old_precache_state;
				}
				else {
					m.active = false;
				}

				utils::hook::call_virtual<31, void>(mdlcache); // mdlcache->EndLock
			}
		}
	}

	void map_settings::destroy_markers()
	{
		// destroy active markers
		for (auto& m : m_map_settings.map_markers)
		{
			if (m.handle)
			{
				game::cbaseentity_remove(m.handle);
				m.handle = nullptr;
			}
		}

		m_map_settings.map_markers.clear();
		m_spawned_markers = false;
	}

	void map_settings::on_map_exit()
	{
		api::remix_rayportal::get()->destroy_all_pairs();
		clear_map_settings();
	}

	bool map_settings::parse_ini()
	{
		std::ifstream file;
		if (utils::open_file_homepath("portal2-rtx", "map_settings.ini", file))
		{
			std::string input;
			auto parse_mode = PARSE_MODE::CULL;

			// read line by line
			while (std::getline(file, input))
			{
				// ignore comment
				if (input.starts_with("//")) {
					continue;
				}

				if (input.empty()) {
					continue;
				}

				if (input.starts_with("#FOG")) {
					parse_mode = FOG; continue;
				}

				if (input.starts_with("#API_PORTAL_PAIRS")) {
					parse_mode = API_PORTALS; continue;
				}

				if (input.starts_with("#CULL")) {
					parse_mode = CULL; continue;
				}

				if (input.starts_with("#MARKER")) {
					parse_mode = MARKER; continue;
				}

				if (input.starts_with("#API_CONFIGVARS")) {
					parse_mode = API_VARS; continue;
				}

				// split string on ','
				m_args = utils::split(input, ',');

				switch (parse_mode)
				{
				case FOG:
					parse_fog(); break;

				case API_PORTALS: 
					parse_portal_pairs(); break;

				case CULL: 
					parse_culling(); break;

				case MARKER: 
					parse_markers(); break;

				case API_VARS:
					parse_api_var_configs(); break;
				}
			}

			file.close();
			return true;
		}

		return false;
	}

	bool map_settings::matches_map_name()
	{
		return utils::str_to_lower(m_args[0]) == m_map_settings.mapname;
	}

	void map_settings::parse_fog()
	{
		if (matches_map_name())
		{
			m_map_settings.fog_dist = utils::try_stof(m_args[1], 0.0f);
			m_map_settings.fog_color = D3DCOLOR_XRGB
			(
				utils::try_stoi(m_args[2], 255),
				utils::try_stoi(m_args[3], 255),
				utils::try_stoi(m_args[4], 255)
			);
		}
	}

	void map_settings::parse_portal_pairs()
	{
		if (matches_map_name())
		{
			std::unordered_set<int> added_pairs;

			// for each portal pair
			for (auto a = 1u; a < m_args.size(); a++)
			{
				const auto& str = m_args[a];

				if (str.empty()) {
					continue;
				}

				if (!utils::has_matching_symbols(str, '[', ']', true) || !utils::has_matching_symbols(str, '(', ')', false) || !utils::has_matching_symbols(str, '{', '}', false)) {
					continue;
				}

				// which pair are we writing settings for?
				if (const auto	pair_index = utils::try_stoi(utils::split_string_between_delims(str, '[', ']'), -1);
					pair_index >= 0)
				{
					// ignore duplicate pairs or pairs that are out of bounds (3rd pair and up)
					if (added_pairs.contains(pair_index) || pair_index > 2) {
						continue;
					}

					// track added pairs
					added_pairs.emplace(pair_index);

					// string holding definition of the two portals of the pair
					const auto pair_creation_string = utils::split_string_between_delims(str, '(', ')');

					// split into two separate portals
					const auto individual_portals = utils::split(pair_creation_string, '&');

					if (individual_portals.size() == 2)
					{
						{
							// split portal 0 args
							auto p0_args = utils::split(individual_portals[0], '}');
							if (p0_args.size() == 4)
							{
								// remove starting {
								for (auto& arg : p0_args) 
								{
									// remove trailing and leading spaces
									arg = utils::trim(arg);

									if (arg.starts_with('{')) {
										utils::erase_substring(arg, "{");
									}
								}

								Vector p0_pos, p0_rot, p1_pos, p1_rot;
								Vector2D p0_scale, p1_scale;

								{	// portal0
									const auto p0_pos_str = utils::split(p0_args[0], ' ');		if (p0_pos_str.size() != 3) continue;
									const auto p0_rot_str = utils::split(p0_args[1], ' ');		if (p0_rot_str.size() != 3) continue;
									const auto p0_scale_str = utils::split(p0_args[2], ' ');	if (p0_scale_str.size() != 2) continue;

									p0_pos = { utils::try_stof(p0_pos_str[0]), utils::try_stof(p0_pos_str[1]), utils::try_stof(p0_pos_str[2]) };
									p0_rot = { utils::try_stof(p0_rot_str[0]), utils::try_stof(p0_rot_str[1]), utils::try_stof(p0_rot_str[2]) };
									p0_scale = { utils::try_stof(p0_scale_str[0]), utils::try_stof(p0_scale_str[1]) };
								}
								
								const auto p0_mask = utils::try_stoi(p0_args[3], 1);

								{
									auto p1_args = utils::split(individual_portals[1], '}');
									if (p1_args.size() == 4)
									{
										// remove starting {
										for (auto& arg : p1_args) 
										{
											// remove trailing and leading spaces
											arg = utils::trim(arg);

											if (arg.starts_with("{")) {
												utils::erase_substring(arg, "{");
											}
										}

										{	// portal1
											const auto p1_pos_str = utils::split(p1_args[0], ' ');		if (p1_pos_str.size() != 3) continue;
											const auto p1_rot_str = utils::split(p1_args[1], ' ');		if (p1_rot_str.size() != 3) continue;
											const auto p1_scale_str = utils::split(p1_args[2], ' ');	if (p1_scale_str.size() != 2) continue;

											p1_pos = { utils::try_stof(p1_pos_str[0]), utils::try_stof(p1_pos_str[1]), utils::try_stof(p1_pos_str[2]) };
											p1_rot = { utils::try_stof(p1_rot_str[0]), utils::try_stof(p1_rot_str[1]), utils::try_stof(p1_rot_str[2]) };
											p1_scale = { utils::try_stof(p1_scale_str[0]), utils::try_stof(p1_scale_str[1]) };
										}
										
										const auto p1_mask = utils::try_stoi(p1_args[3], 1);

										// finally add the pair
										api::remix_rayportal::get()->add_pair((api::remix_rayportal::PORTAL_PAIR)pair_index,
											p0_pos, p0_rot, p0_scale, p0_mask,
											p1_pos, p1_rot, p1_scale, p1_mask);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void map_settings::parse_culling()
	{
		if (matches_map_name())
		{
			m_map_settings.area_settings.clear();

			// for each area with it's forced leafs with format -> [cell](index index index)
			for (auto a = 1u; a < m_args.size(); a++)
			{
				const auto& str = m_args[a];

				if (str.empty()) {
					continue;
				}

				if (!utils::has_matching_symbols(str, '[', ']', true) || !utils::has_matching_symbols(str, '(', ')', true)) {
					continue;
				}

				// which leaf are we writing settings for?
				if (const auto	leaf_idx = utils::try_stoi(utils::split_string_between_delims(str, '[', ']'), -1); 
								leaf_idx >= 0)
				{
					// ignore duplicate leafs
					if (m_map_settings.area_settings.contains(leaf_idx)) {
						continue;
					}

					const auto elem = m_map_settings.area_settings.emplace(leaf_idx, std::unordered_set<std::uint32_t>()).first;

					// get leaf inidices
					const auto indices_str = utils::split_string_between_delims(str, '(', ')');
					const auto split_indices = utils::split(indices_str, ' ');

					for (const auto& i : split_indices)
					{
						if (const auto n = utils::try_stoi(i, -1); n >= 0) {
							elem->second.emplace(n);
						}
					}
				}
			}
		}
	}

	void map_settings::parse_markers()
	{
		if (matches_map_name())
		{
			destroy_markers();
			m_map_settings.map_markers.resize(100);

			for (auto a = 1u; a < m_args.size(); a++)
			{
				const auto& str = m_args[a];

				if (str.empty()) {
					continue;
				}

				// which marker are we writing settings for?
				const auto marker_index = utils::try_stoi(utils::split_string_between_delims(str, '[', ']'), -1);
				if (marker_index >= 0)
				{
					// limit the vector to 100 entries
					if (marker_index >= 100) {
						continue;
					}

					// get marker
					const auto m = &m_map_settings.map_markers[marker_index];

					// ignore duplicate markers
					if (m->active) {
						continue;
					}

					// get and assign origin
					const auto origin_str = utils::split_string_between_delims(str, '(', ')');
					if (const auto	xyz = utils::split(origin_str, ' ');
									xyz.size() == 3u)
					{
						m->origin[0] = utils::try_stof(xyz[0], 0.0f);
						m->origin[1] = utils::try_stof(xyz[1], 0.0f);
						m->origin[2] = utils::try_stof(xyz[2], 0.0f);
						m->active = true;
					}
				}
			}
		}
	}

	void map_settings::open_and_set_var_config(const std::string& config, const bool ignore_hashes, const char* custom_path)
	{
		std::string path = "portal2-rtx\\map_configs";
		if (custom_path)
		{
			path = custom_path;
		}

		std::ifstream file;
		if (utils::open_file_homepath(path, config, file))
		{
			std::string input;
			while (std::getline(file, input))
			{
				if (utils::starts_with(input, "#")) {
					continue;
				}

				if (auto pair = utils::split(input, '=');
					pair.size() == 2u)
				{
					utils::trim(pair[0]);
					utils::trim(pair[1]);

					if (ignore_hashes && pair[1].starts_with("0x")) {
						continue;
					}

					if (pair[1].empty()) {
						continue;
					}

					if (const auto o = remix_vars::get_option(pair[0].c_str()); o)
					{
						const auto& v = remix_vars::string_to_option_value(o->second.type, pair[1]);
						remix_vars::set_option(o, v, true);
					}
				}
			}

			file.close();
		}
	}

	void map_settings::parse_api_var_configs()
	{
		if (matches_map_name())
		{
			m_map_settings.api_var_configs.clear();
			for (auto a = 1u; a < m_args.size(); a++)
			{
				auto str = m_args[a];
				if (str.empty()) {
					continue;
				}

				utils::trim(str);
				m_map_settings.api_var_configs.emplace_back(str);
			}
		}
	}

	void map_settings::on_map_load(const std::string& map_name)
	{
		map_settings::get()->set_settings_for_map(map_name);
	}

	ConCommand xo_mapsettings_update {};
	void xo_mapsettings_update_fn()
	{
		api::remix_rayportal::get()->destroy_all_pairs();
		map_settings::get()->destroy_markers();
		map_settings::get()->set_settings_for_map("");
	}

	map_settings::map_settings()
	{
		game::con_add_command(&xo_mapsettings_update, "xo_mapsettings_update", xo_mapsettings_update_fn, "Reloads the map_settings.ini file + map.conf");
	}
}