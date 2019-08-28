/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonConvert.h
-------------------------------------------
*/

#ifndef _ION_CONVERT_
#define _ION_CONVERT_

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

#undef min
#undef max

namespace ion::utilities::convert
{
	namespace detail
	{
		//String to number conversions
		//Tries to convert the entire string

		template <typename T>
		struct string_to_number_impl
		{	
			//For char, char16_t, char32_t and wchar_t
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				if constexpr (std::numeric_limits<T>::is_signed)
					return std::strtol(std::data(str), end, 10);
				else
					return std::strtoul(std::data(str), end, 10);
			}
		};

		template <>
		struct string_to_number_impl<bool>
		{
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				return !!std::strtol(std::data(str), end, 10);
			}
		};

		template <>
		struct string_to_number_impl<signed char>
		{
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				return static_cast<signed char>(std::strtol(std::data(str), end, 10));
			}
		};

		template <>
		struct string_to_number_impl<unsigned char>
		{
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				return static_cast<unsigned char>(std::strtoul(std::data(str), end, 10));
			}
		};

		template <>
		struct string_to_number_impl<short>
		{
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				return static_cast<short>(std::strtol(std::data(str), end, 10));
			}
		};

		template <>
		struct string_to_number_impl<unsigned short>
		{
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				return static_cast<unsigned short>(std::strtoul(std::data(str), end, 10));
			}
		};

		template <>
		struct string_to_number_impl<int>
		{
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				return static_cast<int>(std::strtol(std::data(str), end, 10));
			}
		};

		template <>
		struct string_to_number_impl<unsigned int>
		{
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				return static_cast<unsigned int>(std::strtoul(std::data(str), end, 10));
			}
		};

		template <>
		struct string_to_number_impl<long>
		{
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				return std::strtol(std::data(str), end, 10);
			}
		};

		template <>
		struct string_to_number_impl<unsigned long>
		{
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				return std::strtoul(std::data(str), end, 10);
			}
		};

		template <>
		struct string_to_number_impl<long long>
		{
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				return std::strtoll(std::data(str), end, 10);
			}
		};

		template <>
		struct string_to_number_impl<unsigned long long>
		{
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				return std::strtoull(std::data(str), end, 10);
			}
		};

		template <>
		struct string_to_number_impl<float>
		{
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				return std::strtof(std::data(str), end);
			}
		};

		template <>
		struct string_to_number_impl<double>
		{
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				return std::strtod(std::data(str), end);
			}
		};

		template <>
		struct string_to_number_impl<long double>
		{
			inline auto operator()(std::string_view str, char **end = nullptr) const noexcept
			{
				return std::strtold(std::data(str), end);
			}
		};

		template <typename T>
		struct string_to_number :
			string_to_number_impl<std::remove_cv_t<T>>
		{
		};

		template <typename T>
		inline auto entire_string_to_number(std::string_view str) noexcept
		{
			char *end;
			auto value = string_to_number<T>()(str, &end);
			return (end != std::data(str) && !*end ?
					std::make_optional<T>(value) :
					std::nullopt);
		}


		//String to number conversions
		//Tries to convert the first part of the string

		template <typename T>
		struct first_part_of_string_to_number_impl
		{
			//For more narrow types than int
			inline auto operator()(std::string_view str) const noexcept
			{
				return static_cast<T>(std::atoi(std::data(str)));
			}
		};

		template <>
		struct first_part_of_string_to_number_impl<int>
		{
			inline auto operator()(std::string_view str) const noexcept
			{
				return std::atoi(std::data(str));
			}
		};

		template <>
		struct first_part_of_string_to_number_impl<unsigned int>
		{
			inline auto operator()(std::string_view str) const noexcept
			{
				return static_cast<unsigned int>(std::atoll(std::data(str)));
			}
		};

		template <>
		struct first_part_of_string_to_number_impl<long>
		{
			inline auto operator()(std::string_view str) const noexcept
			{
				return std::atol(std::data(str));
			}
		};

		template <>
		struct first_part_of_string_to_number_impl<unsigned long>
		{
			inline auto operator()(std::string_view str) const noexcept
			{
				return static_cast<unsigned long>(std::atoll(std::data(str)));
			}
		};

		template <>
		struct first_part_of_string_to_number_impl<long long>
		{
			inline auto operator()(std::string_view str) const noexcept
			{
				return std::atoll(std::data(str));
			}
		};

		template <>
		struct first_part_of_string_to_number_impl<unsigned long long>
		{
			inline auto operator()(std::string_view str) const noexcept
			{
				return std::strtoull(std::data(str), nullptr, 10);
			}
		};

		template <>
		struct first_part_of_string_to_number_impl<float>
		{
			inline auto operator()(std::string_view str) const noexcept
			{
				return static_cast<float>(std::atof(std::data(str)));
			}
		};

		template <>
		struct first_part_of_string_to_number_impl<double>
		{
			inline auto operator()(std::string_view str) const noexcept
			{
				return std::atof(std::data(str));
			}
		};

		template <>
		struct first_part_of_string_to_number_impl<long double>
		{
			inline auto operator()(std::string_view str) const noexcept
			{
				return std::strtold(std::data(str), nullptr);
			}
		};

		template <typename T>
		struct first_part_of_string_to_number :
			first_part_of_string_to_number_impl<std::remove_cv_t<T>>
		{
		};

		constexpr auto is_digit(char c) noexcept
		{
			//Faster than a naive implementation or std::isdigit
			//Unroll all true cases
			switch (c)
			{
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				return true;

				default:
				return false;
			}
		}

		template <typename T>
		inline auto first_part_to_number(std::string_view str) noexcept
		{
			auto iter = std::cbegin(str);

			//Check for sign
			if (iter != std::cend(str))
			{
				switch (*iter)
				{
					case '-':
					case '+':
					++iter;
					break;
				}
			}

			if constexpr (std::is_floating_point_v<T>)
			{
				//Check for decimal point
				if (iter != std::cend(str) && *iter == '.')
					++iter;
			}

			return iter != std::cend(str) && is_digit(*iter) ?
				std::make_optional<T>(first_part_of_string_to_number<T>()(str)) :
				std::nullopt;
		}

		template <typename T>
		inline auto first_numeric_part_to_number(std::string_view str) noexcept
		{
			auto iter = std::find_if(std::cbegin(str), std::cend(str), is_digit);

			if (iter == std::cend(str))
				return std::optional<T>{};

			if constexpr (std::is_floating_point_v<T>)
			{
				//Check for decimal point
				if (iter != std::cbegin(str) && *(iter - 1) == '.')
					--iter;
			}

			//Check for sign
			if (iter != std::cbegin(str) && *(iter - 1) == '-')
				--iter;

			str.remove_prefix(iter - std::cbegin(str));
			return std::make_optional<T>(first_part_of_string_to_number<T>()(str));
		}


		//Max digits for a given floating point
		//Used to preallocate a buffer of certain size to sprintf

		template <typename T>
		struct max_digits_impl
		{
			static constexpr auto value = 15;
		};

		template <>
		struct max_digits_impl<double>
		{
			static constexpr auto value = 24;
		};

		template <>
		struct max_digits_impl<long double>
		{
			static constexpr auto value = 28;
		};

		template <typename T>
		struct max_digits :
			max_digits_impl<std::remove_cv_t<T>>
		{
		};

		template <typename T>
		constexpr auto max_digits_v = max_digits<T>::value;


		//Format specifier for a given floating point
		//Used as format to sprintf

		template <typename T>
		struct format_specifier_impl
		{
			static constexpr auto value = "f";
		};

		template <>
		struct format_specifier_impl<double>
		{
			static constexpr auto value = "lf";
		};

		template <>
		struct format_specifier_impl<long double>
		{
			static constexpr auto value = "Lf";
		};

		template <typename T>
		struct format_specifier :
			format_specifier_impl<std::remove_cv_t<T>>
		{
		};

		template <typename T>
		constexpr auto format_specifier_v = format_specifier<T>::value;


		//Format specifier (g) for a given floating point
		//Used as format to sprintf

		template <typename T>
		struct g_format_specifier_impl
		{
			static constexpr auto value = "%6.9g";
		};

		template <>
		struct g_format_specifier_impl<double>
		{
			static constexpr auto value = "%10.17lg";
		};

		template <>
		struct g_format_specifier_impl<long double>
		{
			static constexpr auto value = "%10.17Lg";
		};

		template <typename T>
		struct g_format_specifier :
			g_format_specifier_impl<std::remove_cv_t<T>>
		{
		};

		template <typename T>
		constexpr auto g_format_specifier_v = g_format_specifier<T>::value;


		//Number to string conversions
		//Uses sprintf directly instead of to_string

		template <typename T>
		inline auto floating_to_string(T x)
		{
			std::array<char, max_digits_v<T> + 1> str;
			std::sprintf(std::data(str), g_format_specifier_v<T>, x);
			auto iter = std::find_if_not(std::cbegin(str), std::cend(str),
				[](auto c) noexcept
				{
					return c == ' ';
				});

			return std::string(&*iter);
		}

		template <typename T>
		inline auto floating_to_string(T x, std::optional<int> precision)
		{
			auto format = (precision ? "%." + std::to_string(*precision) : "%") + format_specifier_v<T>;
			auto size = std::snprintf(nullptr, 0, std::data(format), x);

			std::string str(size, '\0');
			std::sprintf(&str[0], std::data(format), x);

			return str;
		}
	} //detail


	/*
		String to number
		Entire string must be numeric
	*/

	//Converts the entire given string to a number
	//Returns an empty optional if the conversion fails
	template <typename T>
	[[nodiscard]] inline auto To(const std::string &str) noexcept
		//Make sure that str is null-terminated
	{
		static_assert(std::is_arithmetic_v<T>);
		return detail::entire_string_to_number<T>(str);
	}

	//Converts the entire given NTCS to a number
	//Returns an empty optional if the conversion fails
	template <typename T>
	[[nodiscard]] inline auto To(const char *str) noexcept
		//Make sure that str is null-terminated
	{
		static_assert(std::is_arithmetic_v<T>);
		return detail::entire_string_to_number<T>(str);
	}


	/*
		String to number
		First part of the string must be numeric
	*/

	//Converts the first part of the given string to a number
	//Returns an empty optional if the conversion fails
	template <typename T>
	[[nodiscard]] inline auto FirstPartTo(const std::string &str) noexcept
		//Make sure that str is null-terminated
	{
		static_assert(std::is_arithmetic_v<T>);
		return detail::first_part_to_number<T>(str);
	}

	//Converts the first part of the given NTCS to a number
	//Returns an empty optional if the conversion fails
	template <typename T>
	[[nodiscard]] inline auto FirstPartTo(const char *str) noexcept
		//Make sure that str is null-terminated
	{
		static_assert(std::is_arithmetic_v<T>);
		return detail::first_part_to_number<T>(str);
	}


	/*
		String to number
		First numeric part of the string
	*/

	//Converts the first numeric part of the given string to a number
	//Returns an empty optional if no numeric part is found
	template <typename T>
	[[nodiscard]] inline auto FirstNumericPartTo(const std::string &str) noexcept
		//Make sure that str is null-terminated
	{
		static_assert(std::is_arithmetic_v<T>);
		return detail::first_numeric_part_to_number<T>(str);
	}

	//Converts the first numeric part of the given NTCS to a number
	//Returns an empty optional if no numeric part is found
	template <typename T>
	[[nodiscard]] inline auto FirstNumericPartTo(const char *str) noexcept
		//Make sure that str is null-terminated
	{
		static_assert(std::is_arithmetic_v<T>);
		return detail::first_numeric_part_to_number<T>(str);
	}


	/*
		Number to string
		Integers and floating points
	*/

	//Converts the given number to a string
	template <typename T>
	[[nodiscard]] inline auto ToString(T value)
	{
		static_assert(std::is_arithmetic_v<T>);

		if constexpr (std::is_integral_v<T>)
			return std::to_string(value);
		else
			return detail::floating_to_string(value);
	}

	//Converts the given floating point number, with custom precision, to a string
	template <typename T>
	[[nodiscard]] inline auto ToString(T value, std::optional<int> precision)
	{
		static_assert(std::is_floating_point_v<T>);
		assert(precision.value_or(0) >= 0);
		return detail::floating_to_string(value, precision);
	}
} //ion::utilities::convert

#endif