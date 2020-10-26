/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	managed
File:	IonManagedObject.h
-------------------------------------------
*/

#ifndef ION_MANAGED_OBJECT_H
#define ION_MANAGED_OBJECT_H

#include <string>

namespace ion::managed
{
	template <typename T>
	class ManagedObject
	{
		public:

			using owner_type = T;

		protected:
		
			std::string name_;
			T *owner_ = nullptr;

		public:

			//Constructor
			explicit ManagedObject(std::string name) :
				name_{std::move(name)}
			{
				//Empty
			}

			//Copy constructor
			ManagedObject(const ManagedObject &rhs) :
				name_{rhs.name_}, owner_{nullptr} //A copy of a managed object has no owner
			{
				//Empty
			}

			//Copy assignment
			inline auto& operator=(const ManagedObject &rhs)
			{
				name_ = rhs.name_;
				owner_ = nullptr;
				return *this;
			}


			/*
				Modifiers
			*/

			//Set ownership of this managed object to the given owner
			inline void Owner(T &owner) noexcept
			{
				owner_ = &owner;
			}

			//Release ownership for this managed object
			inline void Release() noexcept
			{
				owner_ = nullptr;
			}


			/*
				Observers
			*/

			//Returns the name of this managed object
			//A name must be unique among objects with the same owner
			[[nodiscard]] inline auto& Name() const noexcept
			{
				return name_;
			}

			//Returns a mutable owner for this managed object
			[[nodiscard]] inline auto Owner() noexcept
			{
				return owner_;
			}

			//Returns an immutable owner for this managed object
			[[nodiscard]] inline const auto Owner() const noexcept
			{
				return owner_;
			}
	};
} //ion::managed

#endif