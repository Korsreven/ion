/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script
File:	IonScriptTypes.h
-------------------------------------------
*/

#ifndef ION_SCRIPT_TYPES_H
#define ION_SCRIPT_TYPES_H

#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "types/IonStrongType.h"
#include "types/IonTypes.h"

namespace ion::script
{
	namespace script_types::detail
	{
		template <typename T, typename Tag>
		struct script_type : types::StrongType<T, Tag>
		{
			constexpr script_type(const T &value) noexcept :
				types::StrongType<T, Tag>{value}
			{
				//Empty
			}

			constexpr script_type(T &&value) noexcept :
				types::StrongType<T, Tag>{std::move(value)}
			{
				//Empty
			}
		};

		template <typename T, typename Tag>
		struct arithmetic_type : script_type<T, Tag>
		{
			static_assert(std::is_arithmetic_v<T>);
			using script_type<T, Tag>::script_type;

			///@brief Returns the value of this argument casted to another arithmetic type
			template <typename U>		
			[[nodiscard]] constexpr auto As() const noexcept
			{
				static_assert(std::is_arithmetic_v<U>);
				static_assert(std::is_convertible_v<T, U>);
				return static_cast<U>(types::StrongType<T, Tag>::Get());
			}
		};
	} //script_types::detail


	///@brief A class representing different script types
	struct ScriptType
	{
		///@brief A script type representing a boolean with an underlying type bool
		struct Boolean final :
			script_types::detail::arithmetic_type<bool, Boolean>
		{
			using arithmetic_type::arithmetic_type;
		};

		///@brief A script type representing a color with an underlying type Color
		struct Color final :
			script_types::detail::script_type<graphics::utilities::Color, Color>
		{
			using script_type::script_type;
		};

		///@brief A script type representing an enumerable with an underlying type std::string
		struct Enumerable final :
			script_types::detail::script_type<std::string, Enumerable>
		{
			using script_type::script_type;
		};

		///@brief A script type representing a floating-point with an underlying type float80
		struct FloatingPoint final :
			script_types::detail::arithmetic_type<float80, FloatingPoint>
		{
			using arithmetic_type::arithmetic_type;
		};

		///@brief A script type representing an integer with an underlying type int64
		struct Integer final :
			script_types::detail::arithmetic_type<int64, Integer>
		{
			using arithmetic_type::arithmetic_type;
		};

		///@brief A script type representing a string with an underlying type std::string
		struct String final :
			script_types::detail::script_type<std::string, String>
		{
			using script_type::script_type;
		};

		///@brief A script type representing a vector2 with an underlying type Vector2
		struct Vector2 final :
			script_types::detail::script_type<graphics::utilities::Vector2, Vector2>
		{
			using script_type::script_type;
		};

		///@brief A script type representing a vector3 with an underlying type Vector3
		struct Vector3 final :
			script_types::detail::script_type<graphics::utilities::Vector3, Vector3>
		{
			using script_type::script_type;
		};
	};
} //ion::script

#endif