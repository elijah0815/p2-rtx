#include "std_include.hpp"

namespace components
{
	void game_settings::write_toml()
	{
		std::ofstream file;
		file.open((game::root_path + "portal2-rtx\\game_settings.toml").c_str());

		file << "# This file is autogenerated. Valid modifications of settings are considered and will be written back to the file." << std::endl << std::endl;

		const std::uint32_t setting_count = sizeof(var_definitions) / sizeof(variable);
		for (auto s = 0u; s < setting_count; s++)
		{
			auto var = reinterpret_cast<variable*>(reinterpret_cast<char*>(&vars) + s * sizeof(variable));

			file << "# " << var->m_desc << std::endl;
			file << "# Type: " << var->get_str_type() << " || Default: " << var->get_str_value(true) << std::endl;
			file << var->m_name << " = " << var->get_str_value() << std::endl << std::endl;
		}

		file.close();
	}

	bool game_settings::parse_toml()
	{
		std::ifstream file;
		if (utils::open_file_homepath("portal2-rtx\\", "game_settings.toml", file))
		{
			// file exists
			file.close();

			try
			{
				auto config = toml::parse("portal2-rtx\\game_settings.toml");

				// #
				auto to_float = [](const toml::value& entry, float default_setting = 0.0f)
					{
						if (entry.is_floating()) {
							return static_cast<float>(entry.as_floating());
						}

						if (entry.is_integer()) {
							return static_cast<float>(entry.as_integer());
						}

						try { // this will fail and let the user know whats wrong
							return static_cast<float>(entry.as_floating());
						}
						catch (toml::type_error& err) {
							game::console(); printf("%s\n", err.what());
						}

						return default_setting;
					};

				// #
				auto to_int = [](const toml::value& entry, int default_setting = 0)
					{
						if (entry.is_floating()) {
							return static_cast<int>(entry.as_floating());
						}

						if (entry.is_integer()) {
							return static_cast<int>(entry.as_integer());
						}

						try { // this will fail and let the user know whats wrong
							return static_cast<int>(entry.as_integer());
						}
						catch (toml::type_error& err) {
							game::console(); printf("%s\n", err.what());
						}

						return default_setting;
					};

				// ---------------------------------------

			#define ASSIGN(name) \
				if (config.contains((#name))) { \
					switch (vars.##name.get_type()) { \
						case (var_type_integer): \
							vars.##name.set_var(to_int(config.at(#name), vars.##name.get_as<int>()), true); break; \
						case (var_type_value): \
							vars.##name.set_var(to_float(config.at(#name), vars.##name.get_as<float>()), true); break; \
					} \
				} 

				ASSIGN(portal_visibility_culling);
				ASSIGN(check_nodes_for_potential_lights);

			#undef ASSIGN
			}

			catch (const toml::syntax_error& err)
			{
				game::console();
				printf("%s\n", err.what());
				printf("[GameSettings] Not writing defaults! Please check 'game_settings.toml' or remove the file to re-generate it on next startup!\n");
				return false;
			}
		}

		// always re-write file
		write_toml();

		return true;
	}

	ConCommand xo_gamesettings_update {};
	void xo_gamesettings_update_fn()
	{
		game_settings::parse_toml();
	}

	game_settings::game_settings()
	{
		parse_toml();

		// ----
		game::con_add_command(&xo_gamesettings_update, "xo_gamesettings_update", xo_gamesettings_update_fn, "Reloads the game_settings.toml file");
	}
}