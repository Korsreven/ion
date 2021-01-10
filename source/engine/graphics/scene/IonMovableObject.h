/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableObject.h
-------------------------------------------
*/

#ifndef ION_MOVABLE_OBJECT_H
#define ION_MOVABLE_OBJECT_H

#include "managed/IonManagedObject.h"

namespace ion::graphics::scene
{
	class SceneManager; //Forward declaration
	class SceneNode; //Forward declaration

	namespace movable_object
	{
		namespace detail
		{
		} //detail
	} //movable_object


	//A movable object that can be attached to a scene node
	class MovableObject : public managed::ManagedObject<SceneManager>
	{
		private:

			SceneNode *parent_node_ = nullptr;

		public:

			//Default constructor
			MovableObject() = default;

			//Construct a movable object with the given name
			explicit MovableObject(std::string name) :
				managed::ManagedObject<SceneManager>{std::move(name)}
			{
				//Empty
			}

			//Copy constructor
			MovableObject(const MovableObject &rhs) noexcept :

				managed::ManagedObject<SceneManager>{rhs},
				parent_node_{nullptr} //A copy of a movable object has no parent node
			{
				//Empty
			}

			//Default virtual destructor
			virtual ~MovableObject() = default;


			/*
				Operators
			*/

			//Copy assignment
			inline auto& operator=(const MovableObject &rhs) noexcept
			{
				managed::ManagedObject<SceneManager>::operator=(rhs);
				parent_node_ = nullptr;
				return *this;
			}


			/*
				Modifiers
			*/

			//Sets parent node of this movable object to the given node
			inline void ParentNode(SceneNode *node) noexcept
			{
				parent_node_ = node;
			}


			/*
				Observers
			*/

			//Returns a pointer to a mutable parent node for this movable object
			[[nodiscard]] inline auto ParentNode() noexcept
			{
				return parent_node_;
			}

			//Returns a pointer to an immutable parent node for this movable object
			[[nodiscard]] inline const auto ParentNode() const noexcept
			{
				return parent_node_;
			}
	};
} //ion::graphics::scene

#endif