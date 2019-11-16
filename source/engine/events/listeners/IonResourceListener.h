/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonResourceListener.h
-------------------------------------------
*/

#ifndef ION_RESOURCE_LISTENER_H
#define ION_RESOURCE_LISTENER_H

#include "IonListener.h"

namespace ion::events::listeners
{
	template <typename T>
	struct ResourceListener : Listener<ResourceListener<T>>
	{
		/*
			Events
		*/

		//Called right after a resource has been created or adopted, with a reference to the resource
		virtual void ResourceCreated(T &resource) noexcept = 0;


		//Called right before a resource is removed or orphaned, with a reference to the resource
		//Return false from this function if the removal should be canceled
		virtual bool ResourceRemovable([[maybe_unused]] T &resource) noexcept
		{
			//Optional to override
			return true;
		}

		//Called right after a resource has been removed or orphaned, with a reference to the resource
		virtual void ResourceRemoved(T &resource) noexcept = 0;
	};
} //ion::events::listeners

#endif