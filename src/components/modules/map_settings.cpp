#include "std_include.hpp"

namespace components
{
	void map_settings::set_settings_for_loaded_map(bool reload_settings)
	{
		if ((m_settings.empty() || reload_settings) && !map_settings::load_settings())
		{
			return;
		}

		{
			std::string map_name = game::get_map_name();
			//utils::replace_all(map_name, std::string("maps/mp/"), "");	// if mp map
			utils::replace_all(map_name, std::string("maps/"), "");		// if sp map
			utils::replace_all(map_name, std::string(".bsp"), "");

			for (const auto& s : m_settings)
			{
				if (s.mapname == map_name)
				{
					m_loaded_map_settings = s;

					// spawn map markers
					/*for (auto i = 0u; i < s.map_markers.size(); i++)
					{
						if (s.map_markers[i].active)
						{
							if (const auto fx = game::DB_FindXAssetHeader(game::XAssetType::ASSET_TYPE_FX, utils::va("rtx/markers/rtx_marker_%02d", i)).fx; fx)
							{
								m_loaded_map_settings.map_markers[i].handle = game::sp::FX_SpawnOrientedEffect(game::IDENTITY_AXIS[0], fx, 0, &s.map_markers[i].origin[0]);
							}
						}
					}*/

					//if (main_module::m_initialized)
					//{
					//	// resets all modified variables back to rtx.conf level
					//	remix_vars::get()->reset_all_modified();

					//	// auto apply {map_name}.conf (if it exists)
					//	open_and_set_var_config(s.mapname + ".conf");

					//	// apply other manually defined configs
					//	for (const auto& f : s.api_var_configs)
					//	{
					//		open_and_set_var_config(f);
					//	}
					//}
					break;
				}
			}
		}
	}

	map_settings::map_settings_s* map_settings::get_or_create_settings(bool parse_mode, const char* map_name)
	{
		// check if map settings exist
		for (auto& e : m_settings)
		{
			if (e.mapname._Equal(parse_mode ? m_args[0] : map_name))
			{
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

				if (str.empty())
				{
					continue;
				}

				if (!utils::has_matching_symbols(str, '[', ']', true) || !utils::has_matching_symbols(str, '(', ')', true))
				{
					continue;
				}

				// which leaf are we writing settings for?
				if (const auto	leaf_idx = utils::try_stoi(utils::split_string_between_delims(str, '[', ']'), -1); 
								leaf_idx >= 0)
				{
					// ignore duplicate leafs
					if (s->area_settings.contains(leaf_idx))
					{
						continue;
					}

					const auto elem = s->area_settings.emplace(leaf_idx, std::unordered_set<std::uint32_t>()).first;

					// get leaf inidices
					const auto indices_str = utils::split_string_between_delims(str, '(', ')');
					const auto split_indices = utils::split(indices_str, ' ');

					for (const auto& i : split_indices)
					{
						if (const auto n = utils::try_stoi(i, -1); n >= 0)
						{
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
			{
				// destroy active markers first
			}

			s->map_markers.clear();
			s->map_markers.resize(100);

			for (auto a = 1u; a < m_args.size(); a++)
			{
				const auto& str = m_args[a];

				if (str.empty())
				{
					// print msg here
					continue;
				}

				// which marker are we writing settings for?
				const auto marker_index = utils::try_stoi(utils::split_string_between_delims(str, '[', ']'), -1);
				if (marker_index >= 0)
				{
					// limit the vector to 100 entries
					if (marker_index >= 100)
					{
						continue;
					}

					// get marker
					const auto m = &s->map_markers[marker_index];

					// ignore duplicate markers
					if (m->active)
					{
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

#if 0
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
				if (utils::starts_with(input, "#"))
				{
					continue;
				}

				if (auto pair = utils::split(input, '=');
					pair.size() == 2u)
				{
					utils::trim(pair[0]);
					utils::trim(pair[1]);

					if (ignore_hashes && pair[1].starts_with("0x"))
					{
						continue;
					}

					if (pair[1].empty())
					{
						continue;
					}

					//sp::api::bridge.DebugPrint(utils::va("Set config var: %s to: %s", pair[0].c_str(), pair[1].c_str()));
					//
					//DEBUG_PRINT(utils::va("[DEBUG] Set rtx var: %s to: %s\n", pair[0].c_str(), pair[1].c_str()));
					//sp::api::bridge.SetConfigVariable(pair[0].c_str(), pair[1].c_str());

					const auto& vars = sp::remix_vars::get();
					const auto o = vars->get_option(pair[0].c_str());

					if (o)
					{
						const auto& v = vars->string_to_option_value(o->second.type, pair[1]);
						vars->set_option(o, v, true);
					}
				}
			}

			file.close();
		}
	}
#endif

	void map_settings::parse_api_var_configs()
	{
		if (map_settings_s* s = get_or_create_settings(); s)
		{
			s->api_var_configs.clear();
			for (auto a = 1u; a < m_args.size(); a++)
			{
				auto str = m_args[a];
				if (str.empty())
				{
					continue;
				}

				utils::trim(str);
				s->api_var_configs.emplace_back(str);
			}
		}
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
				if (input.starts_with("//"))
				{
					continue;
				}

				if (input.empty())
				{
					continue;
				}

				if (input.starts_with("#CULL"))
				{
					parse_mode = CULL;
					continue;
				}

				/*if (input.starts_with("#MARKER"))
				{
					parse_mode = MARKER;
					continue;
				}*/

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

	// TODO - call
	void map_settings::on_map_load()
	{
		map_settings::get()->set_settings_for_loaded_map();
	}

	ConCommand xo_mapsettings_update{};

	void xo_mapsettings_update_fn()
	{
		map_settings::get()->set_settings_for_loaded_map(true);
	}

	map_settings::map_settings()
	{
		game::con_add_command(&xo_mapsettings_update, "xo_mapsettings_update", xo_mapsettings_update_fn, "Reloads the map_settings.ini file");
	}
}