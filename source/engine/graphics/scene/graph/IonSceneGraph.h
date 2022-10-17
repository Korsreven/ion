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

#ifndef ION_SCENE_GRAPH_H
#define ION_SCENE_GRAPH_H

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "IonSceneNode.h"
#include "events/IonListenable.h"
#include "events/listeners/IonSceneNodeListener.h"
#include "graphics/render/IonFog.h"
#include "graphics/render/IonRenderer.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/textures/IonTexture.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonMatrix4.h"
#include "graphics/utilities/IonVector3.h"
#include "managed/IonManagedObject.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion
{
	class Engine; //Forward declaration

	namespace graphics
	{
		namespace render
		{
			class Viewport;
		}

		namespace scene
		{
			class Camera;
			class Light;
		}

		namespace shaders
		{
			class ShaderProgram;
		}
	}
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
				//NOT IN USE when light data textures are used instead of plain arrays

			constexpr auto max_lights_in_mask = 128;
				//Warning: This value must be less or equal to the total bits used for the light (visibility) mask
				//uvec4 has 4 x 32 bit unsigned integers which sums up to a total of 128 bits

			using light_pointers = std::vector<Light*>;
			using movable_object_pointers = std::vector<MovableObject*>;
			using shader_program_pointers = std::vector<shaders::ShaderProgram*>;
			using uvec4 = std::array<uint32, 4>;

			void get_light_mask(const light_pointers &lights, const MovableObject &object, uvec4 &light_mask) noexcept;


			/*
				Uniforms
			*/

			void set_camera_uniforms(const Camera &camera, shaders::ShaderProgram &shader_program) noexcept;
			void set_fog_uniforms(std::optional<render::Fog> fog, shaders::ShaderProgram &shader_program) noexcept;
			void set_light_uniforms(const light_pointers &lights, const MovableObject &object, uvec4 &light_mask,
				shaders::ShaderProgram &shader_program) noexcept;
			void set_emissive_light_uniforms(const light_pointers &lights, const MovableObject &object, uvec4 &light_mask,
				shaders::ShaderProgram &shader_program) noexcept;
			void set_light_uniforms(const light_pointers &lights, std::optional<textures::texture::TextureHandle> &texture_handle,
				const Camera &camera, shaders::ShaderProgram &shader_program) noexcept;
			void set_emissive_light_uniforms(const light_pointers &lights, std::optional<textures::texture::TextureHandle> &texture_handle,
				const Camera &camera, shaders::ShaderProgram &shader_program) noexcept;
			void set_matrix_uniforms(const Matrix4 &projection_mat, shaders::ShaderProgram &shader_program) noexcept;
			void set_matrix_uniforms(const Matrix4 &projection_mat, const Matrix4 &model_view_mat, shaders::ShaderProgram &shader_program) noexcept;
			void set_scene_uniforms(real gamma_value, Color ambient_color, shaders::ShaderProgram &shader_program) noexcept;


			/*
				Graphics API
			*/

			void set_gl_model_view_matrix(const Matrix4 &model_view_mat) noexcept;
			void mult_gl_model_view_matrix(const Matrix4 &model_view_mat) noexcept;
			Matrix4 get_gl_model_view_matrix() noexcept;
			void push_gl_matrix() noexcept;
			void pop_gl_matrix() noexcept;
		} //detail
	} //scene_graph


	//A class that manages the scene and stores scene nodes in a tree structure (from the root node)
	//The scene graph is responsible for updating nodes, shader programs, camera, lights and movable objects
	class SceneGraph final :
		public managed::ManagedObject<Engine>,
		public managed::ObjectManager<SceneManager, SceneGraph>,
		public events::Listenable<events::listeners::SceneNodeListener>
	{
		private:

			using NodeEventsBase = events::Listenable<events::listeners::SceneNodeListener>;

			bool enabled_ = true;
			real gamma_ = 1.0_r; //100% gamma
			Color ambient_color_ = color::White; //No ambient
			std::optional<render::Fog> fog_; //No fog
			bool fog_enabled_ = true;
			bool lighting_enabled_ = true;		

			SceneNode root_node_;
			render::Renderer renderer_;


			scene_graph::detail::light_pointers lights_;
			scene_graph::detail::light_pointers emissive_lights_;
			scene_graph::detail::uvec4 light_mask_{};
			scene_graph::detail::uvec4 emissive_light_mask_{};

			scene_graph::detail::movable_object_pointers visible_objects_;
			scene_graph::detail::shader_program_pointers shader_programs_;
				//Keep these as members so we don't have to reallocate storage for each render call

			std::optional<textures::texture::TextureHandle> light_texture_handle_;
			std::optional<textures::texture::TextureHandle> emissive_light_texture_handle_;


			/*
				Notifying
			*/

			void NotifyNodeRenderStarted(SceneNode &node) noexcept;
			void NotifyNodeRenderEnded(SceneNode &node) noexcept;

		public:

			//Construct a scene graph with the given name and whether or not is should be enabled
			explicit SceneGraph(std::optional<std::string> name = {}, bool enabled = true);

			//Deleted copy constructor
			SceneGraph(const SceneGraph&) = delete;

			//Default move constructor
			SceneGraph(SceneGraph&&) = default;

			//Destructor
			~SceneGraph() noexcept;


			/*
				Operators
			*/

			//Deleted copy assignment
			SceneGraph& operator=(const SceneGraph&) = delete;

			//Default move assignment
			SceneGraph& operator=(SceneGraph&&) = default;


			/*
				Events
			*/

			//Return a mutable reference to the node events of this scene graph
			[[nodiscard]] inline auto& NodeEvents() noexcept
			{
				return static_cast<NodeEventsBase&>(*this);
			}

			//Return an immutable reference to the node events of this scene graph
			[[nodiscard]] inline auto& NodeEvents() const noexcept
			{
				return static_cast<const NodeEventsBase&>(*this);
			}


			/*
				Modifiers
			*/

			//Enable the scene graph rendering
			inline void Enable() noexcept
			{
				enabled_ = true;
			}

			//Disable the scene graph rendering
			inline void Disable() noexcept
			{
				enabled_ = false;
			}

			//Sets whether or not the scene graph rendering is enabled
			inline void Enabled(bool enabled) noexcept
			{
				if (enabled)
					Enable();
				else
					Disable();
			}


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

			//Sets whether or not this scene has fog effect enabled
			inline void FogEnabled(bool enabled) noexcept
			{
				fog_enabled_ = enabled;
			}

			//Sets whether or not this scene has lighting enabled
			inline void LightingEnabled(bool enabled) noexcept
			{
				lighting_enabled_ = enabled;
			}


			/*
				Observers
			*/

			//Returns true if the scene graph rendering is enabled
			[[nodiscard]] inline auto IsEnabled() const noexcept
			{
				return enabled_;
			}

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

			//Returns whether or not this scene has fog effect enabled
			[[nodiscard]] inline auto FogEnabled() const noexcept
			{
				return fog_enabled_;
			}

			//Returns whether or not this scene has lighting enabled
			[[nodiscard]] inline auto LightingEnabled() const noexcept
			{
				return lighting_enabled_;
			}


			//Returns a mutable reference to the root node of this scene graph
			[[nodiscard]] inline auto& RootNode() noexcept
			{
				return root_node_;
			}

			//Returns an immutable reference to the root node of this scene graph
			[[nodiscard]] inline auto& RootNode() const noexcept
			{
				return root_node_;
			}

			//Returns a mutable reference to the renderer of this scene graph
			[[nodiscard]] inline auto& SceneRenderer() noexcept
			{
				return renderer_;
			}

			//Returns an immutable reference to the renderer of this scene graph
			[[nodiscard]] inline auto& SceneRenderer() const noexcept
			{
				return renderer_;
			}


			//Returns an immutable reference to all lights in this scene
			[[nodiscard]] inline auto& Lights() const noexcept
			{
				return lights_;
			}

			//Returns an immutable reference to all lights in this scene
			[[nodiscard]] inline auto& EmissiveLights() const noexcept
			{
				return emissive_lights_;
			}

			//Returns a texture handle to the lights in this scene
			//Returns nullopt if this scene does not use a texture for the lights
			[[nodiscard]] inline auto LightTextureHandle() const noexcept
			{
				return light_texture_handle_;
			}

			//Returns a texture handle to the emissive lights in this scene
			//Returns nullopt if this scene does not use a texture for the emissive lights
			[[nodiscard]] inline auto EmissiveLightTextureHandle() const noexcept
			{
				return emissive_light_texture_handle_;
			}


			/*
				Rendering
			*/

			//Render this entire scene graph to the given viewport
			//This is called once from the engine, with the time in seconds since last frame
			void Render(render::Viewport &viewport, duration time) noexcept;


			/*
				Scene managers
				Ranges
			*/

			//Returns a mutable range of all scene managers in this render target
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto SceneManagers() noexcept
			{
				return Objects();
			}

			//Returns an immutable range of all scene managers in this render target
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto SceneManagers() const noexcept
			{
				return Objects();
			}


			/*
				Scene mangers
				Creating
			*/

			//Create a scene manager with the given name
			NonOwningPtr<SceneManager> CreateSceneManager(std::optional<std::string> name = {}) noexcept;


			/*
				Scene mangers
				Retrieving
			*/

			//Gets a pointer to a mutable scene manager with the given name
			//Returns nullptr if scene manager could not be found
			[[nodiscard]] NonOwningPtr<SceneManager> GetSceneManager(std::string_view name) noexcept;

			//Gets a pointer to an immutable scene manager with the given name
			//Returns nullptr if scene manager could not be found
			[[nodiscard]] NonOwningPtr<const SceneManager> GetSceneManager(std::string_view name) const noexcept;


			/*
				Scene mangers
				Removing
			*/

			//Clear all removable scene managers from this manager
			void ClearSceneMangers() noexcept;

			//Remove a removable scene manager from this manager
			bool RemoveSceneManager(SceneManager &scene_manager) noexcept;

			//Remove a removable scene manager with the given name from this manager
			bool RemoveSceneManager(std::string_view name) noexcept;
	};
} //ion::graphics::scene::graph

#endif