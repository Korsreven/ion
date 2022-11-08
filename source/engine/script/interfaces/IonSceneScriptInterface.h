/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonSceneScriptInterface.h
-------------------------------------------
*/

#ifndef ION_SCENE_SCRIPT_INTERFACE_H
#define ION_SCENE_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "graphics/render/IonFrustum.h"
#include "graphics/render/IonRenderPass.h"
#include "graphics/render/IonRenderPrimitive.h"
#include "graphics/scene/IonCamera.h"
#include "graphics/scene/IonDrawableObject.h"
#include "graphics/scene/IonDrawableParticleSystem.h"
#include "graphics/scene/IonDrawableText.h"
#include "graphics/scene/IonLight.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonMovableObject.h"
#include "graphics/scene/IonMovableSound.h"
#include "graphics/scene/IonMovableSoundListener.h"
#include "graphics/scene/graph/IonSceneGraph.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/scene/graph/animations/IonNodeAnimation.h"
#include "graphics/scene/graph/animations/IonNodeAnimationGroup.h"
#include "graphics/scene/graph/animations/IonNodeAnimationTimeline.h"
#include "graphics/scene/shapes/IonAnimatedSprite.h"
#include "graphics/scene/shapes/IonBorder.h"
#include "graphics/scene/shapes/IonCurve.h"
#include "graphics/scene/shapes/IonEllipse.h"
#include "graphics/scene/shapes/IonLine.h"
#include "graphics/scene/shapes/IonMesh.h"
#include "graphics/scene/shapes/IonRectangle.h"
#include "graphics/scene/shapes/IonShape.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "graphics/scene/shapes/IonTriangle.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::graphics
{
	namespace scene
	{
		class SceneManager;
	}
}

namespace ion::script::interfaces
{
	namespace scene_script_interface::detail
	{
		static inline const Strings motion_technique_types
		{
			"cubic",
			"exponential",
			"linear",
			"logarithmic",
			"sigmoid",
			"sinh",
			"tanh"
		};

		static inline const Strings render_pass_blend_factors
		{
			"zero",
			"one",

			"source-color",
			"one-minus-source-color",
			"destination-color",
			"one-minus-destination-color",

			"source-alpha",
			"one-minus-source-alpha",
			"destination-alpha",
			"one-minus-destination-alpha",

			"constant-color",
			"one-minus-constant-color",
			"constant-alpha",
			"one-minus-constant-alpha",

			"source-one-color",
			"one-minus-source-one-color",
			"source-one-alpha",
			"one-minus-source-one-alpha",

			"source-alpha-saturate"
		};

		static inline const Strings render_pass_blend_equation_modes
		{
			"add",
			"subtract",
			"reverse-subtract",
			"min",
			"max"
		};


		NonOwningPtr<graphics::materials::Material> get_material(std::string_view name, const ManagerRegister &managers) noexcept;
		NonOwningPtr<graphics::particles::ParticleSystem> get_particle_system(std::string_view name, const ManagerRegister &managers) noexcept;
		NonOwningPtr<graphics::shaders::ShaderProgram> get_shader_program(std::string_view name, const ManagerRegister &managers) noexcept;
		NonOwningPtr<sounds::Sound> get_sound(std::string_view name, const ManagerRegister &managers) noexcept;
		NonOwningPtr<sounds::SoundChannelGroup> get_sound_channel_group(std::string_view name, const ManagerRegister &managers) noexcept;
		NonOwningPtr<sounds::SoundListener> get_sound_listener(std::string_view name, const ManagerRegister &managers) noexcept;
		NonOwningPtr<graphics::fonts::Text> get_text(std::string_view name, const ManagerRegister &managers) noexcept;

		graphics::scene::graph::animations::node_animation::MotionTechniqueType get_motion_technique_type(const script_tree::ArgumentNode &arg);
		graphics::render::render_pass::BlendFactor get_pass_blend_factor(const script_tree::ArgumentNode &arg);
		graphics::render::render_pass::BlendEquationMode get_pass_blend_equation_mode(const script_tree::ArgumentNode &arg);

		NonOwningPtr<graphics::scene::graph::animations::NodeAnimation> get_node_animation(std::string_view name,
			graphics::scene::graph::SceneNode &parent_node) noexcept;
		NonOwningPtr<graphics::scene::graph::animations::NodeAnimationGroup> get_node_animation_group(std::string_view name,
			graphics::scene::graph::SceneNode &parent_node) noexcept;


		/**
			@name Validator classes
			@{
		*/

		script_validator::ClassDefinition get_action_class();
		script_validator::ClassDefinition get_frustum_class();
		script_validator::ClassDefinition get_render_pass_class();
		script_validator::ClassDefinition get_rotating_class();
		script_validator::ClassDefinition get_scaling_class();
		script_validator::ClassDefinition get_translating_class();

		script_validator::ClassDefinition get_animated_sprite_class();
		script_validator::ClassDefinition get_border_class();
		script_validator::ClassDefinition get_curve_class();
		script_validator::ClassDefinition get_ellipse_class();
		script_validator::ClassDefinition get_line_class();
		script_validator::ClassDefinition get_mesh_class();
		script_validator::ClassDefinition get_rectangle_class();
		script_validator::ClassDefinition get_render_primitive_class();
		script_validator::ClassDefinition get_shape_class();
		script_validator::ClassDefinition get_sprite_class();
		script_validator::ClassDefinition get_triangle_class();

		script_validator::ClassDefinition get_node_animation_class();
		script_validator::ClassDefinition get_node_animation_group_class();
		script_validator::ClassDefinition get_node_animation_timeline_class();
		script_validator::ClassDefinition get_scene_node_class();

		script_validator::ClassDefinition get_camera_class();
		script_validator::ClassDefinition get_drawable_object_class();
		script_validator::ClassDefinition get_drawable_particle_system_class();
		script_validator::ClassDefinition get_drawable_text_class();
		script_validator::ClassDefinition get_light_class();
		script_validator::ClassDefinition get_model_class();
		script_validator::ClassDefinition get_movable_object_class();
		script_validator::ClassDefinition get_movable_sound_class();
		script_validator::ClassDefinition get_movable_sound_listener_class();

		ScriptValidator get_scene_validator();

		///@}

		/**
			@name Tree parsing
			@{
		*/
		
		void set_frustum_properties(const script_tree::ObjectNode &object, graphics::render::Frustum &frustum);
		void set_render_pass_properties(const script_tree::ObjectNode &object, graphics::render::RenderPass &pass,
			const ManagerRegister &managers);

		void set_animated_sprite_properties(const script_tree::ObjectNode &object, graphics::scene::shapes::AnimatedSprite &aniamted_sprite,
			const ManagerRegister &managers);
		void set_border_properties(const script_tree::ObjectNode &object, graphics::scene::shapes::Border &border,
			const ManagerRegister &managers);
		void set_curve_properties(const script_tree::ObjectNode &object, graphics::scene::shapes::Curve &curve,
			const ManagerRegister &managers);
		void set_ellipse_properties(const script_tree::ObjectNode &object, graphics::scene::shapes::Ellipse &ellipse,
			const ManagerRegister &managers);
		void set_line_properties(const script_tree::ObjectNode &object, graphics::scene::shapes::Line &line,
			const ManagerRegister &managers);
		void set_mesh_properties(const script_tree::ObjectNode &object, graphics::scene::shapes::Mesh &mesh,
			const ManagerRegister &managers);
		void set_rectangle_properties(const script_tree::ObjectNode &object, graphics::scene::shapes::Rectangle &rectangle,
			const ManagerRegister &managers);
		void set_render_primitive_properties(const script_tree::ObjectNode &object, graphics::render::RenderPrimitive &primitive,
			const ManagerRegister &managers);
		void set_shape_properties(const script_tree::ObjectNode &object, graphics::scene::shapes::Shape &shape,
			const ManagerRegister &managers);
		void set_sprite_properties(const script_tree::ObjectNode &object, graphics::scene::shapes::Sprite &sprite,
			const ManagerRegister &managers);
		void set_triangle_properties(const script_tree::ObjectNode &object, graphics::scene::shapes::Triangle &triangle,
			const ManagerRegister &managers);

		void set_node_animation_properties(const script_tree::ObjectNode &object, graphics::scene::graph::animations::NodeAnimation &animation);
		void set_node_animation_group_properties(const script_tree::ObjectNode &object, graphics::scene::graph::animations::NodeAnimationGroup &animation_group,
			graphics::scene::graph::SceneNode &parent_node);
		void set_node_animation_timeline_properties(const script_tree::ObjectNode &object, graphics::scene::graph::animations::NodeAnimationTimeline &timeline,
			graphics::scene::graph::SceneNode &parent_node);
		void set_scene_node_properties(const script_tree::ObjectNode &object, graphics::scene::graph::SceneNode &scene_node,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);

		void set_camera_properties(const script_tree::ObjectNode &object, graphics::scene::Camera &camera);
		void set_drawable_object_properties(const script_tree::ObjectNode &object, graphics::scene::DrawableObject &drawable,
			const ManagerRegister &managers);
		void set_drawable_particle_system_properties(const script_tree::ObjectNode &object, graphics::scene::DrawableParticleSystem &particle_system,
			const ManagerRegister &managers);
		void set_drawable_text_properties(const script_tree::ObjectNode &object, graphics::scene::DrawableText &text,
			const ManagerRegister &managers);
		void set_light_properties(const script_tree::ObjectNode &object, graphics::scene::Light &light);
		void set_model_properties(const script_tree::ObjectNode &object, graphics::scene::Model &model,
			const ManagerRegister &managers);
		void set_movable_object_properties(const script_tree::ObjectNode &object, graphics::scene::MovableObject &movable);
		void set_movable_sound_properties(const script_tree::ObjectNode &object, graphics::scene::MovableSound &sound);
		void set_movable_sound_listener_properties(const script_tree::ObjectNode &object, graphics::scene::MovableSoundListener &sound_listener);

		
		graphics::render::Frustum create_frustum(const script_tree::ObjectNode &object);
		graphics::render::RenderPass create_render_pass(const script_tree::ObjectNode &object,
			const ManagerRegister &managers);

		void create_action(const script_tree::ObjectNode &object,
			graphics::scene::graph::animations::NodeAnimation &animation);
		void create_rotating_motion(const script_tree::ObjectNode &object,
			graphics::scene::graph::animations::NodeAnimation &animation);
		void create_scaling_motion(const script_tree::ObjectNode &object,
			graphics::scene::graph::animations::NodeAnimation &animation);
		void create_translating_motion(const script_tree::ObjectNode &object,
			graphics::scene::graph::animations::NodeAnimation &animation);

		NonOwningPtr<graphics::scene::shapes::AnimatedSprite> create_animated_sprite(const script_tree::ObjectNode &object,
			graphics::scene::Model &model, const ManagerRegister &managers);
		NonOwningPtr<graphics::scene::shapes::Border> create_border(const script_tree::ObjectNode &object,
			graphics::scene::Model &model, const ManagerRegister &managers);
		NonOwningPtr<graphics::scene::shapes::Curve> create_curve(const script_tree::ObjectNode &object,
			graphics::scene::Model &model, const ManagerRegister &managers);
		NonOwningPtr<graphics::scene::shapes::Ellipse> create_ellipse(const script_tree::ObjectNode &object,
			graphics::scene::Model &model, const ManagerRegister &managers);
		NonOwningPtr<graphics::scene::shapes::Line> create_line(const script_tree::ObjectNode &object,
			graphics::scene::Model &model, const ManagerRegister &managers);
		NonOwningPtr<graphics::scene::shapes::Mesh> create_mesh(const script_tree::ObjectNode &object,
			graphics::scene::Model &model, const ManagerRegister &managers);
		NonOwningPtr<graphics::scene::shapes::Rectangle> create_rectangle(const script_tree::ObjectNode &object,
			graphics::scene::Model &model, const ManagerRegister &managers);
		NonOwningPtr<graphics::scene::shapes::Sprite> create_sprite(const script_tree::ObjectNode &object,
			graphics::scene::Model &model, const ManagerRegister &managers);
		NonOwningPtr<graphics::scene::shapes::Triangle> create_triangle(const script_tree::ObjectNode &object,
			graphics::scene::Model &model, const ManagerRegister &managers);
		
		NonOwningPtr<graphics::scene::graph::animations::NodeAnimation> create_node_animation(const script_tree::ObjectNode &object,
			graphics::scene::graph::SceneNode &parent_node);
		NonOwningPtr<graphics::scene::graph::animations::NodeAnimationGroup> create_node_animation_group(const script_tree::ObjectNode &object,
			graphics::scene::graph::SceneNode &parent_node);
		NonOwningPtr<graphics::scene::graph::animations::NodeAnimationTimeline> create_node_animation_timeline(const script_tree::ObjectNode &object,
			graphics::scene::graph::SceneNode &parent_node);
		NonOwningPtr<graphics::scene::graph::SceneNode> create_scene_node(const script_tree::ObjectNode &object,
			graphics::scene::graph::SceneNode &parent_node, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);

		NonOwningPtr<graphics::scene::Camera> create_camera(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager);
		NonOwningPtr<graphics::scene::DrawableParticleSystem> create_drawable_particle_system(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<graphics::scene::DrawableText> create_drawable_text(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<graphics::scene::Light> create_light(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager);
		NonOwningPtr<graphics::scene::Model> create_model(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<graphics::scene::MovableSound> create_movable_sound(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<graphics::scene::MovableSoundListener> create_movable_sound_listener(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);

		void create_scene(const ScriptTree &tree, graphics::scene::graph::SceneNode &parent_node,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);

		///@}
	} //scene_script_interface::detail


	///@brief A class representing an interface to a scene script with a complete validation scheme
	///@details A scene script can load scene nodes (with attached objects) from a script file into a scene graph (and associated scene manager)
	class SceneScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			///@brief Default constructor
			SceneScriptInterface() = default;


			/**
				@name Scene
				Creating from script
				@{
			*/

			///@brief Creates scene from a script (or object file) with the given asset name
			void CreateScene(std::string_view asset_name, graphics::scene::graph::SceneNode &parent_node,
				graphics::scene::SceneManager &scene_manager);

			///@brief Creates scene from a script (or object file) with the given asset name
			void CreateScene(std::string_view asset_name, graphics::scene::graph::SceneNode &parent_node,
				graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);

			///@}
	};
} //ion::script::interfaces

#endif