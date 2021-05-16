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

#include <optional>
#include <unordered_set>
#include <vector>

#include "IonSceneNode.h"
#include "events/IonListenable.h"
#include "events/listeners/IonSceneNodeListener.h"
#include "graphics/render/IonFog.h"
#include "graphics/utilities/IonColor.h"
#include "types/IonTypes.h"

namespace ion::graphics::render
{
	class Viewport; //Forward declaration
}

namespace ion::graphics::scene
{
	class Light; //Forward declaration
}

namespace ion::graphics::shaders
{
	class ShaderProgram; //Forward declaration
}

namespace ion::graphics::scene::graph
{
	using namespace types::type_literals;
	using namespace graphics::utilities;

	namespace scene_graph
	{
		namespace detail
		{
			constexpr auto max_light_count = 8;
				//Warning: This value must be less or equal to the actual array size used for lights (in the fragment shader)
				//If scene graph contains more visible lights, then only the lights nearest to the geometry should be rendered


			/*
				Graphics API
			*/

			void set_fog_uniforms(std::optional<render::Fog> fog, shaders::ShaderProgram &shader_program) noexcept;
			void set_scene_uniforms(real gamma_value, Color ambient_color, int light_count, shaders::ShaderProgram &shader_program) noexcept;			
		} //detail
	} //scene_graph


	class SceneGraph final :
		protected events::Listenable<events::listeners::SceneNodeListener>
	{
		private:

			using NodeEventsBase = events::Listenable<events::listeners::SceneNodeListener>;


			real gamma_ = 1.0_r; //100% gamma
			Color ambient_color_ = color::White; //No ambient
			std::optional<render::Fog> fog_; //No fog
			bool lighting_enabled_ = true;

			SceneNode root_node_;
			bool update_uniforms_ = true;


			std::array<Light*, scene_graph::detail::max_light_count> active_lights_;
			std::unordered_set<shaders::ShaderProgram*> active_shader_programs_;


			/*
				Notifying
			*/

			void NotifyNodeRenderStarted(SceneNode &node) noexcept;
			void NotifyNodeRenderEnded(SceneNode &node) noexcept;

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

			//Sets the gamma of the scene to the given percent
			inline void Gamma(real percent) noexcept
			{
				if (gamma_ != percent)
				{
					gamma_ = percent;
					update_uniforms_ = true;
				}
			}

			//Sets the ambient color for this scene to the given color
			inline void AmbientColor(const Color &ambient) noexcept
			{
				if (ambient_color_ != ambient)
				{
					ambient_color_ = ambient;
					update_uniforms_ = true;
				}
			}

			//Sets the fog effect for this scene to the given fog
			//Pass nullopt to turn off fog effect for this scene
			inline void FogEffect(const std::optional<render::Fog> &fog) noexcept
			{
				fog_ = fog;
				update_uniforms_ = true;
			}

			//Sets whether or not this scene has lighting enabled
			inline void LightingEnabled(bool enable) noexcept
			{
				if (lighting_enabled_ != enable)
				{
					lighting_enabled_ = enable;
					update_uniforms_ = true;
				}
			}


			/*
				Observers
			*/

			//Returns the gamma of this scene
			[[nodiscard]] inline auto Gamma() const noexcept
			{
				return gamma_;
			}

			//Returns the ambient color for this scene
			[[nodiscard]] inline auto& AmbientColor() const noexcept
			{
				return ambient_color_;
			}

			//Returns the fog effect for this scene
			//Returns nullopt if the scene has no fog effect
			[[nodiscard]] inline auto& FogEffect() const noexcept
			{
				return fog_;
			}

			//Returns whether or not this scene has lighting enabled
			[[nodiscard]] inline auto LightingEnabled() const noexcept
			{
				return lighting_enabled_;
			}


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


			/*
				Rendering
			*/

			//Render this entire scene graph to the given viewport
			//This is called once from the engine, with the time in seconds since last frame
			void Render(render::Viewport &viewport, duration time) noexcept;
	};
} //ion::graphics::scene::graph

#endif