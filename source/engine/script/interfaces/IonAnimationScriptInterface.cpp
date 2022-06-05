/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonAnimationScriptInterface.cpp
-------------------------------------------
*/

#include "IonAnimationScriptInterface.h"

#include <string>
#include "graphics/textures/IonFrameSequenceManager.h"
#include "types/IonTypes.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace animation_script_interface;
using namespace graphics::textures;

namespace animation_script_interface::detail
{

/*
	Validator classes
*/

ClassDefinition get_animation_class()
{
	return ClassDefinition::Create("animation")
		.AddRequiredProperty("cycle-duration", ParameterType::FloatingPoint)
		.AddRequiredProperty("frame-sequence", ParameterType::String)
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("cycle-percent", ParameterType::FloatingPoint)
		.AddProperty("cycle-time", ParameterType::FloatingPoint)
		.AddProperty("direction", {"normal"s, "reverse"s, "alternate"s, "alternate-reverse"s})
		.AddProperty("frame-rate", ParameterType::FloatingPoint)
		.AddProperty("jump-backward", ParameterType::FloatingPoint)
		.AddProperty("jump-forward", ParameterType::FloatingPoint)
		.AddProperty("playback-rate", ParameterType::FloatingPoint)
		.AddProperty("repeat-count", ParameterType::Integer)
		.AddProperty("running", ParameterType::Boolean)
		.AddProperty("total-duration", ParameterType::FloatingPoint)
		.AddProperty("total-percent", ParameterType::FloatingPoint)
		.AddProperty("total-time", ParameterType::FloatingPoint);
}

ScriptValidator get_animation_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_animation_class());
}


/*
	Tree parsing
*/

NonOwningPtr<Animation> create_animation(const script_tree::ObjectNode &object,
	AnimationManager &animation_manager,
	FrameSequenceManager &frame_sequence_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto frame_sequence_name = object
		.Property("frame-sequence")[0]
		.Get<ScriptType::String>()->Get();
	auto cycle_duration = duration{object
		.Property("cycle-duration")[0]
		.Get<ScriptType::FloatingPoint>()->As<real>()};

	auto animation = animation_manager.CreateAnimation(std::move(name),
		frame_sequence_manager.GetFrameSequence(frame_sequence_name), cycle_duration);

	if (animation)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "cycle-percent")
				animation->CyclePercent(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
			else if (property.Name() == "cycle-time")
				animation->CycleTime(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
			else if (property.Name() == "direction")
			{
				if (property[0].Get<ScriptType::Enumerable>()->Get() == "normal")
					animation->Direction(animation::PlaybackDirection::Normal);
				else if (property[0].Get<ScriptType::Enumerable>()->Get() == "reverse")
					animation->Direction(animation::PlaybackDirection::Reverse);
				else if (property[0].Get<ScriptType::Enumerable>()->Get() == "alternate")
					animation->Direction(animation::PlaybackDirection::Alternate);
				else if (property[0].Get<ScriptType::Enumerable>()->Get() == "alternate-reverse")
					animation->Direction(animation::PlaybackDirection::AlternateReverse);
			}
			else if (property.Name() == "frame-rate")
				animation->FrameRate(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
			else if (property.Name() == "jump-backward")
				animation->JumpBackward(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
			else if (property.Name() == "jump-forward")
				animation->JumpForward(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
			else if (property.Name() == "playback-rate")
				animation->PlaybackRate(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
			else if (property.Name() == "repeat-count")
				animation->RepeatCount(property[0].Get<ScriptType::Integer>()->As<int>());
			else if (property.Name() == "running")
			{
				if (property[0].Get<ScriptType::Boolean>()->Get())
					animation->Start();
				else
					animation->Stop();
			}
			else if (property.Name() == "total-duration")
				animation->TotalDuration(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
			else if (property.Name() == "total-percent")
				animation->TotalPercent(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
			else if (property.Name() == "total-time")
				animation->TotalTime(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
		}
	}

	return animation;
}

void create_animations(const ScriptTree &tree,
	AnimationManager &animation_manager,
	FrameSequenceManager &frame_sequence_manager)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "animation")
			create_animation(object, animation_manager, frame_sequence_manager);
	}
}

} //animation_script_interface::detail


//Private

ScriptValidator AnimationScriptInterface::GetValidator() const
{
	return detail::get_animation_validator();
}


/*
	Animations
	Creating from script
*/

void AnimationScriptInterface::CreateAnimations(std::string_view asset_name,
	AnimationManager &animation_manager,
	FrameSequenceManager &frame_sequence_manager)
{
	if (Load(asset_name))
		detail::create_animations(*tree_, animation_manager, frame_sequence_manager);
}

} //ion::script::interfaces