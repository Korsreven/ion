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
#include "graphics/scene/IonCamera.h"
#include "graphics/scene/IonDrawableAnimation.h"
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
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::graphics
{
	namespace materials
	{
		struct MaterialManager;
	}

	namespace scene
	{
		class SceneManager;
	}
}

namespace ion::script::interfaces
{
	namespace scene_script_interface::detail
	{
		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_node_animation_class();
		script_validator::ClassDefinition get_node_animation_group_class();
		script_validator::ClassDefinition get_node_animation_timeline_class();
		script_validator::ClassDefinition get_scene_node_class();

		script_validator::ClassDefinition get_camera_class();
		script_validator::ClassDefinition get_drawable_animation_class();
		script_validator::ClassDefinition get_drawable_object_class();
		script_validator::ClassDefinition get_drawable_particle_system_class();
		script_validator::ClassDefinition get_drawable_text_class();
		script_validator::ClassDefinition get_light_class();
		script_validator::ClassDefinition get_model_class();
		script_validator::ClassDefinition get_movable_object_class();
		script_validator::ClassDefinition get_movable_sound_class();
		script_validator::ClassDefinition get_movable_sound_listener_class();

		ScriptValidator get_scene_validator();


		/*
			Tree parsing
		*/
		
		void set_node_animation_properties(const script_tree::ObjectNode &object, graphics::scene::graph::animations::NodeAnimation &animation);
		void set_node_animation_group_properties(const script_tree::ObjectNode &object, graphics::scene::graph::animations::NodeAnimationGroup &animation_group);
		void set_node_animation_timeline_properties(const script_tree::ObjectNode &object, graphics::scene::graph::animations::NodeAnimationTimeline &timeline);
		void set_scene_node_properties(const script_tree::ObjectNode &object, graphics::scene::graph::SceneNode &scene_node);

		void set_camera_properties(const script_tree::ObjectNode &object, graphics::scene::Camera &camera);
		void set_drawable_animation_properties(const script_tree::ObjectNode &object, graphics::scene::DrawableAnimation &animation);
		void set_drawable_object_properties(const script_tree::ObjectNode &object, graphics::scene::DrawableObject &drawable);
		void set_drawable_particle_system_properties(const script_tree::ObjectNode &object, graphics::scene::DrawableParticleSystem &particle_system);
		void set_drawable_text_properties(const script_tree::ObjectNode &object, graphics::scene::DrawableText &text);
		void set_light_properties(const script_tree::ObjectNode &object, graphics::scene::Light &light);
		void set_model_properties(const script_tree::ObjectNode &object, graphics::scene::Model &model);
		void set_movable_object_properties(const script_tree::ObjectNode &object, graphics::scene::MovableObject &movable);
		void set_movable_sound_properties(const script_tree::ObjectNode &object, graphics::scene::MovableSound &sound);
		void set_movable_sound_listener_properties(const script_tree::ObjectNode &object, graphics::scene::MovableSoundListener &sound_listener);

		
		NonOwningPtr<graphics::scene::graph::SceneNode> create_scene_node(const script_tree::ObjectNode &object,
			graphics::scene::graph::SceneNode &scene_node,
			graphics::materials::MaterialManager &material_manager);
		NonOwningPtr<graphics::scene::graph::animations::NodeAnimation> create_node_animation(const script_tree::ObjectNode &object,
			graphics::scene::graph::SceneNode &scene_node);
		NonOwningPtr<graphics::scene::graph::animations::NodeAnimationGroup> create_node_animation_group(const script_tree::ObjectNode &object,
			graphics::scene::graph::SceneNode &scene_node);
		NonOwningPtr<graphics::scene::graph::animations::NodeAnimationTimeline> create_node_animation_timeline(const script_tree::ObjectNode &object,
			graphics::scene::graph::SceneNode &scene_node);

		NonOwningPtr<graphics::scene::Camera> create_camera(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager);
		NonOwningPtr<graphics::scene::DrawableAnimation> create_drawable_animation(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager);
		NonOwningPtr<graphics::scene::DrawableParticleSystem> create_drawable_particle_system(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager);
		NonOwningPtr<graphics::scene::DrawableText> create_drawable_text(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager);
		NonOwningPtr<graphics::scene::Light> create_light(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager);
		NonOwningPtr<graphics::scene::Model> create_model(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager,
			graphics::materials::MaterialManager &material_manager);
		NonOwningPtr<graphics::scene::MovableSound> create_movable_sound(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager);
		NonOwningPtr<graphics::scene::MovableSoundListener> create_movable_sound_listener(const script_tree::ObjectNode &object,
			graphics::scene::SceneManager &scene_manager);

		void create_scene(const ScriptTree &tree,
			graphics::scene::graph::SceneNode &scene_node,
			graphics::scene::SceneManager &scene_manager,
			graphics::materials::MaterialManager &material_manager);
	} //scene_script_interface::detail


	class SceneScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			SceneScriptInterface() = default;


			/*
				Scene
				Creating from script
			*/

			//Create scene from a script (or object file) with the given asset name
			void CreateScene(std::string_view asset_name,
				graphics::scene::graph::SceneNode &scene_node,
				graphics::scene::SceneManager &scene_manager,
				graphics::materials::MaterialManager &material_manager);
	};
} //ion::script::interfaces

#endif