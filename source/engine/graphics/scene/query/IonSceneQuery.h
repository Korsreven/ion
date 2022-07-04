/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/query
File:	IonSceneQuery.h
-------------------------------------------
*/

#ifndef ION_SCENE_QUERY_H
#define ION_SCENE_QUERY_H

#include <algorithm>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

#include "graphics/scene/IonCamera.h"
#include "graphics/scene/IonLight.h"
#include "graphics/scene/IonMovableObject.h"
#include "graphics/scene/graph/IonSceneGraph.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/utilities/IonAabb.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::query
{
	using namespace graphics::scene::graph;
	using graphics::utilities::Aabb;

	namespace scene_query
	{
		struct QueryType
		{
			static constexpr uint32 Movable = 1 << 0;
			static constexpr uint32 Drawable = 1 << 1;

			static constexpr uint32 Camera = 1 << 2;
			static constexpr uint32 Light = 1 << 3;
			static constexpr uint32 Model = 1 << 4;
			static constexpr uint32 ParticleSystem = 1 << 5;
			static constexpr uint32 Sound = 1 << 6;
			static constexpr uint32 SoundListener = 1 << 7;
			static constexpr uint32 Text = 1 << 8;

			static constexpr uint32 UserType0 = 1 << 9;
				//First (pot) value not in use by the engine
		};


		namespace detail
		{
			using query_object = std::pair<MovableObject*, bool>;
			using query_objects = std::vector<query_object>;


			inline void get_eligible_objects(SceneNode &node, uint32 mask, uint32 type_mask, bool only_visible,
				query_objects &objects) noexcept
			{
				const auto default_flags = (mask == ~0_ui32 ? mask : 0_ui32);

				//Check if node is eligible
				if (!only_visible || node.Visible())
				{
					for (auto &attached_object : node.AttachedObjects())
					{
						auto object =
							std::visit([](auto &&x) noexcept -> MovableObject* { return x; }, attached_object);

						//Check if object is eligible
						if ((!only_visible || object->Visible()) &&
							object->QueryTypeFlags() & type_mask &&
							object->QueryFlags().value_or(default_flags) & mask)

							objects.emplace_back(object, true); //Eligible for querying
					}
				}

				for (auto &child_node : node.ChildNodes())
					get_eligible_objects(child_node, mask, type_mask, only_visible, objects); //Recursive
			}

			inline auto get_eligible_objects(SceneNode &node, uint32 mask, uint32 type_mask, bool only_visible) noexcept
			{
				query_objects objects;
				get_eligible_objects(node, mask, type_mask, only_visible, objects);
				return objects;
			}

			inline void derive_bounding_volumes(query_objects &objects) noexcept
			{
				for (auto &object : objects)
				{
					switch (object.first->PreferredBoundingVolume())
					{
						case movable_object::PreferredBoundingVolumeType::BoundingSphere:
						{
							[[maybe_unused]] auto &sphere = object.first->WorldBoundingSphere();
							break;
						}

						case movable_object::PreferredBoundingVolumeType::BoundingBox:
						default:
						{
							[[maybe_unused]] auto &aabb = object.first->WorldAxisAlignedBoundingBox();
							break;
						}
					}
				}
			}

			inline void remove_objects_outside_region(query_objects &objects, const Aabb &region) noexcept
			{
				objects.erase(
					std::remove_if(std::begin(objects), std::end(objects),
						[&](auto &object) noexcept
						{
							switch (object.first->PreferredBoundingVolume())
							{
								case movable_object::PreferredBoundingVolumeType::BoundingSphere:
								return !object.first->WorldBoundingSphere(false).Intersects(region);

								case movable_object::PreferredBoundingVolumeType::BoundingBox:
								default:
								return !object.first->WorldAxisAlignedBoundingBox(false).Intersects(region);
							}
						}),
					std::end(objects));
			}
		} //detail
	} //scene_query


	template <typename ResultT>
	class SceneQuery
	{
		protected:
		
			std::optional<uint32> query_mask_;
			std::optional<uint32> query_type_mask_ = scene_query::QueryType::Model;
			std::optional<Aabb> query_region_;
			bool only_visible_objects_ = true;
			NonOwningPtr<SceneGraph> scene_graph_;

		public:

			//Default constructor
			SceneQuery() = default;

			//Construct a new scene query with the given scene graph
			SceneQuery(NonOwningPtr<SceneGraph> scene_graph) noexcept :
				scene_graph_{scene_graph}
			{
				//Empty
			}


			/*
				Modifiers
			*/

			//Sets the query mask for this scene query to the given mask
			//This scene query will only query objects if a bitwise AND operation between the query mask and the object query flags is non-zero
			//The meaning of the bits is user-specific
			inline void QueryMask(std::optional<uint32> mask) noexcept
			{
				query_mask_ = mask;
			}

			//Adds the given mask to the already existing query mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query mask and the object query flags is non-zero
			//The meaning of the bits is user-specific
			inline void AddQueryMask(uint32 mask) noexcept
			{
				if (query_mask_)
					*query_mask_ |= mask;
				else
					query_mask_ = mask;
			}

			//Removes the given mask to the already existing query mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query mask and the object query flags is non-zero
			//The meaning of the bits is user-specific
			inline void RemoveQueryMask(uint32 mask) noexcept
			{
				if (query_mask_)
					*query_mask_ &= ~mask;
			}


			//Sets the query type mask for this scene query to the given mask
			//This scene query will only query objects if a bitwise AND operation between the query type mask and the object type is non-zero
			inline void QueryTypeMask(std::optional<uint32> mask) noexcept
			{
				query_type_mask_ = mask;
			}

			//Adds the given mask to the already existing query type mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query type mask and the object type is non-zero
			inline void AddQueryTypeMask(uint32 mask) noexcept
			{
				if (query_type_mask_)
					*query_type_mask_ |= mask;
				else
					query_type_mask_ = mask;
			}

			//Removes the given mask to the already existing query type mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query type mask and the object type is non-zero
			inline void RemoveQueryTypeMask(uint32 mask) noexcept
			{
				if (query_type_mask_)
					*query_type_mask_ &= ~mask;
			}


			//Sets the query region for this scene query to the given region
			//Pass nullopt to query the entire scene
			inline void QueryRegion(const Aabb &region) noexcept
			{
				query_region_ = region;
			}

			//Sets whether or not this scene query is only querying visible objects
			inline void OnlyVisibleObjects(bool only_visible) noexcept
			{
				only_visible_objects_ = only_visible;
			}

			//Sets the scene graph this scene query is querying
			inline void Scene(NonOwningPtr<SceneGraph> scene_graph) noexcept
			{
				scene_graph_ = scene_graph;
			}


			/*
				Observers
			*/

			//Returns the query mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query mask and the query flags is non-zero
			//The meaning of the bits is user-specific
			[[nodiscard]] inline auto QueryMask() const noexcept
			{
				return query_mask_;
			}

			//Returns the query type mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query type mask and the object type is non-zero
			[[nodiscard]] inline auto QueryTypeMask() const noexcept
			{
				return query_type_mask_;
			}


			//Returns the query region for this scene query
			//Returns nullopt this scene query is querying the entire scene
			[[nodiscard]] inline auto& QueryRegion() const noexcept
			{
				return query_region_;
			}

			//Returns whether or not this scene query is only querying visible objects
			[[nodiscard]] inline auto OnlyVisibleObjects() const noexcept
			{
				return only_visible_objects_;
			}

			//Returns the scene graph this scene query is querying
			[[nodiscard]] inline auto Scene() const noexcept
			{
				return scene_graph_;
			}


			/*
				Querying
			*/

			//Returns the result of the scene query
			[[nodiscard]] virtual ResultT Execute() const noexcept = 0;
	};
} //ion::graphics::scene::query

#endif