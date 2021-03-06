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

		protected:

			bool visible_ = true;

		public:

			//Default constructor
			MovableObject() = default;

			//Construct a movable object with the given visibility
			explicit MovableObject(bool visible) :
				visible_{visible}
			{
				//Empty
			}

			//Construct a movable object with the given name and visibility
			explicit MovableObject(std::string name, bool visible = true) :
				managed::ManagedObject<SceneManager>{std::move(name)},
				visible_{visible}
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

			//Sets the visibility of this movable object to the given value
			inline void Visible(bool visible) noexcept
			{
				visible_ = visible;
			}


			/*
				Observers
			*/

			//Returns a pointer to the parent node for this movable object
			[[nodiscard]] inline auto ParentNode() const noexcept
			{
				return parent_node_;
			}

			//Returns true if this model is visible
			[[nodiscard]] inline auto Visible() const noexcept
			{
				return visible_;
			}
	};
} //ion::graphics::scene

#endif