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
	//A class representing a strong type of any native type T
	//Different strong types can not be mixed, even if their native types are the same
	template <typename T, typename Tag>
	class StrongType
	{
		private:

			T value_;

		public:

			using value_type = T;

			//Constructs a StrongType containing a T by copying the value
			constexpr explicit StrongType(const T &value) noexcept :
				value_{value}
			{
				//Empty
			}

			//Constructs a StrongType containing a T by moving the value
			constexpr explicit StrongType(T &&value) noexcept :
				value_{std::move(value)}
			{
				//Empty
			}


			/*
				Observers
			*/
			
			//Gets a mutable reference to the contained value
			[[nodiscard]] constexpr auto& Get() noexcept
			{
				return value_;
			}

			//Gets an immutable reference to the contained value
			[[nodiscard]] constexpr auto& Get() const noexcept
			{
				return value_;
			}
	};
} //ion::types

#endif