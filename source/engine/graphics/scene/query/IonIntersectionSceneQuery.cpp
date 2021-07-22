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
	return {};
}

} //ion::graphics::scene::query