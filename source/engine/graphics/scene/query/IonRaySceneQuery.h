/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/query
File:	IonRaySceneQuery.h
-------------------------------------------
*/

#ifndef ION_RAY_SCENE_QUERY_H
#define ION_RAY_SCENE_QUERY_H

#include <optional>
#include <utility>
#include <vector>

#include "IonSceneQuery.h"
#include "graphics/scene/graph/IonSceneGraph.h"
#include "graphics/utilities/IonRay.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::query
{
	using namespace graphics::scene::graph;

	namespace ray_scene_query
	{
		using ResultType = std::vector<std::pair<MovableObject*, real>>;

		namespace detail
		{
			ResultType intersects(scene_query::detail::query_objects &objects, const Ray &ray, uint32 ray_flags) noexcept;
		} //detail
	} //ray_scene_query


	//A class representing an intersection scene query that queries object in a scene or inside a specific region
	//Queries objects only if a bitwise AND operation between the query mask and the object query flags is non-zero
	//A vector of intersected objects, with distance, are returned in the result type
	class RaySceneQuery final : public SceneQuery<ray_scene_query::ResultType>
	{
		private:

			Ray ray_;
			std::optional<uint32> ray_query_flags_;
			bool sort_by_distance_ = true;

		public:

			//Default constructor
			RaySceneQuery() = default;

			//Construct a new scene query with the given scene graph
			RaySceneQuery(NonOwningPtr<SceneGraph> scene_graph) noexcept;

			//Construct a new scene query with the given scene graph and ray
			RaySceneQuery(NonOwningPtr<SceneGraph> scene_graph, const Ray &ray) noexcept;


			/*
				Modifiers
			*/

			//Sets the ray this ray scene query is using when querying
			//This ray scene query will only query objects if a bitwise AND operation between the ray query flags and the object query mask is non-zero
			//The meaning of the bits is user-specific
			inline void RayQuerier(const Ray &ray, const std::optional<uint32> &query_flags = std::nullopt) noexcept
			{
				ray_ = ray;
				ray_query_flags_ = query_flags;
			}

			//Sets whether or not to this ray scene query is sorting query results by distance
			inline void SortByDistance(bool sort) noexcept
			{
				sort_by_distance_ = sort;
			}


			/*
				Observers
			*/

			//Returns the ray this ray scene query is using when querying
			[[nodiscard]] inline auto RayQuerier() const noexcept
			{
				return ray_;
			}

			//Returns the ray query flags this ray scene query is using when querying
			//This ray scene query will only query objects if a bitwise AND operation between the ray query flags and the object query mask is non-zero
			//The meaning of the bits is user-specific
			[[nodiscard]] inline auto RayQueryFlags() const noexcept
			{
				return ray_;
			}

			//Returns whether or not to this ray scene query is sorting query results by distance
			[[nodiscard]] inline auto SortByDistance() const noexcept
			{
				return sort_by_distance_;
			}


			/*
				Querying
			*/

			//Returns the result of the ray scene query
			[[nodiscard]] ray_scene_query::ResultType Execute() const noexcept override;
	};
} //ion::graphics::scene::query

#endif