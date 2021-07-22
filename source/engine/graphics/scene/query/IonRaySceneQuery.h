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
		} //detail
	} //ray_scene_query


	class RaySceneQuery final : public SceneQuery<ray_scene_query::ResultType>
	{
		protected:



		public:

			//Default constructor
			RaySceneQuery() = default;

			//Construct a new scene query with the given scene graph
			RaySceneQuery(NonOwningPtr<SceneGraph> scene_graph) noexcept;


			/*
				Modifiers
			*/




			/*
				Observers
			*/

			


			/*
				Querying
			*/

			//Returns the result of the ray scene query
			[[nodiscard]] ray_scene_query::ResultType Execute() const noexcept override;
	};
} //ion::graphics::scene::query

#endif