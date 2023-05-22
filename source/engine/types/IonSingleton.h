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
	///@brief A class providing a single instance of the derived type
	///@details Allowed: struct Foo : Singleton&lt;Foo&gt; {};
	///Not allowed: struct Foo : Singleton&lt;Bar&gt; {};
	///Not allowed:	Singleton&lt;int&gt; foobar;
	template <typename T>
	class Singleton
	{
		friend T;

		private:

			///@brief Can only be instantiated if inherited from by T
			Singleton() = default;

		public:

			///@brief Returns a static instance of T
			///@details T is lazily constructed, meaning the first time this is called
			[[nodiscard]] static auto& Instance() noexcept
			{
				static T instance;
				return instance;
			}
	};
} //ion::types

#endif