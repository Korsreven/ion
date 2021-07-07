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
#include <optional>
#include <tuple>
#include <vector>

#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonSphere.h"
#include "managed/IonManagedObject.h"

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

	using namespace utilities;


	namespace movable_object
	{
		using ShaderPrograms = std::vector<shaders::ShaderProgram*>;

		namespace detail
		{
		} //detail
	} //movable_object


	//A movable object with bounding volumes, that can be attached to a scene node
	class MovableObject : public managed::ManagedObject<SceneManager>
	{
		protected:

			bool visible_ = true;

			Aabb aabb_;
			Obb obb_;
			Sphere sphere_;

			mutable movable_object::ShaderPrograms shader_programs_;


			/*
				Bounding volumes
			*/

			Aabb DeriveWorldAxisAlignedBoundingBox(Aabb aabb) const noexcept;
			Obb DeriveWorldOrientedBoundingBox(Obb obb, Aabb aabb) const noexcept;
			Sphere DeriveWorldBoundingSphere(Sphere sphere, Aabb aabb) const noexcept;

			void DrawBoundingVolumes(const Aabb &aabb, const Obb &obb, const Sphere &sphere,
				const Color &aabb_color, const Color &obb_color, const Color &sphere_color) const noexcept;

		private:

			bool show_bounding_volumes_ = false;
			Color aabb_color_ = color::White;
			Color obb_color_ = color::White;
			Color sphere_color_ = color::White;
			Aabb bounding_volume_extent_ = {vector2::Zero, vector2::UnitScale};

			graph::SceneNode *parent_node_ = nullptr;
			std::any user_data_;

			mutable Aabb world_aabb_;
			mutable Obb world_obb_;
			mutable Sphere world_sphere_;


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

			//Sets whether or not to show this movable objects bounding volumes
			inline void ShowBoundingVolumes(bool show) noexcept
			{
				show_bounding_volumes_ = show;
			}

			//Sets the colors of the bounding volumes to the given colors
			//Use color::Transparent to hide certain bounding volumes from showing
			inline void BoundingVolumeColors(const Color &aabb_color, const Color &obb_color, const Color &sphere_color) noexcept
			{
				aabb_color_ = aabb_color;
				obb_color_ = obb_color;
				sphere_color_ = sphere_color;
			}

			//Sets the relative bounding volume extent to the given extent
			//Aabb::Min represents the bottom-left corner (default: vector2::Zero)
			//Aabb::Max represents the top-left corner (default: vector2::UnitScale)
			inline void BoundingVolumeExtent(const Aabb &extent) noexcept
			{
				bounding_volume_extent_ = extent;
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

			//Returns true if this movable objects bounding volumes are shown
			[[nodiscard]] inline auto ShowBoundingVolumes() const noexcept
			{
				return show_bounding_volumes_;
			}

			//Returns the bounding volume colors for this movable object
			[[nodiscard]] inline auto BoundingVolumeColors() const noexcept
			{
				return std::tuple{aabb_color_, obb_color_, sphere_color_};
			}

			//Returns the relative bounding volume extent of this movable object
			//Aabb::Min represents the bottom-left corner (default: vector2::Zero)
			//Aabb::Max represents the top-left corner (default: vector2::UnitScale)
			[[nodiscard]] inline auto BoundingVolumeExtent() const noexcept
			{
				return bounding_volume_extent_;
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
			[[nodiscard]] inline auto& WorldAxisAlignedBoundingBox(bool derive = true) const noexcept
			{
				if (derive)
					world_aabb_ = DeriveWorldAxisAlignedBoundingBox(aabb_);

				return world_aabb_;
			}

			//Returns the world oriented bounding box (OBB) for this movable object
			[[nodiscard]] inline auto& WorldOrientedBoundingBox(bool derive = true) const noexcept
			{
				if (derive)
					world_obb_ = DeriveWorldOrientedBoundingBox(obb_, aabb_);

				return world_obb_;
			}

			//Returns the world bounding sphere for this movable object
			[[nodiscard]] inline auto& WorldBoundingSphere(bool derive = true) const noexcept
			{
				if (derive)
					world_sphere_ = DeriveWorldBoundingSphere(sphere_, aabb_);

				return world_sphere_;
			}


			/*
				Rendering
			*/

			//Render this movable object
			//This is called once from a scene graph render queue
			virtual void Render() noexcept;

			//Returns all (distinct) shader programs used to render this movable object
			[[nodiscard]] virtual const movable_object::ShaderPrograms& RenderPrograms(bool derive = true) const;


			/*
				Elapse time
			*/

			//Elapse the total time for this movable object by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			virtual void Elapse(duration time) noexcept;
	};
} //ion::graphics::scene

#endif