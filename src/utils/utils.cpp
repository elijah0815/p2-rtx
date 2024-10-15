#include "std_include.hpp"

#define VA_BUFFER_COUNT		64
#define VA_BUFFER_SIZE		65536

namespace utils
{
	int try_stoi(const std::string& str, const int& default_return_val)
	{
		int ret = default_return_val;

		try
		{
			ret = std::stoi(str);
		}
		catch (const std::invalid_argument)
		{ }

		return ret;
	}

	float try_stof(const std::string& str, const float& default_return_val)
	{
		float ret = default_return_val;

		try
		{
			ret = std::stof(str);
		}
		catch (const std::invalid_argument)
		{ }

		return ret;
	}

	

	std::string split_string_between_delims(const std::string& str, const char delim_start, const char delim_end)
	{
		const auto first = str.find_last_of(delim_start);
		if (first == std::string::npos) return "";

		const auto last = str.find_first_of(delim_end, first);
		if (last == std::string::npos) return "";

		return str.substr(first + 1, last - first - 1);
	}

	bool starts_with(std::string_view haystack, std::string_view needle)
	{
		return (haystack.size() >= needle.size() && !strncmp(needle.data(), haystack.data(), needle.size()));
	}

	bool string_contains(const std::string_view& s1, const std::string_view s2)
	{
		const auto it = s1.find(s2);
		if (it != std::string::npos)
		{
			return true;
		}

		return false;
	}

	void replace_all(std::string& source, const std::string_view& from, const std::string_view& to)
	{
		std::string new_string;
		new_string.reserve(source.length());  // avoids a few memory allocations

		std::string::size_type last_pos = 0;
		std::string::size_type findPos;

		while (std::string::npos != (findPos = source.find(from, last_pos)))
		{
			new_string.append(source, last_pos, findPos - last_pos);
			new_string += to;
			last_pos = findPos + from.length();
		}

		// Care for the rest after last occurrence
		new_string += source.substr(last_pos);

		source.swap(new_string);
	}

	bool erase_substring(std::string& base, const std::string& replace)
	{
		if (const auto it = base.find(replace);
			it != std::string::npos)
		{
			base.erase(it, replace.size());
			return true;
		}

		return false;
	}

	std::string str_to_lower(std::string input)
	{
		std::ranges::transform(input.begin(), input.end(), input.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return input;
	}

	std::string convert_wstring(const std::wstring& wstr)
	{
		std::string result;
		result.reserve(wstr.size());

		for (const auto& chr : wstr)
		{
			result.push_back(static_cast<char>(chr));
		}

		return result;
	}

	int is_space(int c)
	{
		if (c < -1)
		{
			return 0;
		}

		return _isspace_l(c, nullptr);
	}

	// trim from start
	std::string& ltrim(std::string& s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int val)
			{
				return !is_space(val);
			}));

		return s;
	}

	// trim from end
	std::string& rtrim(std::string& s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int val)
			{
				return !is_space(val);

			}).base(), s.end());

		return s;
	}

	// trim from both ends
	std::string& trim(std::string& s)
	{
		return ltrim(rtrim(s));
	}

	bool has_matching_symbols(const std::string& str, char opening_symbol, char closing_symbol, bool single_only)
	{
		int count = 0;

		for (char c : str)
		{
			if (c == opening_symbol)
			{
				count++;
			}
			else if (c == closing_symbol)
			{
				count--;

				if (count < 0)
				{
					return false;  // malformed
				}
			}

			if (single_only && count > 1)
			{
				return false;
			}
		}

		return count == 0;
	}

	const char* va(const char* fmt, ...)
	{
		static char g_vaBuffer[VA_BUFFER_COUNT][VA_BUFFER_SIZE];
		static int g_vaNextBufferIndex = 0;

		va_list ap;
		va_start(ap, fmt);
		char* dest = g_vaBuffer[g_vaNextBufferIndex];
		vsnprintf(g_vaBuffer[g_vaNextBufferIndex], VA_BUFFER_SIZE, fmt, ap);
		g_vaNextBufferIndex = (g_vaNextBufferIndex + 1) % VA_BUFFER_COUNT;
		va_end(ap);
		return dest;
	}

	void extract_integer_words(const std::string_view& str, std::vector<int>& integers, bool check_for_duplicates)
	{
		std::stringstream ss;

		//Storing the whole string into string stream
		ss << str;

		// Running loop till the end of the stream
		std::string temp;
		int found;

		while (!ss.eof())
		{
			// extracting word by word from stream 
			ss >> temp;

			// Checking the given word is integer or not
			if (std::stringstream(temp) >> found)
			{
				if (check_for_duplicates)
				{
					// check if we added the integer already
					if (std::find(integers.begin(), integers.end(), found) == integers.end())
					{
						// new integer
						integers.push_back(found);
					}
				}

				else
				{
					//cout << found << " ";
					integers.push_back(found);
				}
			}

			// To save from space at the end of string
			temp = "";
		}
	}

	void row_major_to_column_major(const float* row_major, float* column_major)
	{
		// transpose the matrix by swapping the rows and columns
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				column_major[j * 4 + i] = row_major[i * 4 + j];
			}
		}
	}

	/**
	* @brief			open handle to a file within the home-path (root)
	* @param sub_dir	sub directory within home-path (root)
	* @param file_name	the file name
	* @param file		in-out file handle
	* @return			file handle state (valid or not)
	*/
	bool open_file_homepath(const std::string& sub_dir, const std::string& file_name, std::ifstream& file)
	{
		char path[MAX_PATH];
		GetModuleFileNameA(nullptr, path, MAX_PATH); // path of the exe
		
		std::string	file_path = path;
		erase_substring(file_path, "portal2.exe");
		file_path += sub_dir + "\\" + file_name;

		file.open(file_path);
		if (!file.is_open())
		{
			return false;
		}

		return true;
	}
}
