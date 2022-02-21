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
		enum class PreferredBoundingVolumeType : bool
		{
			BoundingBox,
			BoundingSphere
		};

		using ShaderPrograms = std::vector<shaders::ShaderProgram*>;


		namespace detail
		{
		} //detail
	} //movable_object


	//A movable object with bounding volumes, that can be attached to a scene node
	class MovableObject : public managed::ManagedObject<SceneManager>
	{
		protected:

			uint32 query_type_flags_ = 0;
			bool visible_ = true;

			Aabb aabb_;
			Obb obb_;
			Sphere sphere_;

			mutable movable_object::ShaderPrograms shader_programs_;


			/*
				Bounding volumes
			*/

			Aabb DeriveWorldAxisAlignedBoundingBox(Aabb aabb, bool apply_extent = true) const noexcept;
			Obb DeriveWorldOrientedBoundingBox(Obb obb, Aabb aabb, bool apply_extent = true) const noexcept;
			Sphere DeriveWorldBoundingSphere(Sphere sphere, Aabb aabb, bool apply_extent = true) const noexcept;

			void DrawBoundingVolumes(const Aabb &aabb, const Obb &obb, const Sphere &sphere,
				const Color &aabb_color, const Color &obb_color, const Color &sphere_color) const noexcept;

		private:

			Aabb bounding_volume_extent_ = {vector2::Zero, vector2::UnitScale};
			movable_object::PreferredBoundingVolumeType preferred_bounding_volume_ =
				movable_object::PreferredBoundingVolumeType::BoundingBox;
			std::optional<uint32> query_flags_;
			std::optional<uint32> query_mask_;

			bool show_bounding_volumes_ = false;
			Color aabb_color_ = color::White;
			Color obb_color_ = color::White;
			Color sphere_color_ = color::White;

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

			//Sets the relative bounding volume extent to the given extent
			//Aabb::Min represents the bottom-left corner (default: vector2::Zero)
			//Aabb::Max represents the top-left corner (default: vector2::UnitScale)
			inline void BoundingVolumeExtent(const Aabb &extent) noexcept
			{
				bounding_volume_extent_ = extent;
			}

			//Sets the preferred bounding volume for this movable object to the given type
			//Is used for choosing bounding volume when querying
			inline void PreferredBoundingVolume(movable_object::PreferredBoundingVolumeType type) noexcept
			{
				preferred_bounding_volume_ = type;
			}


			//Sets the query flags for this movable object to the given flags
			//This object will only be queried if a bitwise AND operation between the query flags and the scene query mask is non-zero
			//The meaning of the bits is user-specific
			inline void QueryFlags(std::optional<uint32> flags) noexcept
			{
				query_flags_ = flags;
			}

			//Adds the given flags to the already existing query flags for this movable object
			//This object will only be queried if a bitwise AND operation between the query flags and the scene query mask is non-zero
			//The meaning of the bits is user-specific
			inline void AddQueryFlags(uint32 flags) noexcept
			{
				if (query_flags_)
					*query_flags_ |= flags;
				else
					query_flags_ = flags;
			}

			//Removes the given flags to the already existing query flags for this movable object
			//This object will only be queried if a bitwise AND operation between the query flags and the scene query mask is non-zero
			//The meaning of the bits is user-specific
			inline void RemoveQueryFlags(uint32 flags) noexcept
			{
				if (query_flags_)
					*query_flags_ &= ~flags;
			}


			//Sets the query mask for this movable object to the given mask
			//This object can only intersect another object if a bitwise AND operation between the query flags and the other objects query mask is non-zero
			//The meaning of the bits is user-specific
			inline void QueryMask(std::optional<uint32> mask) noexcept
			{
				query_mask_ = mask;
			}

			//Adds the given mask to the already existing query mask for this movable object
			//This object can only intersect another object if a bitwise AND operation between the query flags and the other objects query mask is non-zero
			//The meaning of the bits is user-specific
			inline void AddQueryMask(uint32 mask) noexcept
			{
				if (query_mask_)
					*query_mask_ |= mask;
				else
					query_mask_ = mask;
			}

			//Removes the given mask to the already existing query mask for this movable object
			//This object can only intersect another object if a bitwise AND operation between the query flags and the other objects query mask is non-zero
			//The meaning of the bits is user-specific
			inline void RemoveQueryMask(uint32 mask) noexcept
			{
				if (query_mask_)
					*query_mask_ &= ~mask;
			}


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

			//Returns the relative bounding volume extent of this movable object
			//Aabb::Min represents the bottom-left corner (default: vector2::Zero)
			//Aabb::Max represents the top-left corner (default: vector2::UnitScale)
			[[nodiscard]] inline auto BoundingVolumeExtent() const noexcept
			{
				return bounding_volume_extent_;
			}

			//Returns the preferred bounding volume for this movable object
			//Is used for choosing bounding volume when querying
			[[nodiscard]] inline auto PreferredBoundingVolume() const noexcept
			{
				return preferred_bounding_volume_;
			}

			//Returns the query flags for this movable object
			//This object will only be queried if a bitwise AND operation between the query flags and the scene query mask is non-zero
			//The meaning of the bits is user-specific
			[[nodiscard]] inline auto QueryFlags() const noexcept
			{
				return query_flags_;
			}

			//Returns the query mask for this movable object
			//This object can only intersect another object if a bitwise AND operation between the query flags and the other objects query mask is non-zero
			//The meaning of the bits is user-specific
			[[nodiscard]] inline auto QueryMask() const noexcept
			{
				return query_mask_;
			}

			//Returns the query type flags for this movable object
			//This object will only be queried if a bitwise AND operation between the query type flags and the scene query type mask is non-zero
			[[nodiscard]] inline auto QueryTypeFlags() const noexcept
			{
				return query_type_flags_;
			}


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


			//Returns the world axis-aligned bounding box (AABB) for this movable object
			[[nodiscard]] inline auto& WorldAxisAlignedBoundingBox(bool derive = true, bool apply_extent = true) const noexcept
			{
				if (derive)
					world_aabb_ = DeriveWorldAxisAlignedBoundingBox(aabb_, apply_extent);

				return world_aabb_;
			}

			//Returns the world oriented bounding box (OBB) for this movable object
			[[nodiscard]] inline auto& WorldOrientedBoundingBox(bool derive = true, bool apply_extent = true) const noexcept
			{
				if (derive)
					world_obb_ = DeriveWorldOrientedBoundingBox(obb_, aabb_, apply_extent);

				return world_obb_;
			}

			//Returns the world bounding sphere for this movable object
			[[nodiscard]] inline auto& WorldBoundingSphere(bool derive = true, bool apply_extent = true) const noexcept
			{
				if (derive)
					world_sphere_ = DeriveWorldBoundingSphere(sphere_, aabb_, apply_extent);

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