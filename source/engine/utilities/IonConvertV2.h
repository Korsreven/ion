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

#ifndef _ION_CONVERT_V2_
#define _ION_CONVERT_V2_

#include <array>
#include <cassert>
#include <charconv>
#include <optional>
#include <string>
#include <string_view>

#undef min
#undef max

namespace ion::utilities::experimental::convert
{
	namespace detail
	{
		//Max digits for a given numeric type
		//Used to preallocate a buffer of certain size to std::to_chars

		template <typename T>
		struct max_digits_impl
		{
			static constexpr auto value = 28;
		};

		template <>
		struct max_digits_impl<bool>
		{
			static constexpr auto value = 1;
		};

		template <>
		struct max_digits_impl<char>
		{
			static constexpr auto value = 4;
		};

		template <>
		struct max_digits_impl<unsigned char>
		{
			static constexpr auto value = 3;
		};

		template <>
		struct max_digits_impl<signed char>
		{
			static constexpr auto value = 4;
		};

		template <>
		struct max_digits_impl<short>
		{
			static constexpr auto value = 6;
		};

		template <>
		struct max_digits_impl<unsigned short>
		{
			static constexpr auto value = 5;
		};

		template <>
		struct max_digits_impl<int>
		{
			static constexpr auto value = 11;
		};

		template <>
		struct max_digits_impl<unsigned int>
		{
			static constexpr auto value = 10;
		};

		template <>
		struct max_digits_impl<long>
		{
			static constexpr auto value = 11;
		};

		template <>
		struct max_digits_impl<unsigned long>
		{
			static constexpr auto value = 10;
		};

		template <>
		struct max_digits_impl<long long>
		{
			static constexpr auto value = 20;
		};

		template <>
		struct max_digits_impl<unsigned long long>
		{
			static constexpr auto value = 20;
		};

		template <>
		struct max_digits_impl<float>
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


		//String to number conversions
		//Tries to convert the entire string

		template <typename T>
		inline auto entire_string_to_number(std::string_view str) noexcept
		{
			T value;
			auto end = std::data(str) + std::size(str);
			auto [ptr, error] = std::from_chars(std::data(str), end, value);

			return ptr == end ?
				std::make_optional<T>(value) :
				std::nullopt;
		}


		//String to number conversions
		//Tries to convert the first part of the string

		template <typename T>
		inline auto first_part_of_string_to_number(std::string_view str) noexcept
		{
			T value;
			auto [ptr, error] = std::from_chars(std::data(str), std::data(str) + std::size(str), value);

			return error != std::errc::invalid_argument ?
				std::make_optional<T>(value) :
				std::nullopt;
		}


		//String to number conversions
		//Tries to convert the first numeric part of the string

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
			return first_part_of_string_to_number<T>(str);
		}


		//Number to string conversions (general)

		template <typename T>
		inline auto number_to_string(T x)
		{
			std::array<char, max_digits_v<T>> chars;
			auto [ptr, error] = std::to_chars(std::data(chars), std::data(chars) + std::size(chars), x);
			assert(error != std::errc::value_too_large);
			return std::string(std::data(chars), ptr);
		}

		//Floating point to string conversions (fixed)

		template <typename T>
		inline auto floating_point_to_string(T x, std::optional<int> precision)
		{
			std::array<char, max_digits_v<T>> chars;
			auto [ptr, error] = precision ?
				std::to_chars(std::data(chars), std::data(chars) + std::size(chars), x, std::chars_format::fixed, *precision) :
				std::to_chars(std::data(chars), std::data(chars) + std::size(chars), x, std::chars_format::fixed);
			assert(error != std::errc::value_too_large);
			return std::string(std::data(chars), ptr);
		}
	} //detail


	/*
		String to number
		Entire string must be numeric
	*/

	//Converts the entire given string to a number
	//Returns an empty optional if the conversion fails
	template <typename T>
	[[nodiscard]] inline auto To(std::string_view str) noexcept
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
	[[nodiscard]] inline auto FirstPartTo(std::string_view str) noexcept
	{
		static_assert(std::is_arithmetic_v<T>);
		return detail::first_part_of_string_to_number<T>(str);
	}


	/*
		String to number
		Some part of the string must be numeric
	*/

	//Converts the first numeric part of the given string to a number
	//Returns an empty optional if no numeric part is found
	template <typename T>
	[[nodiscard]] inline auto FirstNumericPartTo(std::string_view str) noexcept
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
		return detail::number_to_string(value);
	}

	//Converts the given floating point number, with custom precision, to a string
	template <typename T>
	[[nodiscard]] inline auto ToString(T value, std::optional<int> precision)
	{
		static_assert(std::is_floating_point_v<T>);
		assert(precision.value_or(0) >= 0);
		return detail::floating_point_to_string(value, precision);
	}
} //ion::utilities::convert

#endif