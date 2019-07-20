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

#ifndef _ION_RESOURCE_
#define _ION_RESOURCE_

namespace ion::resources
{

template <typename T>
class Resource
{
	protected:
	
		T *owner_ = nullptr;

	public:

		//Default constructor
		Resource() = default;

		//Copy constructor
		Resource(const Resource&) noexcept :
			owner_{nullptr} //A copy of a resource has no owner
		{
			//Empty
		}

		//Copy assignment
		inline auto& operator=(const Resource &rhs) noexcept
		{
			owner_ = nullptr;
			return *this;
		}


		/*
			Modifiers
		*/

		//Set ownership of this resource to the given owner
		inline void Owner(T &owner) noexcept
		{
			owner_ = &owner;
		}

		//Release ownership for this resource
		inline void Release() noexcept
		{
			owner_ = nullptr;
		}


		/*
			Observers
		*/

		//Returns a mutable owner for this resource
		[[nodiscard]] inline auto* Owner() noexcept
		{
			return owner_;
		}

		//Returns an immutable owner for this resource
		[[nodiscard]] inline const auto* Owner() const noexcept
		{
			return owner_;
		}
};

} //ion::resources

#endif