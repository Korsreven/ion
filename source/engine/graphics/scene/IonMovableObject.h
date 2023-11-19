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
#include <span>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonSphere.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonSuppressMove.h"

//Forward declarations
namespace ion::graphics
{
	namespace render
	{
		class Renderer;
		class RenderPrimitive;
	}

	namespace shaders
	{
		class ShaderProgram;
	}
}

namespace ion::graphics::scene
{
	//Forward declarations
	class Light;
	class SceneManager;

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

		using RenderPrimitiveRange = std::span<render::RenderPrimitive*>;
		using ShaderProgramRange = std::span<NonOwningPtr<shaders::ShaderProgram>>;
		using LightRange = std::span<Light*>;


		namespace detail
		{
		} //detail
	} //movable_object


	///@brief A class representing a movable object with bounding volumes, that can be attached to a scene node
	class MovableObject : public managed::ManagedObject<SceneManager>
	{
		protected:

			uint32 query_type_flags_ = 0;
			bool visible_ = true;

			Aabb aabb_;
			Obb obb_;
			Sphere sphere_;


			render::Renderer* ParentRenderer() const noexcept;


			/**
				@name Bounding volumes
				@{
			*/

			Aabb DeriveWorldAxisAlignedBoundingBox(Aabb aabb, bool apply_extent = true) const noexcept;
			Obb DeriveWorldOrientedBoundingBox(Obb obb, Aabb aabb, bool apply_extent = true) const noexcept;
			Sphere DeriveWorldBoundingSphere(Sphere sphere, Aabb aabb, bool apply_extent = true) const noexcept;

			void DrawBoundingVolumes(const Aabb &aabb, const Obb &obb, const Sphere &sphere,
				const Color &aabb_color, const Color &obb_color, const Color &sphere_color, real z) const noexcept;

			///@}

		private:

			std::optional<std::string> alias_;
			std::optional<std::string> tag_;

			Aabb bounding_volume_extent_ = {vector2::Zero, vector2::UnitScale};
			movable_object::PreferredBoundingVolumeType preferred_bounding_volume_ =
				movable_object::PreferredBoundingVolumeType::BoundingBox;
			std::optional<uint32> query_flags_;
			std::optional<uint32> query_mask_;

			bool show_bounding_volumes_ = false;
			Color aabb_color_ = color::White;
			Color obb_color_ = color::White;
			Color sphere_color_ = color::White;

			types::SuppressMove<graph::SceneNode*> parent_node_ = nullptr;
			std::any user_data_;

			mutable std::pair<Aabb, Aabb> world_aabb_;
			mutable std::pair<Obb, Obb> world_obb_;
			mutable std::pair<Sphere, Sphere> world_sphere_;


			/**
				@name Helper functions
				@{
			*/

			void Detach() noexcept;

			///@}

		public:

			///@brief Constructs a movable object with the given name and visibility
			explicit MovableObject(std::optional<std::string> name = {}, bool visible = true) noexcept;		

			///@brief Default copy constructor
			MovableObject(const MovableObject&) = default;

			///@brief Default move constructor
			MovableObject(MovableObject&&) = default;

			///@brief Virtual destructor
			virtual ~MovableObject() noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Default copy assignment
			inline MovableObject& operator=(const MovableObject&) = default;

			///@brief Default move assignment
			inline MovableObject& operator=(MovableObject&&) = default;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the alias of this movable object to the given alias
			///@details An alias does not need to be unique like a name.
			///Alias could be used to query multiple objects or objects without a name
			inline void Alias(std::optional<std::string> alias) noexcept
			{
				alias_ = std::move(alias);
			}

			///@brief Sets the tag of this movable object to the given tag
			///@details Tag could be used to categorize objects
			inline void Tag(std::optional<std::string> tag) noexcept
			{
				tag_ = std::move(tag);
			}


			///@brief Sets the relative bounding volume extent to the given extent
			///@details Aabb::Min represents the bottom-left corner (default: vector2::Zero).
			///Aabb::Max represents the top-left corner (default: vector2::UnitScale)
			inline void BoundingVolumeExtent(const Aabb &extent) noexcept
			{
				bounding_volume_extent_ = extent;
			}

			///@brief Sets the preferred bounding volume for this movable object to the given type
			///@details Is used for choosing bounding volume when querying
			inline void PreferredBoundingVolume(movable_object::PreferredBoundingVolumeType type) noexcept
			{
				preferred_bounding_volume_ = type;
			}


			///@brief Sets the query flags for this movable object to the given flags
			///@details This object will only be queried if a bitwise AND operation between the query flags and the scene query mask is non-zero.
			///The meaning of the bits is user-specific
			inline void QueryFlags(std::optional<uint32> flags) noexcept
			{
				query_flags_ = flags;
			}

			///@brief Adds the given flags to the already existing query flags for this movable object
			///@details This object will only be queried if a bitwise AND operation between the query flags and the scene query mask is non-zero.
			///The meaning of the bits is user-specific
			inline void AddQueryFlags(uint32 flags) noexcept
			{
				if (query_flags_)
					*query_flags_ |= flags;
				else
					query_flags_ = flags;
			}

			///@brief Removes the given flags to the already existing query flags for this movable object
			///@details This object will only be queried if a bitwise AND operation between the query flags and the scene query mask is non-zero.
			///The meaning of the bits is user-specific
			inline void RemoveQueryFlags(uint32 flags) noexcept
			{
				if (query_flags_)
					*query_flags_ &= ~flags;
			}


			///@brief Sets the query mask for this movable object to the given mask
			///@details This object can only intersect another object if a bitwise AND operation between the query flags and the other objects query mask is non-zero.
			///The meaning of the bits is user-specific
			inline void QueryMask(std::optional<uint32> mask) noexcept
			{
				query_mask_ = mask;
			}

			///@brief Adds the given mask to the already existing query mask for this movable object
			///@details This object can only intersect another object if a bitwise AND operation between the query flags and the other objects query mask is non-zero.
			///The meaning of the bits is user-specific
			inline void AddQueryMask(uint32 mask) noexcept
			{
				if (query_mask_)
					*query_mask_ |= mask;
				else
					query_mask_ = mask;
			}

			///@brief Removes the given mask to the already existing query mask for this movable object
			///@details This object can only intersect another object if a bitwise AND operation between the query flags and the other objects query mask is non-zero.
			///The meaning of the bits is user-specific
			inline void RemoveQueryMask(uint32 mask) noexcept
			{
				if (query_mask_)
					*query_mask_ &= ~mask;
			}


			///@brief Sets the visibility of this movable object to the given value
			inline void Visible(bool visible) noexcept
			{
				visible_ = visible;
			}

			///@brief Sets whether or not to show this movable objects bounding volumes
			inline void ShowBoundingVolumes(bool show) noexcept
			{
				show_bounding_volumes_ = show;
			}

			///@brief Sets the colors of the bounding volumes to the given colors
			inline void BoundingVolumeColors(const Color &color) noexcept
			{
				aabb_color_ = color;
				obb_color_ = color;
				sphere_color_ = color;
			}

			///@brief Sets the colors of each of the bounding volumes to the given colors
			///@details Use color::Transparent to hide certain bounding volumes from showing
			inline void BoundingVolumeColors(const Color &aabb_color, const Color &obb_color, const Color &sphere_color) noexcept
			{
				aabb_color_ = aabb_color;
				obb_color_ = obb_color;
				sphere_color_ = sphere_color;
			}


			///@brief Sets parent node of this movable object to the given node
			inline void ParentNode(graph::SceneNode *scene_node) noexcept
			{
				parent_node_ = scene_node;
			}

			///@brief Sets the custom user data for this movable object to the given data
			inline void UserData(std::any data) noexcept
			{
				user_data_ = data;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the alias of this movable object
			///@details Returns nullopt if this object has no alias
			[[nodiscard]] inline auto& Alias() const noexcept
			{
				return alias_;
			}

			///@brief Returns the tag of this movable object
			///@details Returns nullopt if this object has no tag
			[[nodiscard]] inline auto& Tag() const noexcept
			{
				return tag_;
			}


			///@brief Returns the relative bounding volume extent of this movable object
			///@details Aabb::Min represents the bottom-left corner (default: vector2::Zero).
			///Aabb::Max represents the top-left corner (default: vector2::UnitScale)
			[[nodiscard]] inline auto BoundingVolumeExtent() const noexcept
			{
				return bounding_volume_extent_;
			}

			///@brief Returns the preferred bounding volume for this movable object
			///@details Is used for choosing bounding volume when querying
			[[nodiscard]] inline auto PreferredBoundingVolume() const noexcept
			{
				return preferred_bounding_volume_;
			}

			///@brief Returns the query flags for this movable object
			///@details This object will only be queried if a bitwise AND operation between the query flags and the scene query mask is non-zero.
			///The meaning of the bits is user-specific
			[[nodiscard]] inline auto QueryFlags() const noexcept
			{
				return query_flags_;
			}

			///@brief Returns the query mask for this movable object
			///@details This object can only intersect another object if a bitwise AND operation between the query flags and the other objects query mask is non-zero.
			///The meaning of the bits is user-specific
			[[nodiscard]] inline auto QueryMask() const noexcept
			{
				return query_mask_;
			}

			///@brief Returns the query type flags for this movable object
			///@details This object will only be queried if a bitwise AND operation between the query type flags and the scene query type mask is non-zero
			[[nodiscard]] inline auto QueryTypeFlags() const noexcept
			{
				return query_type_flags_;
			}


			///@brief Returns true if this movable object is visible
			[[nodiscard]] inline auto Visible() const noexcept
			{
				return visible_;
			}

			///@brief Returns true if this movable objects bounding volumes are shown
			[[nodiscard]] inline auto ShowBoundingVolumes() const noexcept
			{
				return show_bounding_volumes_;
			}

			///@brief Returns the bounding volume colors for this movable object
			[[nodiscard]] inline auto BoundingVolumeColors() const noexcept
			{
				return std::tuple{aabb_color_, obb_color_, sphere_color_};
			}


			///@brief Returns a pointer to the parent node for this movable object
			[[nodiscard]] inline auto ParentNode() const noexcept
			{
				return parent_node_.Get();
			}

			///@brief Returns the custom user data for this movable object
			[[nodiscard]] inline auto& UserData() const noexcept
			{
				return user_data_;
			}


			///@brief Returns the local axis-aligned bounding box (AABB) for this movable object
			[[nodiscard]] inline auto& AxisAlignedBoundingBox() const noexcept
			{
				return aabb_;
			}

			///@brief Returns the local oriented bounding box (OBB) for this movable object
			[[nodiscard]] inline auto& OrientedBoundingBox() const noexcept
			{
				return obb_;
			}

			///@brief Returns the local bounding sphere for this movable object
			[[nodiscard]] inline auto& BoundingSphere() const noexcept
			{
				return sphere_;
			}


			///@brief Returns the world axis-aligned bounding box (AABB) for this movable object
			[[nodiscard]] inline auto& WorldAxisAlignedBoundingBox(bool derive = true, bool apply_extent = true) const noexcept
			{
				if (derive)
					apply_extent ?
					(world_aabb_.first = DeriveWorldAxisAlignedBoundingBox(aabb_, apply_extent)) :
					(world_aabb_.second = DeriveWorldAxisAlignedBoundingBox(aabb_, apply_extent));

				return apply_extent ? world_aabb_.first : world_aabb_.second;
			}

			///@brief Returns the world oriented bounding box (OBB) for this movable object
			[[nodiscard]] inline auto& WorldOrientedBoundingBox(bool derive = true, bool apply_extent = true) const noexcept
			{
				if (derive)
					apply_extent ?
					(world_obb_.first = DeriveWorldOrientedBoundingBox(obb_, aabb_, apply_extent)) :
					(world_obb_.second = DeriveWorldOrientedBoundingBox(obb_, aabb_, apply_extent));

				return apply_extent ? world_obb_.first : world_obb_.second;
			}

			///@brief Returns the world bounding sphere for this movable object
			[[nodiscard]] inline auto& WorldBoundingSphere(bool derive = true, bool apply_extent = true) const noexcept
			{
				if (derive)
					apply_extent ?
					(world_sphere_.first = DeriveWorldBoundingSphere(sphere_, aabb_, apply_extent)) :
					(world_sphere_.second = DeriveWorldBoundingSphere(sphere_, aabb_, apply_extent));

				return apply_extent ? world_sphere_.first : world_sphere_.second;
			}


			///@brief Returns all render primitives in this movable object
			[[nodiscard]] virtual movable_object::RenderPrimitiveRange AllRenderPrimitives() noexcept;

			///@brief Returns all (distinct) shader programs used to render this movable object
			[[nodiscard]] virtual movable_object::ShaderProgramRange AllShaderPrograms() noexcept;

			///@brief Returns all emissive lights in this movable object
			[[nodiscard]] virtual movable_object::LightRange AllEmissiveLights() noexcept;

			///@}

			/**
				@name Preparing
				@{
			*/

			///@brief Prepares this movable object
			///@details This function is typically called each frame
			virtual void Prepare();

			///@}

			/**
				@name Drawing
				@{
			*/

			///@brief Draws the bounding volumes of this movable object
			virtual void DrawBounds(real z) noexcept;

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses the total time for this movable object by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			virtual void Elapse(duration time) noexcept;

			///@}
	};
} //ion::graphics::scene

#endif