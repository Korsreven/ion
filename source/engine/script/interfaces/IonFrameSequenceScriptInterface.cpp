/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonFrameSequenceScriptInterface.cpp
-------------------------------------------
*/

#include "IonFrameSequenceScriptInterface.h"

#include <string>
#include "graphics/textures/IonTextureManager.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace frame_sequence_script_interface;
using namespace graphics::textures;

namespace frame_sequence_script_interface::detail
{

/*
	Validator classes
*/

ClassDefinition get_frame_sequence_class()
{
	return ClassDefinition::Create("frame-sequence")
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("frame", ParameterType::String)
		.AddProperty("frames", {ParameterType::String, ParameterType::Integer});
}

ScriptValidator get_frame_sequence_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_frame_sequence_class());
}


/*
	Tree parsing
*/

NonOwningPtr<FrameSequence> create_frame_sequence(const script_tree::ObjectNode &object,
	FrameSequenceManager &frame_sequence_manager,
	TextureManager &texture_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto first_frame_name = object
		.Property("frames")[0]
		.Get<ScriptType::String>().value_or(""s).Get();
	auto total_frames = object
		.Property("frames")[1]
		.Get<ScriptType::Integer>().value_or(0).As<int>();

	//Construct from first frame and total frames
	if (total_frames > 0)
		return frame_sequence_manager.CreateFrameSequence(name, texture_manager.GetTexture(first_frame_name), total_frames);
	else //Construct from one or more given frame
	{
		frame_sequence::detail::container_type frames;

		for (auto &property : object.Properties())
		{
			if (property.Name() == "frame")
				frames.push_back(texture_manager.GetTexture(property[0].Get<ScriptType::String>()->Get()));
		}

		if (!std::empty(frames))
			return frame_sequence_manager.CreateFrameSequence(name, frames);
	}
	
	return nullptr;
}

void create_frame_sequences(const ScriptTree &tree,
	FrameSequenceManager &frame_sequence_manager,
	TextureManager &texture_manage)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "frame-sequence")
			create_frame_sequence(object, frame_sequence_manager, texture_manage);
	}
}

} //frame_sequence_script_interface::detail


//Private

ScriptValidator FrameSequenceScriptInterface::GetValidator() const
{
	return detail::get_frame_sequence_validator();
}


/*
	Frame sequences
	Creating from script
*/

void FrameSequenceScriptInterface::CreateFrameSequences(std::string_view asset_name,
	FrameSequenceManager &frame_sequence_manager,
	TextureManager &texture_manage)
{
	if (Load(asset_name))
		detail::create_frame_sequences(*tree_, frame_sequence_manager, texture_manage);
}

} //ion::script::interfaces