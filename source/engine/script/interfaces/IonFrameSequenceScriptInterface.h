/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonFrameSequenceScriptInterface.h
-------------------------------------------
*/

#ifndef ION_FRAME_SEQUENCE_SCRIPT_INTERFACE_H
#define ION_FRAME_SEQUENCE_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "graphics/textures/IonFrameSequence.h"
#include "graphics/textures/IonFrameSequenceManager.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"

//Forward declarations
namespace ion::graphics::textures
{
	class TextureManager;
}

namespace ion::script::interfaces
{
	namespace frame_sequence_script_interface::detail
	{
		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_frame_sequence_class();
		ScriptValidator get_frame_sequence_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<graphics::textures::FrameSequence> create_frame_sequence(const script_tree::ObjectNode &object,
			graphics::textures::FrameSequenceManager &frame_sequence_manager,
			graphics::textures::TextureManager &texture_manager);

		void create_frame_sequences(const ScriptTree &tree,
			graphics::textures::FrameSequenceManager &frame_sequence_manager,
			graphics::textures::TextureManager &texture_manager);
	} //frame_sequence_script_interface::detail


	class FrameSequenceScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			FrameSequenceScriptInterface() = default;


			/*
				Frame sequences
				Creating from script
			*/

			//Create frame sequences from a script (or object file) with the given asset name
			void CreateFrameSequences(std::string_view asset_name,
				graphics::textures::FrameSequenceManager &frame_sequence_manager,
				graphics::textures::TextureManager &texture_manager);
	};
} //ion::script::interfaces

#endif