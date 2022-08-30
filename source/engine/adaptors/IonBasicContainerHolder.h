/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	adaptors
File:	IonBasicContainerHolder.h
-------------------------------------------
*/

#ifndef ION_BASIC_CONTAINER_HOLDER_H
#define ION_BASIC_CONTAINER_HOLDER_H

#include <initializer_list>

namespace ion::adaptors
{
	//A class that can hold any container by value
	template <typename Container>
	class BasicHolder
	{
		protected:

			Container container_;

		public:

			//Default constructor
			BasicHolder() = default;

			//Construct basic holder from the given container by copy
			explicit BasicHolder(const Container &container) :
				container_{container}
			{
				//Empty
			}

			//Construct basic holder from the given container by move
			explicit BasicHolder(Container &&container) noexcept :
				container_{std::move(container)}
			{
				//Empty
			}

			//Construct basic holder from the given initializer list
			explicit BasicHolder(std::initializer_list<typename Container::value_type> list) :
				container_{list}
			{
				//Empty
			}
	};


	//A class that can hold any container by reference
	template <typename Container>
	class BasicHolder<Container&>
	{
		protected:

			Container &container_;

		public:

			//No default constructor
			constexpr BasicHolder() = delete;

			//Construct basic holder from the given reference
			constexpr explicit BasicHolder(Container &container) noexcept :
				container_{container}
			{
				//Empty
			}
	};
} //ion::adaptors

#endif