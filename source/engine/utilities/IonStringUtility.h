/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonStringUtility.h
-------------------------------------------
*/

#ifndef ION_STRING_UTILITY_H
#define ION_STRING_UTILITY_H

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "IonConvert.h"
#include "types/IonTypes.h"

///@brief Namespace containing commonly used (optimized) string functions for manipulating and formatting strings
namespace ion::utilities::string
{
	using namespace std::string_view_literals;
	
	enum class StringCase : bool
	{
		Sensitive,
		Insensitive
	};

	enum class StringSplitOptions
	{
		RemoveEmptyEntries,
		PreserveEmptyEntries,
		PreserveDelimiters,
		PreserveEmptyEntriesAndDelimiters
	};


	namespace detail
	{
		//Constants used for pattern matching

		constexpr auto pattern_wildcards = "%*"sv;
		constexpr auto pattern_any_character = '_';

		//Constants used for formatting

		constexpr auto format_zero_placeholder = '0';
		constexpr auto format_digit_placeholder = '#';
		constexpr auto format_placeholders = "0#"sv;
		constexpr auto format_decimal_point = '.';
		constexpr auto format_thousands_separator = ',';
		constexpr auto format_exponent = 'e';


		constexpr auto is_ascii(char c) noexcept
		{
			return c >= '\0' && c <= '\x7F';
		}

		constexpr auto is_extended_ascii(char c) noexcept
		{
			return !is_ascii(c);
		}

		constexpr auto is_non_printable(char c) noexcept
		{
			return (c >= '\0' && c < ' ') || c == '\x7F';
		}

		constexpr auto is_printable(char c) noexcept
		{
			return !is_non_printable(c);
		}

		constexpr auto is_alpha(char c) noexcept
		{
			return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || is_extended_ascii(c);
		}

		constexpr auto is_numeric(char c) noexcept
		{
			return c >= '0' && c <= '9';
		}

		constexpr auto is_alpha_numeric(char c) noexcept
		{
			return is_alpha(c) || is_numeric(c);
		}


		constexpr auto is_lower(char c) noexcept
		{
			return c >= 'a' && c <= 'z';
		}

		constexpr auto is_upper(char c) noexcept
		{
			return c >= 'A' && c <= 'Z';
		}

		constexpr auto to_lower(char c) noexcept
		{
			return is_upper(c) ?
				static_cast<char>(c + 32) :
				c;
		}

		constexpr auto to_upper(char c) noexcept
		{
			return is_lower(c) ?
				static_cast<char>(c - 32) :
				c;
		}

		constexpr auto case_insensitive_equal(char x, char y) noexcept
		{
			return to_lower(x) == to_lower(y);
		}

		constexpr auto case_insensitive_less(char x, char y) noexcept
		{
			return to_lower(x) < to_lower(y);
		}

		constexpr auto  case_sensitive_equal_with_pattern(char x, char y) noexcept
		{
			return x == y || y == pattern_any_character;
		}

		constexpr auto case_insensitive_equal_with_pattern(char x, char y) noexcept
		{
			return case_insensitive_equal(x, y) || y == pattern_any_character;
		}


		template <typename T>
		inline auto realify(T &&value)
		{
			static_assert(types::is_string_v<T> || types::is_number_v<T>);

			if constexpr (types::is_string_v<T>)
				return convert::FirstPartTo<real>(value).value_or(0);
			else
				return static_cast<real>(value);
		}

		template <typename T>
		inline decltype(auto) stringify(T &&value)
		{
			static_assert(types::is_string_v<T> || types::is_number_v<T>);

			if constexpr (types::is_string_v<T> || types::is_char_v<T>)
				return std::forward<T>(value);
			else
				return convert::ToString(value);
		}

		template <typename T, typename... Tn>
		inline void concat_each(std::string &str, T &&first, Tn &&...rest)
		{
			str += stringify(std::forward<T>(first));

			if constexpr (sizeof...(Tn) > 0)
				concat_each(str, std::forward<Tn>(rest)...);
		}

		template <typename T, typename... Tn>
		inline void join_each(std::string &str, [[maybe_unused]] std::string_view delimiter, T &&first, Tn &&...rest)
		{
			str += stringify(std::forward<T>(first));

			if constexpr (sizeof...(Tn) > 0)
			{
				str += delimiter;
				join_each(str, delimiter, std::forward<Tn>(rest)...);
			}
		}


		//Forward declaration
		std::string format(real, std::string_view);

		template <size_t N>
		struct substitute_with_impl
		{
			template <typename T>
			auto operator()(std::string &str, size_t off, size_t length,
				T &&values, size_t index, std::optional<std::string_view> format_str) const noexcept
			{
				//Found correct index
				if (N - 1 == index)
				{
					if (format_str)
					{
						auto result = format(realify(std::get<N - 1>(std::forward<T>(values))), *format_str);
						str.replace(off, length, result);
						return off + std::size(result);
					}

					auto &&result = stringify(std::get<N - 1>(std::forward<T>(values)));

					if constexpr (types::is_char_v<decltype(result)>)
					{
						str.replace(off, length, 1, result);
						return off + 1;
					}
					else
					{
						str.replace(off, length, result);
						return off + std::size(result);
					}
				}
				
				//Check previous index
				if constexpr (N - 1 > 0)
					return substitute_with_impl<N - 1>()(str, off, length,
						std::forward<T>(values), index, format_str);
				else
					return off + length;
			}
		};
		
		template <typename T>
		inline auto substitute_with(std::string &str, size_t off, size_t length,
			T &&values, size_t index, std::optional<std::string_view> format_str)
		{
			return substitute_with_impl<std::tuple_size_v<T>>()(str, off, length,
				std::forward<T>(values), index, format_str);
		}

		template <typename T>
		inline auto substitute_curly_brace(std::string &str, size_t off, size_t length, T &&values)
		{
			if (length > 2)
			{
				auto item = std::string_view{std::data(str) + off + 1, length - 2};
				auto index = convert::FirstPartTo<int>(item);

				if (index && *index < std::tuple_size_v<T>)
				{
					auto format_delimiter = item.find(':');
					return (format_delimiter != std::string::npos) ?
						//Substitute with formatted tuple value
						substitute_with(str, off, length,
							std::forward<T>(values), *index, item.substr(format_delimiter + 1)) :
						//Substitute with unformatted tuple value
						substitute_with(str, off, length,
							std::forward<T>(values), *index, {});
				}
			}

			return off + length;
		}

		template <typename T>
		inline auto &substitute_curly_braces(std::string &str, T &&values)
		{
			auto inside_brace = false;

			for (size_t i = 0, off = 0; i < std::size(str);)
			{
				switch (str[i])
				{
					case '{':
					{
						//Final brace (no consecutive)
						if (!(i + 1 < std::size(str) &&
							str[i + 1] == str[i]))
						{
							inside_brace = true;
							off = i;
						}

						break;
					}
					case '}':
					{
						//Substitute
						if (inside_brace)
						{
							inside_brace = false;
							i = substitute_curly_brace(str, off, i - off + 1, std::forward<T>(values));
							continue;
						}

						break;
					}
				}

				++i;
			}
			
			return str;
		}

		size_t case_insensitive_find(std::string_view str1, std::string_view str2, size_t off) noexcept;

		bool compare_pattern(std::string_view::const_iterator first, std::string_view::const_iterator last,
			std::string_view::const_iterator first_pattern, std::string_view::const_iterator last_pattern,
			StringCase string_case) noexcept;

		size_t find_pattern(std::string_view::const_iterator first, std::string_view::const_iterator last,
			std::string_view::const_iterator first_pattern, std::string_view::const_iterator last_pattern,
			StringCase string_case) noexcept;

		std::pair<size_t, size_t> find_wildcard(std::string_view str, size_t off) noexcept;


		std::string combine(const Strings &parts, std::optional<std::string_view> delimiter,
			int start, std::optional<int> count);

		Strings tokenize(std::string_view str, std::string_view delimiters,
			std::optional<int> max_splits, StringSplitOptions split_options);

		std::string format(real number, std::string_view format_str);
	} //detail


	/**
		@name Comparing
		Case sensitive/insensitive
		@{
	*/

	///@brief Compares two strings by the given string case sensitivity
	///@details Default case sensitivity is sensitive
	[[nodiscard]] bool Compare(std::string_view str1, std::string_view str2,
		StringCase string_case = StringCase::Sensitive) noexcept;

	///@brief Returns true if the first string starts with the second string
	///@details Default case sensitivity is sensitive
	[[nodiscard]] bool StartsWith(std::string_view str1, std::string_view str2,
		StringCase string_case = StringCase::Sensitive) noexcept;

	///@brief Returns true if the first string ends with the second string
	///@details Default case sensitivity is sensitive
	[[nodiscard]] bool EndsWith(std::string_view str1, std::string_view str2,
		StringCase string_case = StringCase::Sensitive) noexcept;

	///@brief Matches two strings by the given pattern and string case sensitivity
	///@details Default case sensitivity is sensitive.
	///Pattern can include the following wildcards:
	///	* or % to match 0 or more characters.
	///	_ to match exactly one character
	[[nodiscard]] bool Match(std::string_view str, std::string_view pattern,
		StringCase string_case = StringCase::Sensitive) noexcept;

	///@}

	/**
		@name Concatenating
		Strings and numbers
		@{
	*/

	///@brief Concatenates strings and numbers to form a single string
	template <typename T, typename = std::enable_if_t<types::is_string_v<T> || types::is_number_v<T>>,
		typename T2, typename... Tn>
	[[nodiscard]] inline auto Concat(T &&first, T2 &&second, Tn &&...rest)
	{
		std::string str;
		detail::concat_each(str, std::forward<T>(first), std::forward<T2>(second), std::forward<Tn>(rest)...);
		return str;
	}

	///@brief Concatenates all parts to form a single string
	[[nodiscard]] std::string Concat(const Strings &parts);

	///@brief Concatenates all parts (up to count) to form a single string
	[[nodiscard]] std::string Concat(const Strings &parts, int count);

	///@brief Concatenates all parts (from start up to count) to form a single string
	[[nodiscard]] std::string Concat(const Strings &parts, int start, int count);

	///@}

	/**
		@name Splitting
		Using current StringSplitOptions
		@{
	*/

	///@brief Splits a string into multiple parts at each delimiters location
	///@details Use StringSplitOptions to control how the string is splitted.
	///When no options is specified, it removes empty entries
	[[nodiscard]] Strings Split(std::string_view str, std::string_view delimiters,
		StringSplitOptions split_options = StringSplitOptions::RemoveEmptyEntries);

	///@brief Splits a string into multiple parts (until max_splits has been reached) at each delimiters location
	///@details Use StringSplitOptions to control how the string is splitted.
	///When no options is specified, it removes empty entries
	[[nodiscard]] Strings Split(std::string_view str, std::string_view delimiters,
		int max_splits, StringSplitOptions split_options = StringSplitOptions::RemoveEmptyEntries);

	///@}

	/**
		@name Joining
		Strings and numbers
		@{
	*/

	///@brief Joins together strings and numbers to form a single delimited string
	///@details The string is delimited by the given delimiter
	template <typename T, typename = std::enable_if_t<types::is_string_v<T> || types::is_number_v<T>>,
				typename T2, typename... Tn>
	[[nodiscard]] inline auto Join(std::string_view delimiter, T &&first, T2 &&second, Tn &&...rest)
	{
		std::string str;
		detail::join_each(str, delimiter, std::forward<T>(first), std::forward<T2>(second), std::forward<Tn>(rest)...);
		return str;
	}

	///@brief Joins together all parts to form a single delimited string
	///@details The string is delimited by the given delimiter
	[[nodiscard]] std::string Join(std::string_view delimiter, const Strings &parts);

	///@brief Joins together all parts (up to count) to form a single delimited string
	///@details The string is delimited by the given delimiter
	[[nodiscard]] std::string Join(std::string_view delimiter, const Strings &parts, int count);

	///@brief Joins together all parts (from start up to count) to form a single delimited string
	///@details The string is delimited by the given delimiter
	[[nodiscard]] std::string Join(std::string_view delimiter, const Strings &parts, int start, int count);

	///@}

	/**
		@name Formating
		Strings and numbers
		@{
	*/

	///@brief Replaces the format items {x} with the given strings and numbers to form a single string
	///@details Format items must correspond to the actual number of arguments given.
	///	{0}, {1}, ..., {n} and
	///	: for specifing a format
	template <typename T, typename = std::enable_if_t<types::is_string_v<T> || types::is_number_v<T>>,
				typename... Tn>
	[[nodiscard]] inline auto Format(std::string str, T &&first, Tn &&...rest)
	{
		return detail::substitute_curly_braces(str, std::forward_as_tuple(first, rest...));
	}

	///@brief Formats the given number with the given specifiers
	///@details Default specifiers returns a thousands separated number with up to two decimals if needed.
	///	0 for zero placeholder,
	///	# for digit placeholder,
	///	. for decimal point,
	///	, for thousands separator
	[[nodiscard]] std::string Format(real number, std::string_view format_str = "0,.##");

	///@}

	/**
		@name Padding
		Inplace or by copying
		@{
	*/

	///@brief Pads to the left of the string by length with the given characters
	///@details Default padding character is space
	std::string& PadLeft(std::string &str, int length, std::string_view characters = " ");

	///@brief Pads to the right of the string by length with the given characters
	///@details Default padding character is space
	std::string& PadRight(std::string &str, int length, std::string_view characters = " ");

	///@brief Pads to the left of the string by length with the given characters
	///@details Default padding character is space.
	///Returns the result as a copy
	[[nodiscard]] std::string PadLeftCopy(std::string str, int length, std::string_view characters = " ");

	///@brief Pads to the right of the string by length with the given characters
	///@details Default padding character is space.
	///Returns the result as a copy
	[[nodiscard]] std::string PadRightCopy(std::string str, int length, std::string_view characters = " ");

	///@}

	/**
		@name Removing
		Alpha/numeric
		Non-printable
		@{
	*/

	///@brief Removes all non-ascii characters from str
	std::string& RemoveNonAscii(std::string &str) noexcept;

	///@brief Removes all non-alpha characters from str
	std::string& RemoveNonAlpha(std::string &str) noexcept;

	///@brief Removes all non-numeric characters from str
	std::string& RemoveNonNumeric(std::string &str) noexcept;

	///@brief Removes all non- alpha numeric characters from str
	std::string& RemoveNonAlphaNumeric(std::string &str) noexcept;

	///@brief Removes all non-printable characters from str
	std::string& RemoveNonPrintable(std::string &str) noexcept;

	///@brief Removes all non-ascii characters from str
	///@details Returns the result as a copy
	[[nodiscard]] std::string RemoveNonAsciiCopy(std::string str) noexcept;

	///@brief Removes all non-alpha characters from str
	///@details Returns the result as a copy
	[[nodiscard]] std::string RemoveNonAlphaCopy(std::string str) noexcept;

	///@brief Removes all non-numeric characters from str
	///@details Returns the result as a copy
	[[nodiscard]] std::string RemoveNonNumericCopy(std::string str) noexcept;

	///@brief Removes all non- alpha numeric characters from str
	///@details Returns the result as a copy
	[[nodiscard]] std::string RemoveNonAlphaNumericCopy(std::string str) noexcept;

	///@brief Removes all non-printable characters from str
	///@details Returns the result as a copy
	[[nodiscard]] std::string RemoveNonPrintableCopy(std::string str) noexcept;

	///@}

	/**
		@name Replacing
		Case sensitive/insensitive
		Inplace or by copying
		@{
	*/

	///@brief Replaces all occurrences of 'what' with 'with_what'
	///@details Default case sensitivity is sensitive
	std::string& ReplaceAll(std::string &str, std::string_view what, std::string_view with_what,
		StringCase string_case = StringCase::Sensitive);

	///@brief Replaces all occurrences of 'what' with 'with_what'
	///@details Default case sensitivity is sensitive.
	///Returns the result as a copy
	[[nodiscard]] std::string ReplaceAllCopy(std::string str, std::string_view what, std::string_view with_what,
		StringCase string_case = StringCase::Sensitive);

	///@}

	/**
		@name Serializing
		@{
	*/

	///@brief Serializes a given object (that is trivially copyable) to a byte array
	template <typename T>
	[[nodiscard]] inline auto Serialize(const T &object) noexcept
	{
		static_assert(std::is_trivially_copyable_v<T>);

		std::array<std::byte, sizeof(T)> bytes;
		auto first = reinterpret_cast<const std::byte*>(std::addressof(object));
		auto last = first + std::size(bytes);
		std::copy(first, last, std::begin(bytes));

		return bytes;
	}

	///@brief Deserializes a given byte array to a given object (that is trivially copyable)
	template <typename T>
	inline auto& Deserialize(const std::array<std::byte, sizeof(T)> &bytes, T &object) noexcept
	{
		static_assert(std::is_trivially_copyable_v<T>);

		auto first = reinterpret_cast<std::byte*>(std::addressof(object));
		std::copy(std::begin(bytes), std::end(bytes), first);

		return object;
	}

	///@brief Deserializes a given byte array to an object of type T (that is trivially copyable)
	template <typename T>
	[[nodiscard]] inline auto Deserialize(const std::array<std::byte, sizeof(T)> &bytes) noexcept(T{})
	{
		T object;
		return Deserialize(bytes, object);
	}
	
	///@}

	/**
		@name Transforming
		Inplace or by copying
		@{
	*/

	///@brief Transforms all characters of the string to lower case
	std::string& ToLowerCase(std::string &str) noexcept;

	///@brief Transforms all characters of the string to UPPER CASE
	std::string& ToUpperCase(std::string &str) noexcept;

	///@brief Transforms all characters of the string to lower case
	///@details Returns the result as a copy
	[[nodiscard]] std::string ToLowerCaseCopy(std::string str);

	///@brief Transforms all characters of the string to UPPER CASE
	///@details Returns the result as a copy
	[[nodiscard]] std::string ToUpperCaseCopy(std::string str);

	///@}

	/**
		@name Trimming
		Inplace or by copying
		@{
	*/

	///@brief Trims string, removes characters from left side
	///@details Removes whitespaces (space, tab and new line) as default
	std::string& TrimLeft(std::string &str, std::string_view characters = " \t\n") noexcept;

	///@brief Trims string, removes characters from right side
	///@details Removes whitespaces (space, tab and new line) as default
	std::string& TrimRight(std::string &str, std::string_view characters = " \t\n") noexcept;

	///@brief Trims string, removes characters from both sides
	///@details Removes whitespaces (space, tab and new line) as default
	std::string& Trim(std::string &str, std::string_view characters = " \t\n") noexcept;

	///@brief Trims string, removes characters from left side
	///@details Removes whitespaces (space, tab and new line) as default.
	///Returns the result as a copy
	[[nodiscard]] std::string TrimLeftCopy(std::string str, std::string_view characters = " \t\n");

	///@brief Trims string, removes characters from right side
	///@details Removes whitespaces (space, tab and new line) as default.
	///Returns the result as a copy
	[[nodiscard]] std::string TrimRightCopy(std::string str, std::string_view characters = " \t\n");

	///@brief Trims string, removes characters from both sides
	///@details Removes whitespaces (space, tab and new line) as default.
	///Returns the result as a copy
	[[nodiscard]] std::string TrimCopy(std::string str, std::string_view characters = " \t\n");

	///@}
} //ion::utilities::string

#endif