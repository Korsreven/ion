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

NonOwningPtr<Texture> get_texture(std::string_view name, const ManagerRegister &managers) noexcept
{
	for (auto &texture_manager : managers.ObjectsOf<TextureManager>())
	{
		if (texture_manager)
		{
			if (auto texture = texture_manager->GetTexture(name); texture)
				return texture;
		}
	}

	return nullptr;
}


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
	FrameSequenceManager &frame_sequence_manager, const ManagerRegister &managers)
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
		return frame_sequence_manager.CreateFrameSequence(std::move(name),
			get_texture(first_frame_name, managers), total_frames);
	else //Construct from one or more given frame
	{
		frame_sequence::detail::container_type frames;

		for (auto &property : object.Properties())
		{
			if (property.Name() == "frame")
				frames.push_back(get_texture(property[0].Get<ScriptType::String>()->Get(), managers));
		}

		if (!std::empty(frames))
			return frame_sequence_manager.CreateFrameSequence(std::move(name), frames);
	}
	
	return nullptr;
}

void create_frame_sequences(const ScriptTree &tree,
	FrameSequenceManager &frame_sequence_manager, const ManagerRegister &managers)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "frame-sequence")
			create_frame_sequence(object, frame_sequence_manager, managers);
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
	FrameSequenceManager &frame_sequence_manager)
{
	if (Load(asset_name))
		detail::create_frame_sequences(*tree_, frame_sequence_manager, Managers());
}

void FrameSequenceScriptInterface::CreateFrameSequences(std::string_view asset_name,
	FrameSequenceManager &frame_sequence_manager, const ManagerRegister &managers)
{
	if (Load(asset_name))
		detail::create_frame_sequences(*tree_, frame_sequence_manager, managers);
}

} //ion::script::interfaces