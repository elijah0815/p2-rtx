#pragma once

namespace components
{
	class game_settings : public component
	{
	public:
		game_settings();
		~game_settings() = default;

		static inline game_settings* p_this = nullptr;
		static auto get() { return &vars; }

		static void write_toml();
		static bool parse_toml();

	private:
		union var_value
		{
			int integer;
			float value;
		};

		enum var_type : std::uint8_t
		{
			var_type_integer = 0,
			var_type_value = 1,
		};

		class variable
		{
		public:
			variable(const char* name, const char* desc, const int integer) :
				m_name(name), m_desc(desc), m_type(var_type_integer)
			{
				m_var.integer = integer;
				m_var_default.integer = integer;
			}

			variable(const char* name, const char* desc, const float value) :
				m_name(name), m_desc(desc), m_type(var_type_value)
			{
				m_var.value = value;
				m_var_default.value = value;
			}

			const char* get_str_value(bool get_default = false) const
			{
				switch (m_type)
				{
				case var_type_integer:
					return utils::va("%d", !get_default ? m_var.integer : m_var_default.integer);

				case var_type_value:
					return utils::va("%.2f", !get_default ? m_var.value : m_var_default.value);
				}

				return nullptr;
			}

			const char* get_str_type() const
			{
				switch (m_type)
				{
				case var_type_integer:
					return "INT";

				case var_type_value:
					return "FLOAT";
				}

				return nullptr;
			}

			template <typename T>
			T get_as(bool default_val = false)
			{
				if (m_type == var_type_integer) {
					return static_cast<T>(!default_val ? m_var.integer : m_var_default.integer);
				}

				if (m_type == var_type_value) {
					return static_cast<T>(!default_val ? m_var.value : m_var_default.value);
				}

				throw std::runtime_error("Unknown var_type");
			}

			var_type get_type() const {
				return m_type;
			}

			// sets var and writes toml
			void set_var(const int integer, bool no_toml_update = false)
			{
				m_var.integer = integer;
				if (!no_toml_update) {
					write_toml();
				}
			}

			// sets var and writes toml
			void set_var(const float value, bool no_toml_update = false)
			{
				m_var.value = value;
				if (!no_toml_update) {
					write_toml();
				}
			}

			const char* m_name;
			const char* m_desc;

		private:
			var_value m_var;
			var_value m_var_default;
			var_type m_type;
		};


		struct var_definitions
		{
			variable portal_visibility_culling =
			{
				"portal_visibility_culling",
				"Enable culling of exit portal areas when the entry portal is not visible from the players POV (++ performance)",
				1
			};

			variable check_nodes_for_potential_lights =
			{
				"check_nodes_for_potential_lights",
				"Check each node for potential emissive light strips (rectangular with limited depth) & force-draw node on match.",
				1
			};
		}; //STATIC_ASSERT_SIZE(var_definitions, 2 * sizeof(variable));

		static inline var_definitions vars = {};
	};
}
