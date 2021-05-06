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

#include <any>

#include "managed/IonManagedObject.h"

namespace ion::graphics::scene
{
	class SceneManager; //Forward declaration

	namespace graph
	{
		class SceneNode; //Forward declaration
	}

	namespace movable_object::detail
	{
	} //movable_object::detail


	//A movable object that can be attached to a scene node
	class MovableObject : public managed::ManagedObject<SceneManager>
	{
		protected:

			bool visible_ = true;

		private:

			graph::SceneNode *parent_node_ = nullptr;
			std::any user_data_;


			/*
				Helper functions
			*/

			void Detach();

		public:

			//Default constructor
			MovableObject() = default;

			//Construct a movable object with the given visibility
			explicit MovableObject(bool visible);

			//Construct a movable object with the given name and visibility
			explicit MovableObject(std::string name, bool visible = true);

			//Copy constructor
			MovableObject(const MovableObject &rhs) noexcept;

			//Virtual destructor
			virtual ~MovableObject() noexcept;


			/*
				Operators
			*/

			//Copy assignment
			inline auto& operator=(const MovableObject &rhs) noexcept
			{
				managed::ManagedObject<SceneManager>::operator=(rhs);
				Detach();
				return *this;
			}


			/*
				Modifiers
			*/

			//Sets the visibility of this movable object to the given value
			inline void Visible(bool visible) noexcept
			{
				visible_ = visible;
			}


			//Sets parent node of this movable object to the given node
			inline void ParentNode(graph::SceneNode *scene_node) noexcept
			{
				Detach();
				parent_node_ = scene_node;
			}

			//Sets the custom user data for this movable object to the given data
			inline void UserData(std::any data) noexcept
			{
				user_data_ = data;
			}


			/*
				Observers
			*/

			//Returns true if this model is visible
			[[nodiscard]] inline auto Visible() const noexcept
			{
				return visible_;
			}


			//Returns a pointer to the parent node for this movable object
			[[nodiscard]] inline auto ParentNode() const noexcept
			{
				return parent_node_;
			}

			//Returns the custom user data for this movable object
			[[nodiscard]] inline auto& UserData() const noexcept
			{
				return user_data_;
			}
	};
} //ion::graphics::scene

#endif