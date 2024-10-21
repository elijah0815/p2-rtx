#pragma once

namespace components
{
	class map_settings : public component
	{
	public:
		map_settings();
		~map_settings() = default;
		const char* get_name() override { return "map_settings"; }

		static inline map_settings* p_this = nullptr;
		static map_settings* get() { return p_this; }

		static void on_map_load(const std::string& map_name);

		enum PARSE_MODE : std::uint32_t
		{
			CULL,
			MARKER,
			API_VARS
		};

		struct marker_settings_s
		{
			float origin[3] = {};
			void* handle = nullptr;
			bool active = false;
		};

		struct api_config_var
		{
			std::string variable;
			std::string value;
		};

		struct map_settings_s
		{
			std::string	mapname;
			std::unordered_map<std::uint32_t, std::unordered_set<std::uint32_t>> area_settings;
			std::vector<marker_settings_s> map_markers;
			std::vector<std::string> api_var_configs;
		};

		static map_settings_s* get_loaded_map_settings() { return m_loaded_map_settings; }
		static const std::string& get_loaded_map_name() { return m_loaded_map_name; }
		static void clear_loaded_map_settings() { m_loaded_map_settings = nullptr; }
		void set_settings_for_map(const std::string& map_name, bool reload_settings = false);
		static void spawn_markers_once();
		static void on_map_exit();


	private:
		static inline std::string m_loaded_map_name;
		static inline map_settings_s* m_loaded_map_settings = nullptr;
		static inline std::vector<map_settings_s> m_settings;
		static inline std::vector<std::string> m_args;
		static inline bool m_spawned_markers = false;

		bool load_settings();
		map_settings_s* get_or_create_settings(bool parse_mode = true, const char* map_name = nullptr);
		void parse_culling();
		void parse_markers();
		void open_and_set_var_config(const std::string& config, bool ignore_hashes = false, const char* custom_path = nullptr);
		void parse_api_var_configs();
	};
}
