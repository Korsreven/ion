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
		.AddClass(get_sound_channel_class())

		.AddRequiredProperty("asset-name", ParameterType::String)
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("distance", {ParameterType::FloatingPoint, ParameterType::FloatingPoint}, 1)
		.AddProperty("looping-mode", {"forward"s, "bidirectional"s})
		.AddProperty("orientation-mode", {"head"s, "world"s})
		.AddProperty("processing-mode", {"two-dimensional"s, "three-dimensional"s})
		.AddProperty("rolloff-mode", {"linear"s, "linear-square"s, "inverse"s, "inverse-tapered"s})
		.AddProperty("type", {"sample"s, "compressed-sample"s, "stream"s});
}

ClassDefinition get_sound_channel_class()
{
	return ClassDefinition::Create("sound-channel")
		.AddProperty("attributes", {ParameterType::Vector3, ParameterType::Vector3})
		.AddProperty("distance", {ParameterType::FloatingPoint, ParameterType::FloatingPoint}, 1)
		.AddProperty("mute", ParameterType::Boolean)
		.AddProperty("paused", ParameterType::Boolean)
		.AddProperty("pitch", ParameterType::FloatingPoint)
		.AddProperty("sound-channel-group", ParameterType::String)
		.AddProperty("volume", ParameterType::FloatingPoint);
}

ClassDefinition get_sound_channel_group_class()
{
	return ClassDefinition::Create("sound-channel-group")
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("mute", ParameterType::Boolean)
		.AddProperty("pitch", ParameterType::FloatingPoint)
		.AddProperty("volume", ParameterType::FloatingPoint);
}

ClassDefinition get_sound_listener_class()
{
	return ClassDefinition::Create("sound-listener")
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("attributes", {ParameterType::Vector3, ParameterType::Vector3});
}


ScriptValidator get_sound_validator()
{
	return ScriptValidator::Create()
		.AddClass(get_sound_class())
		.AddClass(get_sound_channel_group_class())
		.AddClass(get_sound_listener_class());
}


/*
	Tree parsing
*/

void set_sound_properties(const script_tree::ObjectNode &object, Sound &sound)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "distance")
		{
			if (property.NumberOfArguments() == 2)
				sound.Distance(property[0].Get<ScriptType::FloatingPoint>()->As<real>(),
							   property[1].Get<ScriptType::FloatingPoint>()->As<real>());
			else
				sound.Distance(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		}
	}

	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "sound-channel")
		{
			auto sound_channel_group_name = obj
				.Property("sound-channel-group")[0]
				.Get<ScriptType::String>().value_or(""s).Get();
			auto paused = obj
				.Property("paused")[0]
				.Get<ScriptType::Boolean>().value_or(false).Get();

			if (auto sound_channel =
				[&]() noexcept
				{
					if (auto owner = sound.Owner(); owner &&
						(sound.IsLoaded() || owner->Load(sound)))

						return sound.Play(owner->GetSoundChannelGroup(sound_channel_group_name), paused);
					else
						return sound.Play(paused);
				}(); sound_channel)

				set_sound_channel_properties(obj, *sound_channel);
		}
	}
}

void set_sound_channel_properties(const script_tree::ObjectNode &object, SoundChannel &sound_channel)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "attributes")
			sound_channel.Attributes(property[0].Get<ScriptType::Vector3>()->Get(),
									 property[1].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "distance")
		{
			if (property.NumberOfArguments() == 2)
				sound_channel.Distance(property[0].Get<ScriptType::FloatingPoint>()->As<real>(),
									   property[1].Get<ScriptType::FloatingPoint>()->As<real>());
			else
				sound_channel.Distance(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		}
		else if (property.Name() == "mute")
			sound_channel.Mute(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "pitch")
			sound_channel.Pitch(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "volume")
			sound_channel.Volume(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
	}
}

void set_sound_channel_group_properties(const script_tree::ObjectNode &object, SoundChannelGroup &sound_channel_group)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "mute")
			sound_channel_group.Mute(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "pitch")
			sound_channel_group.Pitch(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "volume")
			sound_channel_group.Volume(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
	}
}

void set_sound_listener_properties(const script_tree::ObjectNode &object, SoundListener &sound_listener)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "attributes")
			sound_listener.Attributes(property[0].Get<ScriptType::Vector3>()->Get(),
									  property[1].Get<ScriptType::Vector3>()->Get());
	}
}


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

	auto sound = sound_manager.CreateSound(std::move(name), std::move(asset_name),
		type, processing_mode, orientation_mode, rolloff_mode, looping_mode);

	if (sound)
		set_sound_properties(object, *sound);

	return sound;
}

NonOwningPtr<SoundChannelGroup> create_sound_channel_group(const script_tree::ObjectNode &object,
	SoundManager &sound_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto sound_channel_group = sound_manager.CreateSoundChannelGroup(std::move(name));

	if (sound_channel_group)
		set_sound_channel_group_properties(object, *sound_channel_group);

	return sound_channel_group;
}

NonOwningPtr<SoundListener> create_sound_listener(const script_tree::ObjectNode &object,
	SoundManager &sound_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto sound_listener = sound_manager.CreateSoundListener(std::move(name));

	if (sound_listener)
		set_sound_listener_properties(object, *sound_listener);

	return sound_listener;
}


void create_sounds(const ScriptTree &tree,
	SoundManager &sound_manager)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "sound")
			create_sound(object, sound_manager);
		else if (object.Name() == "sound-channel-group")
			create_sound_channel_group(object, sound_manager);
		else if (object.Name() == "sound-listener")
			create_sound_listener(object, sound_manager);
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