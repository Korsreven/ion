/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	resources
File:	IonResource.h
-------------------------------------------
*/

#ifndef ION_RESOURCE_H
#define ION_RESOURCE_H

#include "managed/IonManagedObject.h"

namespace ion::resources
{
	template <typename T>
	class Resource : public managed::ManagedObject<T>
	{
	};
} //ion::resources

#endif