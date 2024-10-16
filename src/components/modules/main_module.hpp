#pragma once

namespace components
{
	namespace api
	{
		extern bool m_initialized;
		extern remixapi_Interface bridge;

		extern remixapi_Transform portal0 = {};
		extern remixapi_Transform portal1 = {};

		extern void init();
	}

	class main_module : public component
	{
	public:
		main_module();
		~main_module();
		const char* get_name() override { return "main_module"; }

		static inline std::uint64_t framecount = 0u;

	private:
	};
}
