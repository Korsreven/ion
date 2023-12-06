/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonSceneGraphListener.h
-------------------------------------------
*/

#ifndef ION_SCENE_GRAPH_LISTENER_H
#define ION_SCENE_GRAPH_LISTENER_H

#include "IonListener.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::graph
{
	class SceneGraph; //Forward declaration
}

namespace ion::events::listeners
{
	///@brief A class representing a listener that listens to scene graph events
	struct SceneGraphListener : Listener<SceneGraphListener>
	{
		/**
			@name Events
			@{
		*/

		///@brief Called when a scene graph is about to begin rendering, with a reference to the scene graph
		virtual void GraphRenderStarted(graphics::scene::graph::SceneGraph &graph) noexcept = 0;
		
		///@brief Called just after a scene graph has been rendered, with a reference to the scene graph
		virtual void GraphRenderEnded(graphics::scene::graph::SceneGraph &graph) noexcept = 0;

		///@}
	};
} //ion::events::listeners

#endif