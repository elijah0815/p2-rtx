#pragma once

namespace components
{
	class model_render_hlslpp : public component
	{
	public:
		model_render_hlslpp();

		static Vector4D fix_sprite_trail_particles(prim_fvf_context& ctx, CPrimList* primlist);
	};
}
