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

#include <optional>
#include <string>

namespace ion::managed
{
	///@brief A class representing a managed object with an optional name and owner (usually an object manager)
	///@details Manually created objects or orphaned objects are ownerless, meaning their lifetimes are managed by the user
	template <typename T>
	class ManagedObject
	{
		public:

			using owner_type = T;

		protected:
		
			std::optional<std::string> name_;
			T *owner_ = nullptr;

		public:

			///@brief Default constructor
			ManagedObject() = default;

			///@brief Constructs a new managed object with the given name
			explicit ManagedObject(std::string name) noexcept :
				name_{std::move(name)}
			{
				//Empty
			}

			///@brief Constructs a new managed object with the given name
			explicit ManagedObject(std::optional<std::string> name) noexcept :
				name_{std::move(name)}
			{
				//Empty
			}

			///@brief Copy constructor
			ManagedObject(const ManagedObject &rhs) :
				name_{rhs.name_}, owner_{nullptr} //A copy of a managed object has no owner
			{
				//Empty
			}


			/*
				@name Operators
				@{
			*/

			///@brief Copy assignment
			inline auto& operator=(const ManagedObject &rhs)
			{
				if (this != &rhs)
				{
					name_ = rhs.name_;
					owner_ = nullptr;
				}

				return *this;
			}

			///@}

			/*
				@name Modifiers
				@{
			*/

			///@brief Sets ownership of this managed object to the given owner
			inline void Owner(T &owner) noexcept
			{
				owner_ = &owner;
			}

			///@brief Releases ownership for this managed object
			inline void Owner(std::nullptr_t) noexcept
			{
				owner_ = nullptr;
			}

			///@}

			/*
				@name Observers
				@{
			*/

			///@brief Returns the name of this managed object
			///@details A name must be unique among named objects with the same owner.
			///Returns nullopt if this object has no name
			[[nodiscard]] inline auto& Name() const noexcept
			{
				return name_;
			}

			///@brief Returns a pointer to the owner of this managed object
			[[nodiscard]] inline auto Owner() const noexcept
			{
				return owner_;
			}

			///@}
	};
} //ion::managed

#endif