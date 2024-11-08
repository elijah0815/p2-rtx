#pragma once

namespace components::api
{
	class remix_lights : public component
	{
	public:
		remix_lights();

		static inline remix_lights* p_this = nullptr;
		static remix_lights* get() { return p_this; }

		static void on_client_frame();
		static void on_map_load();

		static void bts3_set_flashlight_end_pos(const Vector& v) { m_bts3_flashlight_pos = v; }
		static void bts3_set_flashlight_start_pos(const Vector& v) { m_bts3_wheatly_pos = v; }

	private:
		void a2_bts3_flashlight();
		void a2_bts3_flashlight_destroy();
		static inline remixapi_LightHandle m_bts3_flashlight_handle = nullptr;
		static inline remixapi_LightHandle m_bts3_flashlight_sphere_handle = nullptr;
		static inline Vector m_bts3_flashlight_pos = {};
		static inline Vector m_bts3_wheatly_pos = {};
	};
}
