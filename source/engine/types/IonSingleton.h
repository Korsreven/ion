/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	types
File:	IonSingleton.h
-------------------------------------------
*/

#ifndef _ION_SINGLETON_
#define _ION_SINGLETON_

namespace ion::types
{
	template <typename T>
	class Singleton
	{
		friend T;

		private:

			//Can only be instantiated if inherited from by T
			//Allowed:		struct Foo : Singleton<Foo> {};
			//Not allowed:	struct Foo : Singleton<Bar> {};
			//Not allowed:	Singleton<int> integer;
			Singleton() = default;

		public:

			//Returns a static instance of T
			//T is lazily constructed, meaning the first time this is called
			[[nodiscard]] static auto& Instance() noexcept
			{
				static T instance;
				return instance;
			}
	};
} //ion::types

#endif