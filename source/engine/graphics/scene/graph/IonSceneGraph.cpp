/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph
File:	IonSceneGraph.cpp
-------------------------------------------
*/

#include "IonSceneGraph.h"

#include <variant>

#include "graphics/scene/IonCamera.h"
#include "graphics/scene/IonLight.h"
#include "graphics/scene/IonMovableObject.h"

namespace ion::graphics::scene::graph
{

using namespace scene_graph;

namespace scene_graph
{

} //scene_graph


//Private

/*
	Notifying
*/

void SceneGraph::NotifyNodeRenderStarted(SceneNode &node) noexcept
{
	NodeEventsBase::NotifyAll(NodeEvents().Listeners(), &events::listeners::SceneNodeListener::NodeRenderStarted, std::ref(node));
}

void SceneGraph::NotifyNodeRenderEnded(SceneNode &node) noexcept
{
	NodeEventsBase::NotifyAll(NodeEvents().Listeners(), &events::listeners::SceneNodeListener::NodeRenderEnded, std::ref(node));
}


//Public

/*
	Rendering
*/

void SceneGraph::Render(duration time) noexcept
{
	//For each visible node
	for (auto &node : root_node_.OrderedSceneNodes())
	{
		//The node render started/ended events can be called without any attached objects
		//The visibility of the node is also used as a flag to enable/disable event notifications
		if (node.Visible())
		{
			NotifyNodeRenderStarted(node);

			//For each attached object
			for (auto &object : node.AttachedObjects())
			{
				std::visit(
					[=](auto &&object) noexcept
					{
						object->Elapse(time);
						object->Render();
					}, object);
			}

			NotifyNodeRenderEnded(node);
		}
	}
}

} //ion::graphics::scene::graph