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

namespace ion::graphics::scene::query
{

using namespace ray_scene_query;
using namespace types::type_literals;

namespace ray_scene_query::detail
{
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

	return {};
}

} //ion::graphics::scene::query