/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	types
File:	IonTypes.h
-------------------------------------------
*/

#ifndef ION_TYPES_H
#define ION_TYPES_H

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

#include "IonTypeTraits.h"

#undef min
#undef max

namespace ion::types
{
	/*
		Fixed width integer types
	*/

	using int8 = std::int8_t;
	using uint8 = std::uint8_t;
	using int16 = std::int16_t;
	using uint16 = std::uint16_t;
	using int32 = std::int32_t;
	using uint32 = std::uint32_t;
	using int64 = std::int64_t;
	using uint64 = std::uint64_t;


	/*
		Fixed width floating point types
	*/

	using float32 = float;
	using float64 = double;
	using float80 = long double;


	/*
		Real (variable width)
		Engine default floating point type
	*/

	#ifdef ION_EXTENDED_PRECISION
	using real = float80;
	#elif ION_DOUBLE_PRECISION
	using real = float64;
	#else //ION_SINGLE_PRECISION
	using real = float32;
	#endif


	/*
		Duration (variable width)
		Engine default floating point duration in seconds
	*/

	using duration = std::chrono::duration<real>;


	/*
		Index type (signed integer)
		Engine default signed index type for use with the STL
		The type is the signed version of std::size_t
	*/

	using index_t = std::make_signed_t<std::size_t>;


	/*
		Strings (multiple std::string)
		Engine default
	*/

	using Strings = std::vector<std::string>;


	inline namespace type_literals
	{
		/*
			User defined literals (UDLs)
			For fixed width integer types
		*/

		constexpr auto operator""_i8(unsigned long long value) noexcept
		{
			return static_cast<int8>(value);
		}

		constexpr auto operator""_ui8(unsigned long long value) noexcept
		{
			return static_cast<uint8>(value);
		}

		constexpr auto operator""_i16(unsigned long long value) noexcept
		{
			return static_cast<int16>(value);
		}

		constexpr auto operator""_ui16(unsigned long long value) noexcept
		{
			return static_cast<uint16>(value);
		}

		constexpr auto operator""_i32(unsigned long long value) noexcept
		{
			return static_cast<int32>(value);
		}

		constexpr auto operator""_ui32(unsigned long long value) noexcept
		{
			return static_cast<uint32>(value);
		}

		constexpr auto operator""_i64(unsigned long long value) noexcept
		{
			return static_cast<int64>(value);
		}

		constexpr auto operator""_ui64(unsigned long long value) noexcept
		{
			return static_cast<uint64>(value);
		}


		/*
			User defined literals (UDLs)
			For fixed width floating point types
		*/

		constexpr auto operator""_f32(long double value) noexcept
		{
			return static_cast<float32>(value);
		}

		constexpr auto operator""_f64(long double value) noexcept
		{
			return static_cast<float64>(value);
		}

		constexpr auto operator""_f80(long double value) noexcept
		{
			return static_cast<float80>(value);
		}


		/*
			User defined literals (UDLs)
			For real type
		*/

		constexpr auto operator""_r(long double value) noexcept
		{
			return static_cast<real>(value);
		}


		/*
			User defined literals (UDLs)
			For duration type
		*/

		constexpr auto operator""_sec(long double value) noexcept
		{
			return duration{static_cast<real>(value)};
		}

		
		/*
			User defined literals (UDLs)
			For index type
		*/

		constexpr auto operator""_idx(unsigned long long value) noexcept
		{
			return static_cast<index_t>(value);
		}
	} //type_literals
} //ion::types


namespace ion
{
	/*
		Pull common types out of the types namespace
	*/

	using types::Strings;
} //ion


/*
	Pull common POD types out of the ion namespace
*/

using ion::types::int8;
using ion::types::uint8;
using ion::types::int16;
using ion::types::uint16;
using ion::types::int32;
using ion::types::uint32;
using ion::types::int64;
using ion::types::uint64;

using ion::types::float32;
using ion::types::float64;
using ion::types::float80;
using ion::types::real;

using ion::types::duration;
using ion::types::index_t;

#endif