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

namespace ion::sounds
{

using namespace sound;

namespace sound::detail
{
} //sound::detail


Sound::Sound(std::string name, std::string asset_name,
	SoundType type, SoundProcessingMode processing_mode,
	SoundOrientationMode orientation_mode, SoundRolloffMode rolloff_mode,
	std::optional<SoundLoopingMode> looping_mode) :

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
	std::optional<SoundLoopingMode> looping_mode) :

	FileResource{std::move(name), std::move(asset_name)},

	type_{type},
	processing_mode_{processing_mode},
	looping_mode_{looping_mode}
{
	//Empty
}

Sound::Sound(std::string name, std::string asset_name,
	SoundType type, std::optional<SoundLoopingMode> looping_mode) :

	FileResource{std::move(name), std::move(asset_name)},

	type_{type},
	looping_mode_{looping_mode}
{
	//Empty
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
	Creating
	Sound channel
*/

NonOwningPtr<SoundChannel> Sound::Play(bool paused) noexcept
{
	return nullptr;
}

NonOwningPtr<SoundChannel> Sound::Play(SoundChannelGroup &channel_group, bool paused) noexcept
{
	return nullptr;
}

} //ion::sounds