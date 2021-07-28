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

#ifndef ION_RAY_SCENE_QUERY
#define ION_RAY_SCENE_QUERY

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
			ResultType intersects(scene_query::detail::query_objects &objects, const Ray &ray) noexcept;
		} //detail
	} //ray_scene_query


	class RaySceneQuery final : public SceneQuery<ray_scene_query::ResultType>
	{
		private:

			Ray ray_;
			bool sort_by_distance_ = true;
			std::optional<int> max_results_;

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
			inline void SceneRay(const Ray &ray) noexcept
			{
				ray_ = ray;
			}

			//Sets whether or not to this ray scene query is sorting query results by distance
			inline void SortByDistance(bool sort) noexcept
			{
				sort_by_distance_ = sort;
			}

			//Sets the max results this ray scene query is returning
			//Pass nullopt to allow indefinitely number of results
			inline void MaxResults(std::optional<int> max_results) noexcept
			{
				max_results_ = max_results;
			}
			


			/*
				Observers
			*/

			//Returns the ray this ray scene query is using when querying
			[[nodiscard]] inline auto SceneRay() const noexcept
			{
				return ray_;
			}

			//Returns whether or not to this ray scene query is sorting query results by distance
			[[nodiscard]] inline auto SortByDistance() const noexcept
			{
				return sort_by_distance_;
			}

			//Returns the max results this ray scene query is returning
			//Returns nullopt if there is no max results set
			[[nodiscard]] inline auto MaxResults() const noexcept
			{
				return max_results_;
			}


			/*
				Querying
			*/

			//Returns the result of the ray scene query
			[[nodiscard]] ray_scene_query::ResultType Execute() const noexcept override;
	};
} //ion::graphics::scene::query

#endif