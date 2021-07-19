/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/query
File:	IonSceneQuery.cpp
-------------------------------------------
*/

#include "IonSceneQuery.h"

namespace ion::graphics::scene::query
{

using namespace scene_query;

namespace scene_query::detail
{
} //scene_query::detail


SceneQuery::SceneQuery(NonOwningPtr<SceneGraph> scene_graph) noexcept :
	scene_graph_{scene_graph}
{
	//Empty
}

} //ion::graphics::scene::query