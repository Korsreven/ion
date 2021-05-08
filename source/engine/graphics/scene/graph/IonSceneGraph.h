/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph
File:	IonSceneGraph.h
-------------------------------------------
*/

#ifndef ION_SCENE_GRAPH
#define ION_SCENE_GRAPH

#include "IonSceneNode.h"
#include "events/IonListenable.h"
#include "events/listeners/IonSceneNodeListener.h"

namespace ion::graphics::scene::graph
{
	namespace scene_graph
	{
		namespace detail
		{
		} //detail
	} //scene_graph


	class SceneGraph final :
		protected events::Listenable<events::listeners::SceneNodeListener>
	{
		private:

			using NodeEventsBase = events::Listenable<events::listeners::SceneNodeListener>;


			SceneNode root_node_;

		public:

			//Default constructor
			SceneGraph() = default;


			/*
				Events
			*/

			//Return a mutable reference to the node events of this scene graph
			[[nodiscard]] inline auto& NodeEvents() noexcept
			{
				return static_cast<NodeEventsBase&>(*this);
			}

			//Return a immutable reference to the node events of this scene graph
			[[nodiscard]] inline auto& NodeEvents() const noexcept
			{
				return static_cast<const NodeEventsBase&>(*this);
			}


			/*
				Modifiers
			*/




			/*
				Observers
			*/

			//Return a mutable reference to the root node of this scene graph
			[[nodiscard]] inline auto& RootNode() noexcept
			{
				return root_node_;
			}

			//Return an immutable reference to the root node of this scene graph
			[[nodiscard]] inline auto& RootNode() const noexcept
			{
				return root_node_;
			}
	};
} //ion::graphics::scene::graph

#endif