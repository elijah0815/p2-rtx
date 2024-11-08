#pragma once

namespace components
{
	extern int g_player_current_node;
	extern int g_player_current_leaf;
	extern int g_player_current_area;

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

		extern remixapi_LightHandle bts3_flashlight_handle;
		extern Vector bts3_flashlight_pos;
		extern Vector bts3_wheatly_pos;
	}

	namespace events
	{
		struct scene_entity_event_s
		{
			class event_single
			{
			public:
				void trigger()
				{
					m_triggered = true;
					m_time_point = std::chrono::steady_clock::now();
				}

				/**
				 * Can be used to check if a specified time frame has passed since the event was triggered
				 * @param seconds	Amount of seconds - Use 0 to only check if the event was triggered
				 * @return			False if event was not triggered or elapsed time smaller then specified amount of seconds
				 */
				bool has_elapsed(const float seconds = 0.0f) const
				{
					if (!m_triggered) {
						return false;
					}

					if (utils::float_equal(seconds, 0.0f)) {
						return m_triggered;
					}

					const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_time_point).count();
					return static_cast<float>(elapsed) > seconds;
				}

				void reset() {
					m_triggered = false;
				}

			private:
				bool m_triggered = false;
				std::chrono::steady_clock::time_point m_time_point;
			};

			event_single a4_f2_api_portal_spawn = {};

			void reset()
			{
				a4_f2_api_portal_spawn.reset();
			}
		};

		extern scene_entity_event_s s_ent;
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
