#include "std_include.hpp"

namespace components::api
{
	void remix_lights::a2_bts3_flashlight()
	{
		if (map_settings::is_level.sp_a2_bts3)
		{
			if (!m_bts3_flashlight_pos.IsZero(0.0001f) && !m_bts3_wheatly_pos.IsZero(0.0001f))
			{
				auto dir = m_bts3_flashlight_pos - m_bts3_wheatly_pos;
				dir.Normalize();

				// offset pos along along direction so it does not start inside wheatly
				auto pos = m_bts3_wheatly_pos + dir * 20.0f;

				auto ext = remixapi_LightInfoSphereEXT
				{
					.sType = REMIXAPI_STRUCT_TYPE_LIGHT_INFO_SPHERE_EXT,
					.pNext = nullptr,
					.position = remixapi_Float3D { pos.x, pos.y, pos.z },
					.radius = 1.2f,
					.shaping_hasvalue = TRUE,
					.shaping_value = {},
				};

				if (ext.shaping_hasvalue)
				{
					// ensure the direction is normalized
					ext.shaping_value.direction = { dir.x, dir.y, dir.z };
					ext.shaping_value.coneAngleDegrees = 45.0f;
					ext.shaping_value.coneSoftness = 0.2f;
					ext.shaping_value.focusExponent = 0;
				}

				const float lscale = 15.0f;

				auto info = remixapi_LightInfo
				{
					.sType = REMIXAPI_STRUCT_TYPE_LIGHT_INFO,
					.pNext = &ext,
					.hash = utils::string_hash64("bts3fl"),
					.radiance = remixapi_Float3D { 150 * lscale, 140 * lscale, 130 * lscale },
				};

				if (m_bts3_flashlight_handle)
				{
					bridge.DestroyLight(m_bts3_flashlight_handle);
					m_bts3_flashlight_handle = nullptr;
				}

				bridge.CreateLight(&info, &m_bts3_flashlight_handle);

				if (m_bts3_flashlight_handle) {
					bridge.DrawLightInstance(m_bts3_flashlight_handle);
				}

				// ---
				// sphere light at start of spotlight

				// bts3_flashlight_sphere_handle
				pos = m_bts3_wheatly_pos + dir * 9.0f;
				ext.position = { pos.x, pos.y, pos.z };
				ext.radius = 1.5f;
				ext.shaping_hasvalue = FALSE;
				info.hash = utils::string_hash64("bts3flsp");
				info.radiance = { info.radiance.x * 0.2f, info.radiance.y * 0.25f, info.radiance.z * 0.35f };

				if (m_bts3_flashlight_sphere_handle)
				{
					bridge.DestroyLight(m_bts3_flashlight_sphere_handle);
					m_bts3_flashlight_sphere_handle = nullptr;
				}

				bridge.CreateLight(&info, &m_bts3_flashlight_sphere_handle);

				if (m_bts3_flashlight_sphere_handle) {
					bridge.DrawLightInstance(m_bts3_flashlight_sphere_handle);
				}

				// ---

				m_bts3_flashlight_pos = { 0, 0, 0 };
				m_bts3_wheatly_pos = { 0, 0, 0 };
			}
		}
	}

	void remix_lights::a2_bts3_flashlight_destroy()
	{
		if (m_bts3_flashlight_handle)
		{
			bridge.DestroyLight(m_bts3_flashlight_handle);
			m_bts3_flashlight_handle = nullptr;
		}

		if (m_bts3_flashlight_sphere_handle)
		{
			bridge.DestroyLight(m_bts3_flashlight_sphere_handle);
			m_bts3_flashlight_sphere_handle = nullptr;
		}

		m_bts3_flashlight_pos = { 0, 0, 0 };
		m_bts3_wheatly_pos = { 0, 0, 0 };
	}


	// ##
	// ##

	void remix_lights::on_client_frame()
	{
		remix_lights::get()->a2_bts3_flashlight();
	}

	void remix_lights::on_map_load()
	{
		remix_lights::get()->a2_bts3_flashlight_destroy();
	}

	remix_lights::remix_lights()
	{
		p_this = this;
	}
}
