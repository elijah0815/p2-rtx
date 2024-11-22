#pragma once

namespace components
{
	class map_settings : public component
	{
	public:
		map_settings();
		~map_settings() = default;

		static inline map_settings* p_this = nullptr;
		static map_settings* get() { return p_this; }

		enum LEAF_TRANS_MODE : uint8_t
		{
			ONCE_ON_ENTER = 0,
			ONCE_ON_LEAVE = 1,
			ALWAYS_ON_ENTER = 2,
			ALWAYS_ON_LEAVE = 3,
		};


		struct leaf_transition_s
		{
			std::unordered_set<std::uint32_t> leafs;
			std::string config_name;
			LEAF_TRANS_MODE mode;
			api::remix_vars::EASE_TYPE interpolate_type;
			float delay_in = 0.0f;
			float delay_out = 0.0f;
			float duration = 0.0f;
			std::uint64_t hash;
			bool _state_enter = false;
		};

		struct marker_settings_s
		{
			std::uint32_t index = 0;
			float origin[3] = {};
			void* handle = nullptr;
		};

		struct api_config_var
		{
			std::string variable;
			std::string value;
		};

		struct map_settings_s
		{
			std::string	mapname;
			float fog_dist = 0.0f;
			DWORD fog_color = 0xFFFFFFFF;
			std::unordered_map<std::uint32_t, std::unordered_set<std::uint32_t>> area_settings;
			std::vector<leaf_transition_s> leaf_transitions;
			std::vector<marker_settings_s> map_markers;
			std::vector<std::string> api_var_configs;
		};

		static map_settings_s& get_map_settings() { return m_map_settings; }
		static const std::string& get_map_name() { return m_map_settings.mapname; }

		void set_settings_for_map(const std::string& map_name);
		static void spawn_markers_once();
		static void destroy_markers();
		static void on_map_load(const std::string& map_name);
		static void clear_map_settings();

		struct level_bool_s
		{
			bool sp_a1_intro1 = false,			sp_a1_intro2 = false,			sp_a1_intro3 = false,			sp_a1_intro4 = false,
				 sp_a1_intro5 = false,			sp_a1_intro6 = false,			sp_a1_intro7 = false,			sp_a1_wakeup = false,
				 sp_a2_intro = false,			sp_a2_laser_intro = false,		sp_a2_laser_stairs = false,		sp_a2_dual_lasers = false,
				 sp_a2_laser_over_goo = false,	sp_a2_catapult_intro = false,	sp_a2_trust_fling = false,		sp_a2_pit_flings = false,
				 sp_a2_fizzler_intro = false,	sp_a2_sphere_peek = false,		sp_a2_ricochet = false,			sp_a2_bridge_intro = false,
				 sp_a2_bridge_the_gap = false,	sp_a2_turret_intro = false,		sp_a2_laser_relays = false,		sp_a2_turret_blocker = false,
				 sp_a2_laser_vs_turret = false, sp_a2_pull_the_rug = false,		sp_a2_column_blocker = false,	sp_a2_laser_chaining = false,
				 sp_a2_triple_laser = false,	sp_a2_bts1 = false,				sp_a2_bts2 = false,				sp_a2_bts3 = false,
				 sp_a2_bts4 = false,			sp_a2_bts5 = false,				sp_a2_bts6 = false,				sp_a2_core = false,
				 sp_a3_00 = false,				sp_a3_01 = false,				sp_a3_03 = false,				sp_a3_jump_intro = false,
				 sp_a3_bomb_flings = false,		sp_a3_crazy_box = false,		sp_a3_transition01 = false,		sp_a3_speed_ramp = false,
				 sp_a3_speed_flings = false,	sp_a3_portal_intro = false,		sp_a3_end = false,				sp_a4_intro = false,
				 sp_a4_tb_intro = false,		sp_a4_tb_trust_drop = false,	sp_a4_tb_wall_button = false,	sp_a4_tb_polarity = false,
				 sp_a4_tb_catch = false,		sp_a4_stop_the_box = false,		sp_a4_laser_catapult = false,	sp_a4_laser_platform = false,
				 sp_a4_speed_tb_catch = false,	sp_a4_jump_polarity = false,	sp_a4_finale1 = false,			sp_a4_finale2 = false,
				 sp_a4_finale3 = false,			sp_a4_finale4 = false;

			void update(const std::string& n)
			{
					 if (n == "sp_a1_intro1") sp_a1_intro1 = true;
				else if (n == "sp_a1_intro2") sp_a1_intro2 = true;
				else if (n == "sp_a1_intro3") sp_a1_intro3 = true;
				else if (n == "sp_a1_intro4") sp_a1_intro4 = true;
				else if (n == "sp_a1_intro5") sp_a1_intro5 = true;
				else if (n == "sp_a1_intro6") sp_a1_intro6 = true;
				else if (n == "sp_a1_intro7") sp_a1_intro7 = true;
				else if (n == "sp_a1_wakeup") sp_a1_wakeup = true;
				else if (n == "sp_a2_intro") sp_a2_intro = true;
				else if (n == "sp_a2_laser_intro") sp_a2_laser_intro = true;
				else if (n == "sp_a2_laser_stairs") sp_a2_laser_stairs = true;
				else if (n == "sp_a2_dual_lasers") sp_a2_dual_lasers = true;
				else if (n == "sp_a2_laser_over_goo") sp_a2_laser_over_goo = true;
				else if (n == "sp_a2_catapult_intro") sp_a2_catapult_intro = true;
				else if (n == "sp_a2_trust_fling") sp_a2_trust_fling = true;
				else if (n == "sp_a2_pit_flings") sp_a2_pit_flings = true;
				else if (n == "sp_a2_fizzler_intro") sp_a2_fizzler_intro = true;
				else if (n == "sp_a2_sphere_peek") sp_a2_sphere_peek = true;
				else if (n == "sp_a2_ricochet") sp_a2_ricochet = true;
				else if (n == "sp_a2_bridge_intro") sp_a2_bridge_intro = true;
				else if (n == "sp_a2_bridge_the_gap") sp_a2_bridge_the_gap = true;
				else if (n == "sp_a2_turret_intro") sp_a2_turret_intro = true;
				else if (n == "sp_a2_laser_relays") sp_a2_laser_relays = true;
				else if (n == "sp_a2_turret_blocker") sp_a2_turret_blocker = true;
				else if (n == "sp_a2_laser_vs_turret") sp_a2_laser_vs_turret = true;
				else if (n == "sp_a2_pull_the_rug") sp_a2_pull_the_rug = true;
				else if (n == "sp_a2_column_blocker") sp_a2_column_blocker = true;
				else if (n == "sp_a2_laser_chaining") sp_a2_laser_chaining = true;
				else if (n == "sp_a2_triple_laser") sp_a2_triple_laser = true;
				else if (n == "sp_a2_bts1") sp_a2_bts1 = true;
				else if (n == "sp_a2_bts2") sp_a2_bts2 = true;
				else if (n == "sp_a2_bts3") sp_a2_bts3 = true;
				else if (n == "sp_a2_bts4") sp_a2_bts4 = true;
				else if (n == "sp_a2_bts5") sp_a2_bts5 = true;
				else if (n == "sp_a2_bts6") sp_a2_bts6 = true;
				else if (n == "sp_a2_core") sp_a2_core = true;
				else if (n == "sp_a3_00") sp_a3_00 = true;
				else if (n == "sp_a3_01") sp_a3_01 = true;
				else if (n == "sp_a3_03") sp_a3_03 = true;
				else if (n == "sp_a3_jump_intro") sp_a3_jump_intro = true;
				else if (n == "sp_a3_bomb_flings") sp_a3_bomb_flings = true;
				else if (n == "sp_a3_crazy_box") sp_a3_crazy_box = true;
				else if (n == "sp_a3_transition01") sp_a3_transition01 = true;
				else if (n == "sp_a3_speed_ramp") sp_a3_speed_ramp = true;
				else if (n == "sp_a3_speed_flings") sp_a3_speed_flings = true;
				else if (n == "sp_a3_portal_intro") sp_a3_portal_intro = true;
				else if (n == "sp_a3_end") sp_a3_end = true;
				else if (n == "sp_a4_intro") sp_a4_intro = true;
				else if (n == "sp_a4_tb_intro") sp_a4_tb_intro = true;
				else if (n == "sp_a4_tb_trust_drop") sp_a4_tb_trust_drop = true;
				else if (n == "sp_a4_tb_wall_button") sp_a4_tb_wall_button = true;
				else if (n == "sp_a4_tb_polarity") sp_a4_tb_polarity = true;
				else if (n == "sp_a4_tb_catch") sp_a4_tb_catch = true;
				else if (n == "sp_a4_stop_the_box") sp_a4_stop_the_box = true;
				else if (n == "sp_a4_laser_catapult") sp_a4_laser_catapult = true;
				else if (n == "sp_a4_laser_platform") sp_a4_laser_platform = true;
				else if (n == "sp_a4_speed_tb_catch") sp_a4_speed_tb_catch = true;
				else if (n == "sp_a4_jump_polarity") sp_a4_jump_polarity = true;
				else if (n == "sp_a4_finale1") sp_a4_finale1 = true;
				else if (n == "sp_a4_finale2") sp_a4_finale2 = true;
				else if (n == "sp_a4_finale3") sp_a4_finale3 = true;
				else if (n == "sp_a4_finale4") sp_a4_finale4 = true;
			}

			void reset()
			{
				memset(this, 0, sizeof(level_bool_s));
			}
		};

		static inline level_bool_s is_level = {};

	private:
		static inline map_settings_s m_map_settings = {};
		static inline std::vector<std::string> m_args;
		static inline bool m_spawned_markers = false;

		bool parse_toml();
		bool matches_map_name();
		void open_and_set_var_config(const std::string& config, bool ignore_hashes = false, const char* custom_path = nullptr);
	};
}
