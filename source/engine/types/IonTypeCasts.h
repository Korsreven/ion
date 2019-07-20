/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	types
File:	IonTypeCasts.h
-------------------------------------------
*/

#ifndef _ION_TYPE_CASTS_
#define _ION_TYPE_CASTS_

#include <cassert>
#include <limits>
#include <type_traits>

namespace ion::types
{
	/*
		Safe type cast
	*/

	//Safely cast from one type to another
	//In debug it will assert that the value will remain the same
	//In release it will just do a static_cast to be efficient
	template <typename To, typename From>
	[[nodiscard]] inline To safe_cast(From value) noexcept
	{
		static_assert(std::is_arithmetic_v<From> && std::is_arithmetic_v<To> &&
						!std::is_same_v<From, To>);

		//Signed to unsigned conversion (assert not negative)
		if constexpr (std::is_signed_v<From> && std::is_unsigned_v<To>)
			assert(value >= To{});

		//Potentially lossy conversion (assert limits)
		//To a type with less binary precision
		if constexpr (std::numeric_limits<To>::digits < std::numeric_limits<From>::digits)
		{
			//Integer to floating point
			if constexpr (std::is_integral_v<From> && std::is_floating_point_v<To>)
			{
				//Assert min limit
				//Signed integer to floating point
				if constexpr (std::is_signed_v<From>)
					assert(value >= min_whole_number_v<To>);

				//Assert max limit
				assert(value <= max_whole_number_v<To>);
			}
			else
			{
				//Assert min limit
				//Signed to signed
				if constexpr (std::is_signed_v<From> && std::is_signed_v<To>)
					assert(value >= static_cast<From>(std::numeric_limits<To>::min()));
				
				//Assert max limit
				//To a type with less byte size or to signed type
				if constexpr (sizeof(To) < sizeof(From) || std::is_signed_v<To>)
					assert(value <= static_cast<From>(std::numeric_limits<To>::max()));
			}
		}

		return static_cast<To>(value);
	}


	/*
		Safe integral signedness cast
	*/

	//Safely cast from unsigned to signed integral type
	//In debug it will assert that the value will remain the same
	//In release it will just do a static_cast to be efficient
	template <typename T>
	[[nodiscard]] inline auto signed_cast(T value) noexcept
	{
		static_assert(std::is_unsigned_v<T>); //false for floating point types
		assert(value <= static_cast<T>(std::numeric_limits<std::make_signed_t<T>>::max()));
		return static_cast<std::make_signed_t<T>>(value);
	}

	//Safely cast from signed to unsigned integral type
	//In debug it will assert that the value will remain the same
	//In release it will just do a static_cast to be efficient
	template <typename T>
	[[nodiscard]] inline auto unsigned_cast(T value) noexcept
	{
		static_assert(std::is_signed_v<T> && std::is_integral_v<T>);
		assert(value >= T{});
		return static_cast<std::make_unsigned_t<T>>(value);
	}
} //ion::types


namespace ion
{
	/*
		Pull type casts out of the types namespace
	*/

	using types::safe_cast;
	using types::signed_cast;
	using types::unsigned_cast;
} //ion

#endif