/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonSoundScriptInterface.h
-------------------------------------------
*/

#ifndef ION_SOUND_SCRIPT_INTERFACE_H
#define ION_SOUND_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "sounds/IonSound.h"
#include "sounds/IonSoundManager.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"
#include "types/IonTypes.h"

namespace ion::script::interfaces
{
	namespace sound_script_interface::detail
	{
		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_sound_class();
		ScriptValidator get_sound_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<sounds::Sound> create_sound(const script_tree::ObjectNode &object,
			sounds::SoundManager &sound_manager);

		void create_sounds(const ScriptTree &tree,
			sounds::SoundManager &sound_manager);
	} //sound_script_interface::detail


	class SoundScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			SoundScriptInterface() = default;


			/*
				Sounds
				Creating from script
			*/

			//Create sounds from a script (or object file) with the given asset name
			void CreateSounds(std::string_view asset_name,
				sounds::SoundManager &sound_manager);
	};
} //ion::script::interfaces

#endif