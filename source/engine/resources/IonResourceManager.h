/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	resources
File:	IonResourceManager.h
-------------------------------------------
*/

#ifndef ION_RESOURCE_MANAGER_H
#define ION_RESOURCE_MANAGER_H

#include "IonResource.h"
#include "events/IonListenable.h"
#include "events/listeners/IonResourceListener.h"
#include "managed/IonObjectManager.h"

namespace ion::resources
{
	template <typename ResourceT, typename OwnerT>
	class ResourceManager : public managed::ObjectManager<ResourceT, OwnerT, events::listeners::ResourceListener<ResourceT, OwnerT>>
	{
		public:

			//Default constructor
			ResourceManager() = default;

			//Deleted copy constructor
			ResourceManager(const ResourceManager&) = delete;

			//Default move constructor
			ResourceManager(ResourceManager&&) = default;


			/*
				Operators
			*/

			//Deleted copy assignment
			ResourceManager& operator=(const ResourceManager&) = delete;

			//Move assignment
			ResourceManager& operator=(ResourceManager&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all resources in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Resources() noexcept
			{
				return this->Objects();
			}

			//Returns an immutable range of all resources in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Resources() const noexcept
			{
				return this->Objects();
			}
	};
} //ion::resources

#endif