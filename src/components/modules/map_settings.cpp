#include "std_include.hpp"

namespace components
{
	void map_settings::set_settings_for_map(const std::string& map_name, bool reload_settings)
	{
		if (m_settings.empty() || reload_settings) {
			map_settings::load_settings();
		}

		map_settings::loaded_map_name = !map_name.empty() ? map_name : game::get_map_name();
		utils::replace_all(map_settings::loaded_map_name, std::string("maps/"), "");		// if sp map
		utils::replace_all(map_settings::loaded_map_name, std::string(".bsp"), "");

		if (api::m_initialized)
		{
			// resets all modified variables back to rtx.conf level
			remix_vars::get()->reset_all_modified();

			// auto apply {map_name}.conf (if it exists)
			open_and_set_var_config(map_settings::loaded_map_name + ".conf");
		}

		// get settings for loaded map
		for (auto& s : m_settings)
		{
			if (s.mapname == map_settings::loaded_map_name)
			{
				// found map settings - 
				m_loaded_map_settings = &s;

				// cannot spawn markers in here - too early

				if (api::m_initialized)
				{
					// apply other manually defined configs
					for (const auto& f : s.api_var_configs) {
						open_and_set_var_config(f);
					}
				}

				break;
			}
		}
	}

	// cannot be called in the current on_map_load stub (too early)
	// called from 'once_per_frame_cb()' instead
	void map_settings::spawn_markers_once()
	{
		if (auto s = get_loaded_map_settings(); s)
		{
			// only spawn markers once
			if (map_settings::m_spawned_markers) {
				return;
			}

			map_settings::m_spawned_markers = true;

			// spawn map markers
			for (auto i = 0u; i < s->map_markers.size(); i++)
			{
				auto& m = s->map_markers[i];
				if (m.active)
				{
					if (i > 99) {
						continue; // safety check
					}

					const auto mdl_num = i / 10u;
					const auto skin_num = i % 10u;

					const auto model_name = utils::va("models/props_xo/mapmarker%03d.mdl", mdl_num * 10);

					// #OFFSET
					void* mdlcache = reinterpret_cast<void*>(*(DWORD*)(SERVER_BASE + 0x8618FC));

					// mdlcache->BeginLock
					utils::hook::call_virtual<30, void>(mdlcache);

					// mdlcache->FindMDL
					const auto mdl_handle = utils::hook::call_virtual<9, std::uint16_t>(mdlcache, model_name);
					if (mdl_handle != 0xFFFF)
					{
						// #OFFSET
						// save precache state - CBaseEntity::m_bAllowPrecache
						const bool old_precache_state = *reinterpret_cast<bool*>(SERVER_BASE + 0x7B2C58);

						// #OFFSET
						// allow precaching - CBaseEntity::m_bAllowPrecache
						*reinterpret_cast<bool*>(SERVER_BASE + 0x7B2C58) = true;

						// CreateEntityByName - CBaseEntity *__cdecl CreateEntityByName(const char *className, int iForceEdictIndex, bool bNotify)
						m.handle = utils::hook::call<void* (__cdecl)(const char* className, int iForceEdictIndex, bool bNotify)>(SERVER_BASE + 0x19A090)
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
							utils::hook::call<void(__cdecl)(void* pEntity, bool bRunVScripts)>(SERVER_BASE + 0x279480)
								(m.handle, true);

							// ent->Activate
							utils::hook::call_virtual<37, void>(m.handle);
						}
						else {
							m.active = false;
						}

						// #OFFSET
						// restore precaching state - CBaseEntity::m_bAllowPrecache
						*reinterpret_cast<bool*>(SERVER_BASE + 0x7B2C58) = old_precache_state;
					}
					else {
						m.active = false;
					}

					utils::hook::call_virtual<31, void>(mdlcache); // mdlcache->EndLock
				}
			}
		}
	}

	void map_settings::on_map_exit()
	{
		if (auto* s = map_settings::get_loaded_map_settings(); s)
		{
			// destroy active markers
			for (auto& m : s->map_markers)
			{
				if (m.handle) 
				{
					game::cbaseentity_remove(m.handle);
					m.handle = nullptr;
				}
			}
		}

		map_settings::clear_loaded_map_settings();
		map_settings::m_spawned_markers = false;
	}

	bool map_settings::load_settings()
	{
		m_settings.clear();
		m_settings.reserve(32);

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

				if (input.starts_with("#CULL"))
				{
					parse_mode = CULL;
					continue;
				}

				if (input.starts_with("#MARKER"))
				{
					parse_mode = MARKER;
					continue;
				}

				if (input.starts_with("#API_CONFIGVARS"))
				{
					parse_mode = API_VARS;
					continue;
				}

				// split string on ','
				m_args = utils::split(input, ',');

				switch (parse_mode)
				{
				case CULL:
					parse_culling();
					break;
				case MARKER:
					parse_markers();
					break;
				case API_VARS:
					parse_api_var_configs();
					break;
				}
			}

			file.close();
			return true;
		}

		return false;
	}

	map_settings::map_settings_s* map_settings::get_or_create_settings(bool parse_mode, const char* map_name)
	{
		// check if map settings exist
		for (auto& e : m_settings)
		{
			if (e.mapname._Equal(parse_mode ? m_args[0] : map_name)) {
				return &e;
			}
		}

		// create defaults if not
		m_settings.push_back(map_settings_s(parse_mode ? m_args[0] : map_name));
		return &m_settings.back();
	}

	void map_settings::parse_culling()
	{
		if (map_settings_s* s = get_or_create_settings(); s)
		{
			s->area_settings.clear();

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
					if (s->area_settings.contains(leaf_idx)) {
						continue;
					}

					const auto elem = s->area_settings.emplace(leaf_idx, std::unordered_set<std::uint32_t>()).first;

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
		if (map_settings_s* s = get_or_create_settings(); s)
		{
			// destroy active markers first
			for (auto& m : s->map_markers)
			{
				if (m.handle) 
				{
					game::cbaseentity_remove(m.handle);
					m.handle = nullptr;
				}
			}

			s->map_markers.clear();
			s->map_markers.resize(100);

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
					const auto m = &s->map_markers[marker_index];

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
		if (map_settings_s* s = get_or_create_settings(); s)
		{
			s->api_var_configs.clear();
			for (auto a = 1u; a < m_args.size(); a++)
			{
				auto str = m_args[a];
				if (str.empty()) {
					continue;
				}

				utils::trim(str);
				s->api_var_configs.emplace_back(str);
			}
		}
	}


	void map_settings::on_map_load(const std::string& map_name)
	{
		map_settings::get()->set_settings_for_map(map_name);
	}

	ConCommand xo_mapsettings_update{};

	void xo_mapsettings_update_fn()
	{
		map_settings::get()->set_settings_for_map("", true);
	}

	map_settings::map_settings()
	{
		game::con_add_command(&xo_mapsettings_update, "xo_mapsettings_update", xo_mapsettings_update_fn, "Reloads the map_settings.ini file");
	}
}