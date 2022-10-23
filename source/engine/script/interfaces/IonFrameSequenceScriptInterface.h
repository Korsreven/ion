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

namespace ion::script::interfaces
{
	namespace frame_sequence_script_interface::detail
	{
		NonOwningPtr<graphics::textures::Texture> get_texture(std::string_view name, const ManagerRegister &managers) noexcept;


		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_frame_sequence_class();
		ScriptValidator get_frame_sequence_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<graphics::textures::FrameSequence> create_frame_sequence(const script_tree::ObjectNode &object,
			graphics::textures::FrameSequenceManager &frame_sequence_manager, const ManagerRegister &managers);

		void create_frame_sequences(const ScriptTree &tree,
			graphics::textures::FrameSequenceManager &frame_sequence_manager, const ManagerRegister &managers);
	} //frame_sequence_script_interface::detail


	//A class representing an interface to a frame sequence script with a complete validation scheme
	//A frame sequence script can load frame sequences from a script file into a frame sequence manager
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

			//Creates frame sequences from a script (or object file) with the given asset name
			void CreateFrameSequences(std::string_view asset_name,
				graphics::textures::FrameSequenceManager &frame_sequence_manager);

			//Creates frame sequences from a script (or object file) with the given asset name
			void CreateFrameSequences(std::string_view asset_name,
				graphics::textures::FrameSequenceManager &frame_sequence_manager, const ManagerRegister &managers);
	};
} //ion::script::interfaces

#endif