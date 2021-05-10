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

#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonSphere.h"
#include "managed/IonManagedObject.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::graphics
{
	namespace shaders
	{
		class ShaderProgram;
	}
}

namespace ion::graphics::scene
{
	class SceneManager; //Forward declaration

	namespace graph
	{
		class SceneNode; //Forward declaration
	}

	using utilities::Aabb;
	using utilities::Obb;
	using utilities::Sphere;


	namespace movable_object::detail
	{
	} //movable_object::detail


	//A movable object with bounding volumes, that can be attached to a scene node
	class MovableObject : public managed::ManagedObject<SceneManager>
	{
		protected:

			bool visible_ = true;

			Aabb aabb_;
			Obb obb_;
			Sphere sphere_;
			
			mutable bool need_bounding_update_ = false;

		private:

			graph::SceneNode *parent_node_ = nullptr;
			std::any user_data_;

			mutable Aabb world_aabb_;
			mutable Obb world_obb_;
			mutable Sphere world_sphere_;


			/*
				Updating
			*/

			void UpdateBoundingVolumes() const noexcept;


			/*
				Helper functions
			*/

			void Detach() noexcept;

		public:

			//Construct a movable object with the given visibility
			explicit MovableObject(bool visible = true);

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

			//Returns true if this movable object is visible
			[[nodiscard]] inline auto Visible() const noexcept
			{
				return visible_;
			}


			//Returns the local axis-aligned bounding box (AABB) for this movable object
			[[nodiscard]] inline auto& AxisAlignedBoundingBox() const noexcept
			{
				return aabb_;
			}

			//Returns the local oriented bounding box (OBB) for this movable object
			[[nodiscard]] inline auto& OrientedBoundingBox() const noexcept
			{
				return obb_;
			}

			//Returns the local bounding sphere for this movable object
			[[nodiscard]] inline auto& BoundingSphere() const noexcept
			{
				return sphere_;
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


			//Returns the world axis-aligned bounding box (AABB) for this movable object
			[[nodiscard]] inline auto& WorldAxisAlignedBoundingBox() const noexcept
			{
				if (need_bounding_update_)
					UpdateBoundingVolumes();

				return world_aabb_;
			}

			//Returns the world oriented bounding box (OBB) for this movable object
			[[nodiscard]] inline auto& WorldOrientedBoundingBox() const noexcept
			{
				if (need_bounding_update_)
					UpdateBoundingVolumes();

				return world_obb_;
			}

			//Returns the world bounding sphere for this movable object
			[[nodiscard]] inline auto& WorldBoundingSphere() const noexcept
			{
				if (need_bounding_update_)
					UpdateBoundingVolumes();

				return world_sphere_;
			}


			/*
				Rendering
			*/

			//Render this movable object based on its defined passes
			//This is called once from a scene graph render queue, with the time in seconds since last frame
			//It will call elapse then prepare, and then call draw one time per pass
			virtual void Render(duration time) noexcept;


			/*
				Elapse time
			*/

			//Elapse the total time for this movable object by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			virtual void Elapse(duration time) noexcept;
	};
} //ion::graphics::scene

#endif