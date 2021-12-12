/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonSceneNodeListener.h
-------------------------------------------
*/

#ifndef ION_SCENE_NODE_LISTENER_H
#define ION_SCENE_NODE_LISTENER_H

#include "IonListener.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::graph
{
	class SceneNode; //Forward declaration
}

namespace ion::events::listeners
{
	struct SceneNodeListener : Listener<SceneNodeListener>
	{
		/*
			Events
		*/

		//Called when a scene node is about to begin rendering, with a reference to the scene node
		virtual void NodeRenderStarted(graphics::scene::graph::SceneNode &node) noexcept = 0;
		
		//Called just after a scene node has been rendered, with a reference to the scene node
		virtual void NodeRenderEnded(graphics::scene::graph::SceneNode &node) noexcept = 0;
	};
} //ion::events::listeners

#endif