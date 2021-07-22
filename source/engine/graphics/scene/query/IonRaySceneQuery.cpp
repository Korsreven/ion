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

namespace ray_scene_query::detail
{
} //ray_scene_query::detail


RaySceneQuery::RaySceneQuery(NonOwningPtr<SceneGraph> scene_graph) noexcept :
	SceneQuery<ResultType>{scene_graph}
{
	//Empty
}


/*
	Querying
*/

ResultType RaySceneQuery::Execute() const noexcept
{
	return {};
}

} //ion::graphics::scene::query