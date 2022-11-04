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

#include <type_traits>
#include "IonManagedObjectListener.h"

namespace ion::events::listeners
{
	///@brief A class representing a listener that listens to resource events
	template <typename ResourceT, typename OwnerT, typename ListenerT = void>
	struct ResourceListener :
		ManagedObjectListener<ResourceT, OwnerT, std::conditional_t<std::is_same_v<ListenerT, void>, ResourceListener<ResourceT, OwnerT>, ListenerT>>
	{
		/**
			@name Events
			@{
		*/

		///@brief Called right after a resource has been prepared, with a reference to the resource
		virtual void ResourcePrepared([[maybe_unused]] ResourceT &resource) noexcept
		{
			//Optional to override
		}

		///@brief Called right after a resource has been loaded, with a reference to the resource
		virtual void ResourceLoaded([[maybe_unused]] ResourceT &resource) noexcept
		{
			//Optional to override
		}

		///@brief Called right after a resource has been unloaded, with a reference to the resource
		virtual void ResourceUnloaded([[maybe_unused]] ResourceT &resource) noexcept
		{
			//Optional to override
		}

		///@brief Called right after a resource has failed preparing/loading, with a reference to the resource
		virtual void ResourceFailed([[maybe_unused]] ResourceT &resource) noexcept
		{
			//Optional to override
		}


		///@brief Called right after a resource has its loading state changed, with a reference to the resource
		virtual void ResourceLoadingStateChanged([[maybe_unused]] ResourceT &resource) noexcept
		{
			//Optional to override
		}

		///@}
	};
} //ion::events::listeners

#endif