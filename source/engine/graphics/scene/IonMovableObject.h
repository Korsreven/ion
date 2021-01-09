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
	class SceneManager; //Forward declaration
	class SceneNode; //Forward declaration

	namespace movable_object
	{
		namespace detail
		{
		} //detail
	} //movable_object


	//A movable object that can be attached to a scene node
	class MovableObject
	{
		private:

			SceneManager *scene_manager_ = nullptr;
			SceneNode *parent_node_ = nullptr;

		public:

			//Default constructor
			MovableObject() = default;

			//Copy constructor
			MovableObject(const MovableObject&) :
				scene_manager_{nullptr}, //A copy of a movable object has no scene manager
				parent_node_{nullptr} //A copy of a movable object has no parent node
			{
				//Empty
			}

			//Copy assignment
			inline auto& operator=(const MovableObject&)
			{
				scene_manager_ = nullptr;
				parent_node_ = nullptr;
				return *this;
			}


			/*
				Modifiers
			*/

			//Sets the scene that contains this movable object to the given scene
			inline void Scene(SceneManager *scene) noexcept
			{
				scene_manager_ = scene;
			}

			//Sets parent node of this movable object to the given node
			inline void ParentNode(SceneNode *node) noexcept
			{
				parent_node_ = node;
			}


			/*
				Observers
			*/

			//Returns a pointer to a mutable scene that contains this movable object
			[[nodiscard]] inline auto Scene() noexcept
			{
				return scene_manager_;
			}

			//Returns a pointer to an immutable scene that contains this movable object
			[[nodiscard]] inline const auto Scene() const noexcept
			{
				return scene_manager_;
			}


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