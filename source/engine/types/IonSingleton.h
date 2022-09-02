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

#ifndef ION_SINGLETON_H
#define ION_SINGLETON_H

namespace ion::types
{
	//A class providing a singleton instance of the derived type
	//Allowed:		struct Foo : Singleton<Foo> {};
	//Not allowed:	struct Foo : Singleton<Bar> {};
	//Not allowed:	Singleton<int> foobar;
	template <typename T>
	class Singleton
	{
		friend T;

		private:

			//Can only be instantiated if inherited from by T
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