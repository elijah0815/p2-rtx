#include "std_include.hpp"

namespace components
{
	// > scene_ent_on_start_event_hk && scene_ent_on_finish_event_hk can be used to detect when choreography's (vcd's) start & end
	// > use cvar 'scene_print' to get event names or look into NUT files

	// called from main_module::on_renderview()
	void choreo_events::on_client_frame()
	{
		const auto& mapname = map_settings::get_map_name();
		if (!mapname.empty())
		{
			if (mapname.starts_with("sp_a4"))
			{
				if (mapname.ends_with("le2")) // sp_a4_finale2
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
			} // end 'sp_a4'
		}
	}

	// #
	// #

	// 'CSceneEntity::StartEvent' :: triggered on event start
	void scene_ent_on_start_event_hk([[maybe_unused]] CChoreoEvent* ev)
	{
		if (ev && ev->m_Name.string && ev->m_pScene)
		{
			const auto sname = std::string_view(ev->m_pScene->m_szFileName);
			if (std::string_view(ev->m_Name.string) != "NULL")
			{
				const auto& mapname = map_settings::get_map_name();
				if (!mapname.empty())
				{
					if (mapname.starts_with("sp_a4"))
					{
						// fix invisible world model (arms/pgun) after expl.
						if (mapname.ends_with("le4")) // sp_a4_finale4
						{
							// bw_finale04_button_press01.vcd
							if (sname.ends_with("button_press01.vcd")) {
								api::remix_vars::set_option(api::remix_vars::get_option("rtx.playerModel.enableInPrimarySpace"), { true });
							}

							// bw_finale04_button_press04.vcd
							else if (sname.ends_with("button_press04.vcd")) {
								api::remix_vars::set_option(api::remix_vars::get_option("rtx.playerModel.enableInPrimarySpace"), { false });
							}
						}
					} // end 'sp_a4'
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
			const auto& mapname = map_settings::get_map_name();

			if (!mapname.empty())
			{
				if (mapname.starts_with("sp_a4"))
				{
					if (mapname.ends_with("le2")) // sp_a4_finale2
					{
						// scenes/npc/sphere03/bw_a4_finale02_trapintro02.vcd
						if (sname.ends_with("trapintro02.vcd")) {
							choreo_events::ev_a4_f2_api_portal_spawn.trigger();
						}
					}
				} // end 'sp_a4'
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
