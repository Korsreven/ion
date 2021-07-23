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
} //intersection_scene_query::detail


IntersectionSceneQuery::IntersectionSceneQuery(NonOwningPtr<SceneGraph> scene_graph) noexcept :
	SceneQuery<ResultType>{scene_graph}
{
	//Empty
}


/*
	Querying
*/

ResultType IntersectionSceneQuery::Execute() const noexcept
{
	if (!scene_graph_)
		return {};

	auto objects =
		scene_query::detail::get_eligible_objects(
			scene_graph_->RootNode(), query_type_mask_.value_or(~0_ui32), only_visible_objects_);

	return {};
}

} //ion::graphics::scene::query