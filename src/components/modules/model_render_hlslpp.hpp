#pragma once

namespace components
{
	class model_render_hlslpp : public component
	{
	public:
		model_render_hlslpp();
		const char* get_name() override { return "model_render_hlslpp"; }

		static void fix_sprite_trail_particles(prim_fvf_context& ctx, CPrimList* primlist);
	};
}
