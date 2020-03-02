/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	types
File:	IonStrongType.h
-------------------------------------------
*/

#ifndef ION_STRONG_TYPE_H
#define ION_STRONG_TYPE_H

namespace ion::types
{
	template <typename T, typename Tag>
	class StrongType
	{
		private:

			T value_;

		public:

			using value_type = T;

			//Construct a StrongType containing a T by copying the value
			constexpr explicit StrongType(const T &value) noexcept :
				value_{value}
			{
				//Empty
			}

			//Construct a StrongType containing a T by moving the value
			constexpr explicit StrongType(T &&value) noexcept :
				value_{std::move(value)}
			{
				//Empty
			}


			/*
				Observers
			*/
			
			//Get a mutable reference to the contained value
			[[nodiscard]] constexpr auto& Get() noexcept
			{
				return value_;
			}

			//Get an immutable reference to the contained value
			[[nodiscard]] constexpr auto& Get() const noexcept
			{
				return value_;
			}
	};
} //ion::types

#endif