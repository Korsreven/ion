/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	sounds
File:	IonSound.cpp
-------------------------------------------
*/

#include "IonSound.h"

#include "IonSoundManager.h"
#include "Fmod/fmod.hpp"

namespace ion::sounds
{

using namespace sound;

namespace sound::detail
{
} //sound::detail


//Protected

/*
	Events
*/

void Sound::Removed(SoundChannel &sound_channel) noexcept
{
	if (auto handle = sound_channel.Handle(); handle)
		sound_manager::detail::stop(*handle);
}


//Public

Sound::Sound(std::string name, std::string asset_name,
	SoundType type, SoundProcessingMode processing_mode,
	SoundOrientationMode orientation_mode, SoundRolloffMode rolloff_mode,
	std::optional<SoundLoopingMode> looping_mode) noexcept :

	FileResource{std::move(name), std::move(asset_name)},

	type_{type},
	processing_mode_{processing_mode},
	orientation_mode_{orientation_mode},
	rolloff_mode_{rolloff_mode},
	looping_mode_{looping_mode}
{
	//Empty
}

Sound::Sound(std::string name, std::string asset_name,
	SoundType type, SoundProcessingMode processing_mode,
	std::optional<SoundLoopingMode> looping_mode) noexcept :

	FileResource{std::move(name), std::move(asset_name)},

	type_{type},
	processing_mode_{processing_mode},
	looping_mode_{looping_mode}
{
	//Empty
}

Sound::Sound(std::string name, std::string asset_name,
	SoundType type, std::optional<SoundLoopingMode> looping_mode) noexcept :

	FileResource{std::move(name), std::move(asset_name)},

	type_{type},
	looping_mode_{looping_mode}
{
	//Empty
}


Sound::~Sound() noexcept
{
	ClearSoundChannels();
		//Clears all sound channels before sound is destroyed
		//Virtual functions cannot be called post destruction
}


/*
	Static sound conversions
*/

Sound Sound::NonPositional(std::string name, std::string asset_name,
	SoundType type, std::optional<SoundLoopingMode> looping_mode)
{
	return {std::move(name), std::move(asset_name),
			type, SoundProcessingMode::TwoDimensional,
			looping_mode};
}


Sound Sound::Positional(std::string name, std::string asset_name,
	SoundType type, SoundOrientationMode orientation_mode, SoundRolloffMode rolloff_mode,
	std::optional<SoundLoopingMode> looping_mode)
{
	return {std::move(name), std::move(asset_name),
			type, SoundProcessingMode::ThreeDimensional,
			orientation_mode, rolloff_mode, looping_mode};
}

Sound Sound::Positional(std::string name, std::string asset_name,
	SoundType type, std::optional<SoundLoopingMode> looping_mode)
{
	return {std::move(name), std::move(asset_name),
			type, SoundProcessingMode::ThreeDimensional,
			looping_mode};
}


/*
	Modifiers
*/

void Sound::Distance(real min_distance, real max_distance) noexcept
{
	if (handle_)
		sound_manager::detail::set_min_max_distance(*handle_, min_distance, max_distance);
}


/*
	Observers
*/

std::optional<std::pair<real, real>> Sound::Distance() const noexcept
{
	if (handle_)
		return sound_manager::detail::get_min_max_distance(*handle_);
	else
		return {};
}


/*
	Sound channels
	Creating
*/

NonOwningPtr<SoundChannel> Sound::Play(bool paused)
{
	return Play(Create(), paused);
}

NonOwningPtr<SoundChannel> Sound::Play(NonOwningPtr<SoundChannelGroup> sound_channel_group, bool paused)
{
	return Play(Create(sound_channel_group), paused);
}

NonOwningPtr<SoundChannel> Sound::Play(NonOwningPtr<SoundChannel> sound_channel, bool paused)
{
	if (!sound_channel)
		return Play(paused);

	if (handle_)
	{
		if (auto system = sound_manager::detail::get_system(*handle_); system)
			sound_channel->Handle(
				sound_manager::detail::play_sound(
					*system, *handle_,
					sound_channel->CurrentChannelGroup() ? sound_channel->CurrentChannelGroup()->Handle() : nullptr,
					sound_channel->Handle(),
					paused)
			);
	}

	return sound_channel;
}


/*
	Sound channels
	Removing
*/

void Sound::ClearSoundChannels() noexcept
{
	Clear();
}

bool Sound::RemoveSoundChannel(SoundChannel &sound_channel) noexcept
{
	return Remove(sound_channel);
}

} //ion::sounds