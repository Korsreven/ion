/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonSoundScriptInterface.cpp
-------------------------------------------
*/

#include "IonSoundScriptInterface.h"

#include <optional>
#include <string>

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace sound_script_interface;
using namespace sounds;

namespace sound_script_interface::detail
{

/*
	Validator classes
*/

ClassDefinition get_sound_class()
{
	return ClassDefinition::Create("sound")
		.AddRequiredProperty("asset-name", ParameterType::String)
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("looping-mode", {"forward"s, "bidirectional"s})
		.AddProperty("orientation-mode", {"head"s, "world"s})
		.AddProperty("processing-mode", {"two-dimensional"s, "three-dimensional"s})
		.AddProperty("rolloff-mode", {"linear"s, "linear-square"s, "inverse"s, "inverse-tapered"s})
		.AddProperty("type", {"sample"s, "compressed-sample"s, "stream"s});
}

ScriptValidator get_sound_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_sound_class());
}


/*
	Tree parsing
*/

NonOwningPtr<Sound> create_sound(const script_tree::ObjectNode &object,
	SoundManager &sound_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto asset_name = object
		.Property("asset-name")[0]
		.Get<ScriptType::String>()->Get();
	auto type_name = object
		.Property("type")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto processing_mode_name = object
		.Property("processing-mode")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto orientation_mode_name = object
		.Property("orientation-mode")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto rolloff_mode_name = object
		.Property("rolloff-mode")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto looping_mode_name = object
		.Property("looping-mode")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();

	sound::SoundType type = sound::SoundType::Sample;

	if (type_name == "compressed-sample")
		type = sound::SoundType::CompressedSample;
	else if (type_name == "stream")
		type = sound::SoundType::Stream;

	sound::SoundProcessingMode processing_mode = sound::SoundProcessingMode::TwoDimensional;

	if (processing_mode_name == "three-dimensional")
		processing_mode = sound::SoundProcessingMode::ThreeDimensional;

	sound::SoundOrientationMode orientation_mode = sound::SoundOrientationMode::World;

	if (orientation_mode_name == "head")
		orientation_mode = sound::SoundOrientationMode::Head;

	sound::SoundRolloffMode rolloff_mode = sound::SoundRolloffMode::Inverse;

	if (rolloff_mode_name == "linear")
		rolloff_mode = sound::SoundRolloffMode::Linear;
	else if (rolloff_mode_name == "linear-square")
		rolloff_mode = sound::SoundRolloffMode::LinearSquare;
	else if (rolloff_mode_name == "inverse-tapered")
		rolloff_mode = sound::SoundRolloffMode::InverseTapered;

	std::optional<sound::SoundLoopingMode> looping_mode;

	if (looping_mode_name == "forward")
		looping_mode = sound::SoundLoopingMode::Forward;
	else if (looping_mode_name == "bidirectional")
		looping_mode = sound::SoundLoopingMode::Bidirectional;

	return sound_manager.CreateSound(std::move(name), std::move(asset_name),
		type, processing_mode, orientation_mode, rolloff_mode, looping_mode);
}

void create_sounds(const ScriptTree &tree,
	SoundManager &sound_manager)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "sound")
			create_sound(object, sound_manager);
	}
}

} //sound_script_interface::detail


//Private

ScriptValidator SoundScriptInterface::GetValidator() const
{
	return detail::get_sound_validator();
}


/*
	Sounds
	Creating from script
*/

void SoundScriptInterface::CreateSounds(std::string_view asset_name,
	SoundManager &sound_manager)
{
	if (Load(asset_name))
		detail::create_sounds(*tree_, sound_manager);
}

} //ion::script::interfaces