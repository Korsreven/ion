/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/query
File:	IonIntersectionSceneQuery.h
-------------------------------------------
*/

#ifndef ION_INTERSECTION_SCENE_QUERY
#define ION_INTERSECTION_SCENE_QUERY

#include <utility>
#include <vector>

#include "IonSceneQuery.h"
#include "graphics/scene/graph/IonSceneGraph.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::query
{
	using namespace graphics::scene::graph;

	namespace intersection_scene_query
	{
		using ResultType = std::vector<std::pair<MovableObject*, MovableObject*>>;

		namespace detail
		{
			ResultType intersects(scene_query::detail::query_objects &objects) noexcept;
			ResultType intersects(scene_query::detail::query_objects &objects, scene_query::detail::query_objects &objects2) noexcept;
		} //detail
	} //intersection_scene_query


	class IntersectionSceneQuery final : public SceneQuery<intersection_scene_query::ResultType>
	{
		private:



		public:

			//Default constructor
			IntersectionSceneQuery() = default;

			//Construct a new scene query with the given scene graph
			IntersectionSceneQuery(NonOwningPtr<SceneGraph> scene_graph) noexcept;


			/*
				Modifiers
			*/




			/*
				Observers
			*/



			
			/*
				Querying
			*/

			//Returns the result of the intersection scene query
			[[nodiscard]] intersection_scene_query::ResultType Execute() const noexcept override;
	};
} //ion::graphics::scene::query

#endif