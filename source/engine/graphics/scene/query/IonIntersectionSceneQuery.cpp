/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/query
File:	IonIntersectionSceneQuery.cpp
-------------------------------------------
*/

#include "IonIntersectionSceneQuery.h"

namespace ion::graphics::scene::query
{

using namespace intersection_scene_query;
using namespace types::type_literals;

namespace intersection_scene_query::detail
{

bool sphere_sphere_hit(const scene_query::detail::query_object &sphere_object, const scene_query::detail::query_object &sphere_object2) noexcept
{
	//Sphere vs Sphere
	return sphere_object.first->WorldBoundingSphere(false).
		Intersects(sphere_object2.first->WorldBoundingSphere(false));
}

bool box_box_hit(scene_query::detail::query_object &box_object, scene_query::detail::query_object &box_object2) noexcept
{
	//Aabb vs Aabb
	if (box_object.first->WorldAxisAlignedBoundingBox(false).
		Intersects(box_object2.first->WorldAxisAlignedBoundingBox(false)))
	{
		if (box_object.first->ParentNode()->AxisAligned() &&
			box_object2.first->ParentNode()->AxisAligned())
			return true;

		else //Obb vs Obb
			return box_object.first->WorldOrientedBoundingBox(derive_once(box_object.second)).
				Intersects(box_object2.first->WorldOrientedBoundingBox(derive_once(box_object2.second)));
	}

	return false;
}

bool sphere_box_hit(const scene_query::detail::query_object &sphere_object, scene_query::detail::query_object &box_object) noexcept
{
	//Sphere vs Aabb
	if (sphere_object.first->WorldBoundingSphere(false).
		Intersects(box_object.first->WorldAxisAlignedBoundingBox(false)))
	{
		if (box_object.first->ParentNode()->AxisAligned())
			return true;

		else //Sphere vs Obb
			return sphere_object.first->WorldBoundingSphere(false).
				Intersects(box_object.first->WorldOrientedBoundingBox(derive_once(box_object.second)));
	}

	return false;
}


ResultType intersects(scene_query::detail::query_objects &objects) noexcept
{
	ResultType result;

	for (auto iter = std::begin(objects), end = std::end(objects); iter != end; ++iter)
	{
		for (auto iter2 = iter + 1; iter2 != end; ++iter2)
		{
			if (iter->first->QueryFlags().value_or(~0_ui32) &
				iter2->first->QueryMask().value_or(~0_ui32))
			{
				switch (iter->first->PreferredBoundingVolume())
				{
					case movable_object::PreferredBoundingVolumeType::BoundingSphere:
					{
						switch (iter2->first->PreferredBoundingVolume())
						{
							case movable_object::PreferredBoundingVolumeType::BoundingSphere:
							{
								if (sphere_sphere_hit(*iter, *iter2))
									result.emplace_back(iter->first, iter2->first);
								break;
							}

							case movable_object::PreferredBoundingVolumeType::BoundingBox:
							default:
							{
								if (sphere_box_hit(*iter, *iter2))
									result.emplace_back(iter->first, iter2->first);
								break;
							}
						}

						break;
					}

					case movable_object::PreferredBoundingVolumeType::BoundingBox:
					default:
					{
						switch (iter2->first->PreferredBoundingVolume())
						{
							case movable_object::PreferredBoundingVolumeType::BoundingSphere:
							{
								if (sphere_box_hit(*iter2, *iter)) //Flipped
									result.emplace_back(iter->first, iter2->first);
								break;
							}

							case movable_object::PreferredBoundingVolumeType::BoundingBox:
							default:
							{
								if (box_box_hit(*iter, *iter2))
									result.emplace_back(iter->first, iter2->first);
								break;
							}
						}

						break;
					}
				}
			}
		}
	}

	return result;
}

} //intersection_scene_query::detail


//Private

/*
	Querying
*/

ResultType IntersectionSceneQuery::Execute(scene_query::detail::query_objects &objects) const noexcept
{
	scene_query::detail::derive_bounding_volumes(objects);

	if (query_region_)
		scene_query::detail::remove_objects_outside_region(objects, *query_region_);

	return detail::intersects(objects);
}

//Public

/*
	Querying
*/

ResultType IntersectionSceneQuery::Execute(SceneNode &node) const noexcept
{
	auto objects =
		scene_query::detail::get_eligible_objects(
			node, query_mask_.value_or(~0_ui32), query_type_mask_.value_or(~0_ui32), only_visible_objects_);

	return Execute(objects);
}

ResultType IntersectionSceneQuery::Execute(scene_query::MovableObjects &movable_objects) const noexcept
{
	auto objects =
		scene_query::detail::get_eligible_objects(
			movable_objects, query_mask_.value_or(~0_ui32), query_type_mask_.value_or(~0_ui32), only_visible_objects_);

	return Execute(objects);
}

} //ion::graphics::scene::query