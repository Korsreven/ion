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
#include <vector>

#include "IonSceneNode.h"
#include "events/IonListenable.h"
#include "events/listeners/IonSceneNodeListener.h"
#include "graphics/render/IonFog.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonMatrix4.h"
#include "graphics/utilities/IonVector3.h"
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

			using light_container = std::array<Light*, max_light_count>;
			using shader_program_container = std::vector<shaders::ShaderProgram*>;


			/*
				Graphics API
			*/

			void set_camera_uniforms(const Camera &camera, shaders::ShaderProgram &shader_program) noexcept;
			void set_fog_uniforms(std::optional<render::Fog> fog, shaders::ShaderProgram &shader_program) noexcept;
			void set_light_uniforms(const light_container &lights, int light_count, const Camera &camera, shaders::ShaderProgram &shader_program) noexcept;	
			void set_matrix_uniforms(const Matrix4 &projection_mat, shaders::ShaderProgram &shader_program) noexcept;
			void set_matrix_uniforms(const Matrix4 &projection_mat, const Matrix4 &model_view_mat, shaders::ShaderProgram &shader_program) noexcept;
			void set_scene_uniforms(real gamma_value, Color ambient_color, int light_count, shaders::ShaderProgram &shader_program) noexcept;

			void set_gl_model_view_matrix(const Matrix4 &model_view_mat) noexcept;
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


			scene_graph::detail::light_container active_lights_;

			scene_graph::detail::shader_program_container shader_programs_;
			scene_graph::detail::shader_program_container shader_programs_node_;
				//Keep these as members so we don't have to reallocate storage for each render call


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
				gamma_ = percent;
			}

			//Sets the ambient color for this scene to the given color
			inline void AmbientColor(const Color &ambient) noexcept
			{
				ambient_color_ = ambient;
			}

			//Sets the fog effect for this scene to the given fog
			//Pass nullopt to turn off fog effect for this scene
			inline void FogEffect(const std::optional<render::Fog> &fog) noexcept
			{
				fog_ = fog;
			}

			//Sets whether or not this scene has lighting enabled
			inline void LightingEnabled(bool enable) noexcept
			{
				lighting_enabled_ = enable;
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