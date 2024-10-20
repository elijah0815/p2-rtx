#include "std_include.hpp"

namespace components
{
	std::vector<component*> loader::components_;
	utils::memory::allocator loader::mem_allocator_;

	void loader::initialize()
	{
		mem_allocator_.clear();
		_register(new flags());
		_register(new main_module());
		_register(new model_render());
		_register(new map_settings());
		_register(new remix_vars());

		XASSERT(MH_EnableHook(MH_ALL_HOOKS) != MH_STATUS::MH_OK);
	}

	void loader::uninitialize()
	{
		std::ranges::reverse(components_.begin(), components_.end());
		for (const auto component : components_) {
			delete component;
		}

		components_.clear();
		mem_allocator_.clear();
		fflush(stdout);
		fflush(stderr);
	}

	void loader::_register(component* component)
	{
		if (component)
		{
			game::loaded_modules.push_back("component registered: "s + component->get_name() + "\n");
			components_.push_back(component);
		}
	}

	utils::memory::allocator* loader::get_alloctor()
	{
		return &loader::mem_allocator_;
	}
}
