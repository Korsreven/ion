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

#ifndef ION_INTERSECTION_SCENE_QUERY_H
#define ION_INTERSECTION_SCENE_QUERY_H

#include <utility>
#include <vector>

#include "IonSceneQuery.h"
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
			inline auto derive_once(bool &derive) noexcept
			{
				auto temp = derive;
				return derive = false, temp;
			}

			bool sphere_sphere_hit(const scene_query::detail::query_object &sphere_object, const scene_query::detail::query_object &sphere_object2) noexcept;
			bool box_box_hit(scene_query::detail::query_object &box_object, scene_query::detail::query_object &box_object2) noexcept;
			bool sphere_box_hit(const scene_query::detail::query_object &sphere_object, scene_query::detail::query_object &box_object) noexcept;

			ResultType intersects(scene_query::detail::query_objects &objects) noexcept;
		} //detail
	} //intersection_scene_query


	///@brief A class representing an intersection scene query that queries object in a scene or inside a specific region
	///@details Queries objects only if a bitwise AND operation between the query mask and the object query flags is non-zero.
	///Unique pairs of intersected objects are returned in the result type
	class IntersectionSceneQuery final : public SceneQuery<intersection_scene_query::ResultType>
	{
		private:
			
			/**
				@name Querying
				@{
			*/

			intersection_scene_query::ResultType Execute(scene_query::detail::query_objects &objects) const noexcept;

			///@}

		public:

			///@brief Default constructor
			IntersectionSceneQuery() = default;

			
			/**
				@name Querying
				@{
			*/

			///@brief Returns the result of the intersection scene query from the given node
			[[nodiscard]] intersection_scene_query::ResultType Execute(SceneNode &node) const noexcept override;

			///@brief Returns the result of the intersection scene query amongst the given movable objects
			[[nodiscard]] intersection_scene_query::ResultType Execute(scene_query::MovableObjects &movable_objects) const noexcept override;

			///@}
	};
} //ion::graphics::scene::query

#endif