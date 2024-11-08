#pragma once

namespace components
{
	class component
	{
	public:
		component() {};
		virtual ~component() {};
	};

	class loader
	{
	public:
		static void initialize();
		static void uninitialize();
		static void _register(component* component);

		static utils::memory::allocator* get_alloctor();

	private:
		static std::vector<component*> components_;
		static utils::memory::allocator mem_allocator_;
	};
}

#include "modules/flags.hpp"
#include "modules/choreo_events.hpp"
#include "modules/main_module.hpp"
#include "modules/model_render.hpp"
#include "modules/model_render_hlslpp.hpp"
#include "modules/map_settings.hpp"
#include "modules/remix_vars.hpp"
#include "modules/remix_rayportal.hpp"
#include "modules/remix_lights.hpp"
