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

namespace ion::managed
{
	template <typename T>
	class ManagedObject
	{
		public:

			using owner_type = T;

		protected:
	
			T *owner_ = nullptr;

		public:

			//Default constructor
			ManagedObject() = default;

			//Copy constructor
			ManagedObject(const ManagedObject&) noexcept :
				owner_{nullptr} //A copy of a managed object has no owner
			{
				//Empty
			}

			//Copy assignment
			inline auto& operator=(const ManagedObject &rhs) noexcept
			{
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