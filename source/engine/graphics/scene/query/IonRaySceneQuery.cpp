/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/query
File:	IonRaySceneQuery.cpp
-------------------------------------------
*/

#include "IonRaySceneQuery.h"

#include <algorithm>

namespace ion::graphics::scene::query
{

using namespace ray_scene_query;
using namespace types::type_literals;

namespace ray_scene_query::detail
{

ResultType intersects(scene_query::detail::query_objects &objects, const Ray &ray) noexcept
{
	ResultType result;

	for (auto &object : objects)
	{
		switch (object.first->PreferredBoundingVolume())
		{
			case movable_object::PreferredBoundingVolumeType::BoundingSphere:
			{
				if (auto [hit, distance] = ray.Intersects(object.first->WorldBoundingSphere(false)); hit)
					result.emplace_back(object.first, distance);

				break;
			}

			case movable_object::PreferredBoundingVolumeType::BoundingBox:
			default:
			{
				if (auto [hit, distance] = ray.Intersects(object.first->WorldAxisAlignedBoundingBox(false)); hit)
				{
					if (object.first->ParentNode()->AxisAligned())
						result.emplace_back(object.first, distance);

					else if (auto [hit2, distance2] = ray.Intersects(object.first->WorldOrientedBoundingBox()); hit2)
						result.emplace_back(object.first, distance2);
				}

				break;
			}
		}
	}

	return result;
}

} //ray_scene_query::detail


RaySceneQuery::RaySceneQuery(NonOwningPtr<SceneGraph> scene_graph) noexcept :
	SceneQuery<ResultType>{scene_graph}
{
	//Empty
}

RaySceneQuery::RaySceneQuery(NonOwningPtr<SceneGraph> scene_graph, const Ray &ray) noexcept :
	
	SceneQuery<ResultType>{scene_graph},
	ray_{ray}
{
	//Empty
}


/*
	Querying
*/

ResultType RaySceneQuery::Execute() const noexcept
{
	if (!scene_graph_)
		return {};

	auto objects =
		scene_query::detail::get_eligible_objects(
			scene_graph_->RootNode(), query_type_mask_.value_or(~0_ui32), only_visible_objects_);
	scene_query::detail::derive_bounding_volumes(objects);

	if (query_region_)
		scene_query::detail::remove_objects_outside_region(objects, *query_region_);

	auto result = detail::intersects(objects, ray_);

	if (sort_by_distance_)
		std::sort(std::begin(result), std::end(result),
			[](auto &x, auto &y) noexcept
			{
				return x.second < y.second; //Asc
			});

	return result;
}

} //ion::graphics::scene::query