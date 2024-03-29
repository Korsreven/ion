/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonAnimationScriptInterface.h
-------------------------------------------
*/

#ifndef ION_ANIMATION_SCRIPT_INTERFACE_H
#define ION_ANIMATION_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "graphics/textures/IonAnimation.h"
#include "graphics/textures/IonAnimationManager.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"

namespace ion::script::interfaces
{
	namespace animation_script_interface::detail
	{
		NonOwningPtr<graphics::textures::FrameSequence> get_frame_sequence(std::string_view name, const ManagerRegister &managers) noexcept;


		/**
			@name Validator classes
			@{
		*/

		script_validator::ClassDefinition get_animation_class();
		ScriptValidator get_animation_validator();

		///@}

		/**
			@name Tree parsing
			@{
		*/

		void set_animation_properties(const script_tree::ObjectNode &object, graphics::textures::Animation &animation);


		NonOwningPtr<graphics::textures::Animation> create_animation(const script_tree::ObjectNode &object,
			graphics::textures::AnimationManager &animation_manager, const ManagerRegister &managers);

		void create_animations(const ScriptTree &tree,
			graphics::textures::AnimationManager &animation_manager, const ManagerRegister &managers);

		///@}
	} //animation_script_interface::detail


	///@brief A class representing an interface to an animation script with a complete validation scheme
	///@details An animation script can load animations from a script file into an animation manager
	class AnimationScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			///@brief Default constructor
			AnimationScriptInterface() = default;


			/**
				@name Animations
				Creating from script
				@{
			*/

			///@brief Creates animations from a script (or object file) with the given asset name
			void CreateAnimations(std::string_view asset_name,
				graphics::textures::AnimationManager &animation_manager);

			///@brief Creates animations from a script (or object file) with the given asset name
			void CreateAnimations(std::string_view asset_name,
				graphics::textures::AnimationManager &animation_manager, const ManagerRegister &managers);

			///@}
	};
} //ion::script::interfaces

#endif