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

//Forward declarations
namespace ion::graphics
{
	namespace textures
	{
		struct FrameSequenceManager;
	}
}

namespace ion::script::interfaces
{
	namespace animation_script_interface::detail
	{
		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_animation_class();
		ScriptValidator get_animation_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<graphics::textures::Animation> create_animation(const script_tree::ObjectNode &object,
			graphics::textures::AnimationManager &animation_manager,
			graphics::textures::FrameSequenceManager &frame_sequence_manager);
	} //animation_script_interface::detail


	class AnimationScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			AnimationScriptInterface() = default;


			/*
				Animations
				Creating from script
			*/

			//Create animations from a script (or object file) with the given asset name
			void CreateAnimations(std::string_view asset_name,
				graphics::textures::AnimationManager &animation_manager,
				graphics::textures::FrameSequenceManager &frame_sequence_manager);
	};
} //ion::script::interfaces

#endif