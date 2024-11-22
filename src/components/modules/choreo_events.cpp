#include "std_include.hpp"

namespace components
{
	// > scene_ent_on_start_event_hk && scene_ent_on_finish_event_hk can be used to detect when choreography's (vcd's) start & end
	// > use cvar 'scene_print' to get event names or look into NUT files

	// called from main_module::on_renderview()
	void choreo_events::on_client_frame()
	{
		//const auto& mapname = map_settings::get_map_name();
		//if (!mapname.empty())
		{
			if (map_settings::is_level.sp_a4_finale2) // sp_a4_finale2
			{
				if (ev_a4_f2_api_portal_spawn.has_elapsed(5.0f))
				{
					if (!api::remix_rayportal::get()->empty())
					{
						auto& p = api::remix_rayportal::get()->get_portal_pair(api::remix_rayportal::PORTAL_PAIR_1)->get_portal0();
						p.m_pos = { 2108.0f, 774.0f, -18.0f };
						p.uncache();
						ev_a4_f2_api_portal_spawn.reset();
					}
				}
			}
		}
	}

	// #
	// #

	void handle_confvar_transition(const std::string_view& sname, const bool is_start)
	{
		auto& map_settings = map_settings::get_map_settings();
		for (auto t = map_settings.choreo_transitions.begin(); t != map_settings.choreo_transitions.end();)
		{
			bool iterpp = false;

			const bool mode = is_start
				? (t->mode == map_settings::CHOREO_TRANS_MODE::ONCE_ON_START || t->mode == map_settings::CHOREO_TRANS_MODE::ALWAYS_ON_START)
				: (t->mode == map_settings::CHOREO_TRANS_MODE::ONCE_ON_END || t->mode == map_settings::CHOREO_TRANS_MODE::ALWAYS_ON_END);

			if (mode)
			{
				if (sname.contains(t->choreo_name))
				{
					bool can_add_transition = true;

					// do not allow the same transition twice
					for (const auto& ip : api::remix_vars::interpolate_stack)
					{
						if (ip.identifier == t->hash)
						{
							can_add_transition = false;
							break;
						}
					}

					if (can_add_transition)
					{
						api::remix_vars::parse_and_apply_conf_with_lerp(
							t->config_name,
							t->hash,
							t->interpolate_type,
							t->duration,
							t->delay_in,
							t->delay_out);

						if (t->mode <= map_settings::CHOREO_TRANS_MODE::ONCE_ON_END)
						{
							t = map_settings.choreo_transitions.erase(t);
							iterpp = true; // erase returns the next iterator
						}
					}
				}
			}

			if (!iterpp) {
				++t;
			}
		}
	}

	// 'CSceneEntity::StartEvent' :: triggered on event start
	void scene_ent_on_start_event_hk([[maybe_unused]] CChoreoEvent* ev)
	{
		if (ev && ev->m_Name.string && ev->m_pScene)
		{
			const auto sname = std::string_view(ev->m_pScene->m_szFileName);
			if (std::string_view(ev->m_Name.string) != "NULL")
			{
				//const auto& mapname = map_settings::get_map_name();
				//if (!mapname.empty())
				{
					// fix invisible world model (arms/pgun) after expl.
					if (map_settings::is_level.sp_a4_finale4)
					{
						// bw_finale04_button_press01.vcd
						if (sname.ends_with("button_press01.vcd")) {
							api::remix_vars::set_option(api::remix_vars::get_option("rtx.playerModel.enableInPrimarySpace"), { true });
						}

						// bw_finale04_button_press04.vcd
						else if (sname.ends_with("epilogue10.vcd")) {
							api::remix_vars::set_option(api::remix_vars::get_option("rtx.playerModel.enableInPrimarySpace"), { false });
						}
					}

					// ------

					// handle remix config transitions added via mapsettings
					handle_confvar_transition(sname, true);
				}
			}
		}
	}

	HOOK_RETN_PLACE_DEF(scene_ent_on_start_event_retn);
	__declspec(naked) void scene_ent_on_start_event_stub()
	{
		__asm
		{
			// og
			mov     edi, [ebp + 0x10];
			mov     esi, ecx;

			pushad;
			push    edi;
			call	scene_ent_on_start_event_hk;
			add		esp, 4;
			popad;

			// og
			jmp		scene_ent_on_start_event_retn;
		}
	}

	// #
	// #

	// 'CSceneEntity::OnSceneFinished' :: triggered right after the audio stops - ignores postdelay
	void scene_ent_on_finish_event_hk(const char* scene_name)
	{
		if (scene_name)
		{
			const auto sname = std::string_view(scene_name);
			//const auto& mapname = map_settings::get_map_name();
			//if (!mapname.empty())
			{
				if (map_settings::is_level.sp_a4_finale2)
				{
					// scenes/npc/sphere03/bw_a4_finale02_trapintro02.vcd
					if (sname.ends_with("trapintro02.vcd")) {
						choreo_events::ev_a4_f2_api_portal_spawn.trigger();
					}
				}

				// ------

				// handle remix config transitions added via mapsettings
				handle_confvar_transition(sname, false);
			}
		}
	}

	HOOK_RETN_PLACE_DEF(scene_ent_on_finish_event_retn);
	__declspec(naked) void scene_ent_on_finish_event_stub()
	{
		__asm
		{
			// og
			mov     eax, [esi + 0x360]; // CSceneEntity->m_iszSceneFile

			pushad;
			push    eax;
			call	scene_ent_on_finish_event_hk;
			add		esp, 4;
			popad;

			// og
			jmp		scene_ent_on_finish_event_retn;
		}
	}

	// #
	// #

	choreo_events::choreo_events()
	{
		p_this = this;

		// CSceneEntity::StartEvent :: : can be used to detect the start of scene (vcd) entities
		utils::hook(SERVER_BASE + USE_OFFSET(0x233618, 0x22D428), scene_ent_on_start_event_stub).install()->quick();
		HOOK_RETN_PLACE(scene_ent_on_start_event_retn, SERVER_BASE + USE_OFFSET(0x23361D, 0x22D42D));

		// CSceneEntity::OnSceneFinished
		utils::hook::nop(SERVER_BASE + USE_OFFSET(0x238483, 0x232273), 6);
		utils::hook(SERVER_BASE + USE_OFFSET(0x238483, 0x232273), scene_ent_on_finish_event_stub).install()->quick();
		HOOK_RETN_PLACE(scene_ent_on_finish_event_retn, SERVER_BASE + USE_OFFSET(0x238489, 0x232279));
	}
}
