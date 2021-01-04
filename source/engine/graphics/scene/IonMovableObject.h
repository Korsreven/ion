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

namespace ion::graphics::scene
{
	class SceneNode; //Forward declaration

	namespace movable_object
	{
		namespace detail
		{
		} //detail
	} //movable_object


	//A movable object can be attached to a scene node
	class MovableObject
	{
		private:

			SceneNode *parent_node_ = nullptr;

		public:

			//Default constructor
			MovableObject() = default;

			//Copy constructor
			MovableObject(const MovableObject&) :
				parent_node_{nullptr} //A copy of a movable object has no parent node
			{
				//Empty
			}

			//Copy assignment
			inline auto& operator=(const MovableObject&)
			{
				parent_node_ = nullptr;
				return *this;
			}


			/*
				Modifiers
			*/

			//Sets parent node of this movable object to the given scene node
			inline void ParentNode(SceneNode &parent) noexcept
			{
				parent_node_ = &parent;
			}

			//Releases parent node of this movable object
			inline void ParentNode(std::nullptr_t) noexcept
			{
				parent_node_ = nullptr;
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