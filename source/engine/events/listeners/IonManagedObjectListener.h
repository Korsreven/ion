/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonManagedObjectListener.h
-------------------------------------------
*/

#ifndef ION_MANAGED_OBJECT_LISTENER_H
#define ION_MANAGED_OBJECT_LISTENER_H

#include <type_traits>
#include "IonListener.h"

namespace ion::events::listeners
{
	//A class representing a listener that listens to managed object events
	template <typename ObjectT, typename OwnerT, typename ListenerT = void>
	struct ManagedObjectListener :
		Listener<std::conditional_t<std::is_same_v<ListenerT, void>, ManagedObjectListener<ObjectT, OwnerT>, ListenerT>>
	{
		/*
			Events
		*/

		//Called right after an object has been created or adopted, with a reference to the object
		virtual void ObjectCreated([[maybe_unused]] ObjectT &object) noexcept
		{
			//Optional to override
		}

		//Called right before an object is removed or orphaned, with a reference to the object
		//Return false from this function if the removal should be canceled
		virtual bool ObjectRemovable([[maybe_unused]] ObjectT &object) noexcept
		{
			//Optional to override
			return true;
		}

		//Called right after an object has been removed or orphaned, with a reference to the object
		virtual void ObjectRemoved([[maybe_unused]] ObjectT &object) noexcept
		{
			//Optional to override
		}


		//Called right after a held object has been moved, with a reference to the new owner
		//A held object is an object that is currently being managed by an owner
		virtual void ObjectMoved([[maybe_unused]] OwnerT &owner) noexcept
		{
			//Optional to override
		}
	};
} //ion::events::listeners

#endif