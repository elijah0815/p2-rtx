#pragma once

namespace components
{
	class main_module : public component
	{
	public:
		main_module();
		~main_module() = default;
		const char* get_name() override { return "main_module"; }

		static inline std::uint64_t framecount = 0u;
		static inline remixapi_Interface bridge = {};
		static inline bool m_initialized = false;

		static inline remixapi_Transform portal0 = {};
		static inline remixapi_Transform portal1 = {};

	private:
	};
}
