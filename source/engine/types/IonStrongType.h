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

#ifndef _ION_STRONG_TYPE_
#define _ION_STRONG_TYPE_

namespace ion::types
{
	template <typename T, typename Tag>
	class StrongType
	{
		private:

			T value_;

		public:

			using value_type = T;

			constexpr explicit StrongType(const T &value) noexcept :
				value_{value}
			{
				//Empty
			}

			constexpr explicit StrongType(T &&value) noexcept :
				value_{std::move(value)}
			{
				//Empty
			}


			/*
				Observers
			*/
			
			[[nodiscard]] constexpr auto& Get() noexcept
			{
				return value_;
			}

			[[nodiscard]] constexpr const auto& Get() const noexcept
			{
				return value_;
			}
	};
} //ion::types

#endif