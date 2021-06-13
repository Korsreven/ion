/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	sounds
File:	IonSoundManager.cpp
-------------------------------------------
*/

#include "IonSoundManager.h"

#include "Fmod/fmod.hpp"
#include "types/IonTypes.h"

namespace ion::sounds
{

using namespace sound_manager;
using namespace types::type_literals;

namespace sound_manager::detail
{

FMOD::System* init_sound_system() noexcept
{
	if (FMOD::System *system = nullptr;
		FMOD::System_Create(&system) == FMOD_OK &&
		system->init(max_sound_channels, FMOD_INIT_NORMAL, nullptr) == FMOD_OK)
		
		return system;
	else
	{
		release_sound_system(system);
		return nullptr;
	}
}

void release_sound_system(FMOD::System *system) noexcept
{
	if (system)
	{
		system->release();
		system = nullptr;
	}
}

void update_sound_system(FMOD::System &system) noexcept
{
	system.update();
}


FMOD::Sound* load_sound(
	FMOD::System &system,
	const std::string &file_data, const std::optional<std::string> &stream_data,
	sound::SoundType type, sound::SoundProcessingMode processing_mode,
	sound::SoundOrientationMode orientation_mode, sound::SoundRolloffMode rolloff_mode,
	const std::optional<sound::SoundLoopingMode> &looping_mode) noexcept
{
	FMOD_CREATESOUNDEXINFO ex_info{};
	ex_info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO); //Required
	ex_info.length = std::size(file_data);

	FMOD_MODE mode =
		FMOD_OPENMEMORY |
		FMOD_LOWMEM | //Save 256 bytes per sound, by disabling some unneeded functionality
		[&]() noexcept
		{
			switch (type)
			{
				case sound::SoundType::Stream:
				return FMOD_CREATESTREAM;

				case sound::SoundType::CompressedSample:
				return FMOD_CREATECOMPRESSEDSAMPLE;

				case sound::SoundType::Sample:
				default:
				return FMOD_CREATESAMPLE;
			}
		}() |
		[&]() noexcept
		{
			switch (processing_mode)
			{
				case sound::SoundProcessingMode::ThreeDimensional:
				return FMOD_3D |
					[&]() noexcept
					{
						switch (orientation_mode)
						{
							case sound::SoundOrientationMode::Head:
							return FMOD_3D_HEADRELATIVE;

							case sound::SoundOrientationMode::World:
							default:
							return FMOD_3D_WORLDRELATIVE;
						}
					}() |
					[&]() noexcept
					{
						switch (rolloff_mode)
						{
							case sound::SoundRolloffMode::Linear:
							return FMOD_3D_LINEARROLLOFF;

							case sound::SoundRolloffMode::LinearSquare:
							return FMOD_3D_LINEARSQUAREROLLOFF;

							case sound::SoundRolloffMode::InverseTapered:
							return FMOD_3D_INVERSETAPEREDROLLOFF;

							case sound::SoundRolloffMode::Inverse:
							default:
							return FMOD_3D_INVERSEROLLOFF;
						}
					}();

				case sound::SoundProcessingMode::TwoDimensional:
				default:
				return FMOD_2D;
			}
		}() |
		[&]() noexcept
		{
			if (looping_mode)
			{
				switch (*looping_mode)
				{
					case sound::SoundLoopingMode::Bidirectional:
					return FMOD_LOOP_BIDI;

					case sound::SoundLoopingMode::Forward:
					return FMOD_LOOP_NORMAL;
				}
			}
			
			return FMOD_LOOP_OFF;
		}();


	if (FMOD::Sound *sound_handle = nullptr;
		system.createSound(stream_data ? std::data(*stream_data) : std::data(file_data), mode, &ex_info, &sound_handle) == FMOD_OK)

		return sound_handle;
	else
	{
		unload_sound(sound_handle);
		return nullptr;
	}
}

void unload_sound(FMOD::Sound *sound_handle) noexcept
{
	if (sound_handle)
		sound_handle->release();
}


FMOD::Channel* play_sound(
	FMOD::System &system, FMOD::Sound &sound,
	FMOD::ChannelGroup *channel_group, FMOD::Channel *channel,
	bool paused)
{
	if (system.playSound(&sound, channel_group, paused, &channel) == FMOD_OK && channel)
		return channel;
	else
		return nullptr;
}


FMOD::ChannelGroup* create_channel_group(FMOD::System &system) noexcept
{
	if (FMOD::ChannelGroup *channel_group = nullptr;
		system.createChannelGroup(nullptr, &channel_group) == FMOD_OK)
		
		return channel_group;
	else
		return nullptr;
}

void release_channel_group(FMOD::ChannelGroup *channel_group) noexcept
{
	if (channel_group)
		channel_group->release();
}


FMOD::System* get_system(FMOD::Sound &sound) noexcept
{	
	if (FMOD::System *system = nullptr;
		sound.getSystemObject(&system) == FMOD_OK)

		return system;
	else
		return nullptr;
}

FMOD::ChannelGroup* get_master_channel_group(FMOD::System &system) noexcept
{
	if (FMOD::ChannelGroup *channel_group = nullptr;
		system.getMasterChannelGroup(&channel_group) == FMOD_OK)

		return channel_group;
	else
		return nullptr;
}

void set_channel_group(FMOD::Channel &channel, FMOD::ChannelGroup *group) noexcept
{
	channel.setChannelGroup(group);
}


void stop(FMOD::ChannelControl &control) noexcept
{
	control.stop();
}

void set_mute(FMOD::ChannelControl &control, bool mute) noexcept
{
	control.setMute(mute);
}

void set_paused(FMOD::ChannelControl &control, bool paused) noexcept
{
	control.setPaused(paused);
}

void set_pitch(FMOD::ChannelControl &control, real pitch) noexcept
{
	control.setPitch(pitch);
}

void set_volume(FMOD::ChannelControl &control, real volume) noexcept
{
	control.setVolume(volume);
}


bool get_mute(FMOD::ChannelControl &control) noexcept
{
	auto mute = true;
	control.getMute(&mute);
	return mute;
}

real get_pitch(FMOD::ChannelControl &control) noexcept
{
	auto pitch = 1.0_r;
	control.getPitch(&pitch);
	return pitch;
}

real get_volume(FMOD::ChannelControl &control) noexcept
{
	auto volume = 0.0_r;
	control.getVolume(&volume);
	return volume;
}

bool is_playing(FMOD::ChannelControl &control) noexcept
{
	auto playing = false;
	control.isPlaying(&playing);
	return playing;
}


void set_position(FMOD::Channel &channel, int position) noexcept
{
	channel.setPosition(position, FMOD_TIMEUNIT_MS);
}


/*
	Positional (3D) functionality
*/

void set_settings(FMOD::System &system, real doppler_scale, real distance_factor, real rolloff_scale) noexcept
{
	system.set3DSettings(doppler_scale, distance_factor, rolloff_scale);
}


void set_listener_attributes(FMOD::System &system,
	const graphics::utilities::Vector3 &position, const graphics::utilities::Vector3 &velocity) noexcept
{
	auto [x, y, z] = position.XYZ();
	auto [vx, vy, vz] = velocity.XYZ();

	FMOD_VECTOR pos{x, z, y};
	FMOD_VECTOR vel{vx, vz, vy};
	FMOD_VECTOR forward{0.0_r, 0.0_r, 1.0_r};
	FMOD_VECTOR up{0.0_r, 1.0_r, 0.0_r};
	system.set3DListenerAttributes(0, &pos, &vel, &forward, &up);
}

std::pair<graphics::utilities::Vector3, graphics::utilities::Vector3> get_listener_attributes(FMOD::System &system) noexcept
{
	FMOD_VECTOR pos;
	FMOD_VECTOR vel;
	FMOD_VECTOR forward;
	FMOD_VECTOR up;
	system.get3DListenerAttributes(0, &pos, &vel, &forward, &up);
	return {graphics::utilities::Vector3{pos.x, pos.z, pos.y},
			graphics::utilities::Vector3{vel.x, vel.z, vel.y}};
}


void set_attributes(FMOD::Channel &channel,
	const graphics::utilities::Vector3 &position, const graphics::utilities::Vector3 &velocity) noexcept
{
	auto [x, y, z] = position.XYZ();
	auto [vx, vy, vz] = velocity.XYZ();

	FMOD_VECTOR pos{x, z, y};
	FMOD_VECTOR vel{vx, vz, vy};
	channel.set3DAttributes(&pos, &vel);
}

void set_min_max_distance(FMOD::Sound &sound, real min_distance, real max_distance) noexcept
{
	sound.set3DMinMaxDistance(min_distance, max_distance);
}

void set_min_max_distance(FMOD::Channel &channel, real min_distance, real max_distance) noexcept
{
	channel.set3DMinMaxDistance(min_distance, max_distance);
}


std::tuple<real, real, real> get_settings(FMOD::System &system) noexcept
{
	float doppler_scale = 1.0f;
	float distance_factor = 1.0f;
	float rolloff_scale = 1.0f;
	system.get3DSettings(&doppler_scale, &distance_factor, &rolloff_scale);
	return {doppler_scale, distance_factor, rolloff_scale};
}

std::pair<graphics::utilities::Vector3, graphics::utilities::Vector3> get_attributes(FMOD::Channel &channel) noexcept
{
	FMOD_VECTOR pos;
	FMOD_VECTOR vel;
	channel.get3DAttributes(&pos, &vel);
	return {graphics::utilities::Vector3{pos.x, pos.z, pos.y},
			graphics::utilities::Vector3{vel.x, vel.z, vel.y}};
}

std::pair<real, real> get_min_max_distance(FMOD::Sound &sound) noexcept
{
	float min_distance = 1.0f;
	float max_distance = 10'000.0f;
	sound.get3DMinMaxDistance(&min_distance, &max_distance);
	return {min_distance, max_distance};
}

std::pair<real, real> get_min_max_distance(FMOD::Channel &channel) noexcept
{
	float min_distance = 1.0f;
	float max_distance = 10'000.0f;
	channel.get3DMinMaxDistance(&min_distance, &max_distance);
	return {min_distance, max_distance};
}

} //sound_manager::detail


//Protected

/*
	Events
*/

bool SoundManager::PrepareResource(Sound &sound) noexcept
{
	if (FileResourceManager::PrepareResource(sound))
	{
		if (sound.Type() == sound::SoundType::Stream)
		{
			if (sound.FileData())
				sound.StreamData(*sound.FileData());

			return sound.StreamData().has_value();
		}
		else
			return true;
	}
	else
		return false;
}

bool SoundManager::LoadResource(Sound &sound) noexcept
{
	auto &file_data = sound.FileData();
	auto &stream_data = sound.StreamData();

	auto type = sound.Type();
	auto processing_mode = sound.ProcessingMode();
	auto orientation_mode = sound.OrientationMode();
	auto rolloff_mode = sound.RolloffMode();
	auto &looping_mode = sound.LoopingMode();

	if (sound_system_ && file_data)
	{
		sound.Handle(
			detail::load_sound(
				*sound_system_,
				*file_data, stream_data, type, processing_mode, orientation_mode, rolloff_mode, looping_mode
			));
		return sound.Handle();
	}
	else
		return false;
}

bool SoundManager::UnloadResource(Sound &sound) noexcept
{
	if (auto handle = sound.Handle(); handle)
	{
		detail::unload_sound(handle);
		sound.Handle(nullptr);
		return true;
	}
	else
		return false;
}


void SoundManager::ResourceLoaded(Sound &sound) noexcept
{
	FileResourceManager::ResourceLoaded(sound);
}

void SoundManager::ResourceUnloaded(Sound &sound) noexcept
{
	FileResourceManager::ResourceUnloaded(sound);
	sound.ResetStreamData();
		//Stream data not required after sound has failed (save memory)
}

void SoundManager::ResourceFailed(Sound &sound) noexcept
{
	FileResourceManager::ResourceFailed(sound);
	sound.ResetStreamData();
		//Stream data not required after sound has failed (save memory)
}


void SoundManager::Created(SoundChannelGroup &sound_channel_group) noexcept
{
	if (sound_system_)
		sound_channel_group.Handle(detail::create_channel_group(*sound_system_));
}

void SoundManager::Created(SoundListener &sound_listener) noexcept
{
	if (sound_system_)
		sound_listener.Handle(sound_system_);
}


void SoundManager::Removed(SoundChannelGroup &sound_channel_group) noexcept
{
	detail::release_channel_group(sound_channel_group.Handle());
	sound_channel_group.Handle(nullptr);
}

void SoundManager::Removed(SoundListener &sound_listener) noexcept
{
	sound_listener.Handle(nullptr);
}


//Public

SoundManager::SoundManager() noexcept :
	sound_system_{detail::init_sound_system()}
{
	//Empty
}

SoundManager::~SoundManager() noexcept
{
	UnloadAll();
		//Unload all resources before this manager is destroyed
		//Virtual functions cannot be called post destruction

	detail::release_sound_system(sound_system_);
}


/*
	Modifiers
*/

void SoundManager::Mute(bool mute) noexcept
{
	if (sound_system_)
	{
		if (auto channel_group = detail::get_master_channel_group(*sound_system_); channel_group)
			detail::set_mute(*channel_group, mute);
	}
}

void SoundManager::Pitch(real pitch) noexcept
{
	if (sound_system_)
	{
		if (auto channel_group = detail::get_master_channel_group(*sound_system_); channel_group)
			detail::set_pitch(*channel_group, pitch);
	}
}

void SoundManager::Volume(real volume) noexcept
{
	if (sound_system_)
	{
		if (auto channel_group = detail::get_master_channel_group(*sound_system_); channel_group)
			detail::set_volume(*channel_group, volume);
	}
}


void SoundManager::Settings(real doppler_scale, real distance_factor, real rolloff_scale) noexcept
{
	if (sound_system_)
		return detail::set_settings(*sound_system_, doppler_scale, distance_factor, rolloff_scale);
}


/*
	Observers
*/

bool SoundManager::IsMuted() const noexcept
{
	if (sound_system_)
	{
		if (auto channel_group = detail::get_master_channel_group(*sound_system_); channel_group)
			return detail::get_mute(*channel_group);
	}
	
	return false;
}

real SoundManager::Pitch() const noexcept
{
	if (sound_system_)
	{
		if (auto channel_group = detail::get_master_channel_group(*sound_system_); channel_group)
			return detail::get_pitch(*channel_group);
	}

	return 1.0_r;
}

real SoundManager::Volume() const noexcept
{
	if (sound_system_)
	{
		if (auto channel_group = detail::get_master_channel_group(*sound_system_); channel_group)
			return detail::get_volume(*channel_group);
	}

	return 0.0_r;
}


std::tuple<real, real, real> SoundManager::Settings() const noexcept
{
	if (sound_system_)
		return detail::get_settings(*sound_system_);
	else
		return {1.0_r, 1.0_r, 1.0_r};
}


/*
	Updating
*/

void SoundManager::Update() noexcept
{
	if (sound_system_)
		detail::update_sound_system(*sound_system_);
}


/*
	Sounds
	Creating
*/

NonOwningPtr<Sound> SoundManager::CreateSound(std::string name, std::string asset_name)
{
	return CreateResource(std::move(name), std::move(asset_name));
}

NonOwningPtr<Sound> SoundManager::CreateSound(std::string name, std::string asset_name,
	sound::SoundType type, sound::SoundProcessingMode processing_mode,
	sound::SoundOrientationMode orientation_mode, sound::SoundRolloffMode rolloff_mode,
	std::optional<sound::SoundLoopingMode> looping_mode)
{
	return CreateResource(std::move(name), std::move(asset_name),
						  type, processing_mode, orientation_mode, rolloff_mode,
						  looping_mode);
}

NonOwningPtr<Sound> SoundManager::CreateSound(std::string name, std::string asset_name,
	sound::SoundType type, sound::SoundProcessingMode processing_mode,
	std::optional<sound::SoundLoopingMode> looping_mode)
{
	return CreateResource(std::move(name), std::move(asset_name),
						  type, processing_mode, looping_mode);
}

NonOwningPtr<Sound> SoundManager::CreateSound(std::string name, std::string asset_name,
	sound::SoundType type, std::optional<sound::SoundLoopingMode> looping_mode)
{
	return CreateResource(std::move(name), std::move(asset_name),
						  type, looping_mode);
}


NonOwningPtr<Sound> SoundManager::CreateSound(Sound &&sound)
{
	return CreateResource(std::move(sound));
}


/*
	Sounds
	Retrieving
*/

NonOwningPtr<Sound> SoundManager::GetSound(std::string_view name) noexcept
{
	return GetResource(name);
}

NonOwningPtr<const Sound> SoundManager::GetSound(std::string_view name) const noexcept
{
	return GetResource(name);
}


/*
	Sounds
	Removing
*/

void SoundManager::ClearSounds() noexcept
{
	ClearResources();
}

bool SoundManager::RemoveSound(Sound &sound) noexcept
{
	return RemoveResource(sound);
}

bool SoundManager::RemoveSound(std::string_view name) noexcept
{
	return RemoveResource(name);
}


/*
	Sound channel groups
	Creating
*/

NonOwningPtr<SoundChannelGroup> SoundManager::CreateSoundChannelGroup(std::string name)
{
	return SoundChannelGroupBase::Create(std::move(name));
}


NonOwningPtr<SoundChannelGroup> SoundManager::CreateSoundChannelGroup(const SoundChannelGroup &sound_channel_group)
{
	return SoundChannelGroupBase::Create(sound_channel_group);
}

NonOwningPtr<SoundChannelGroup> SoundManager::CreateSoundChannelGroup(SoundChannelGroup &&sound_channel_group)
{
	return SoundChannelGroupBase::Create(std::move(sound_channel_group));
}


/*
	Sound channel groups
	Retrieving
*/

NonOwningPtr<SoundChannelGroup> SoundManager::GetSoundChannelGroup(std::string_view name) noexcept
{
	return SoundChannelGroupBase::Get(name);
}

NonOwningPtr<const SoundChannelGroup> SoundManager::GetSoundChannelGroup(std::string_view name) const noexcept
{
	return SoundChannelGroupBase::Get(name);
}


/*
	Sound channel groups
	Removing
*/

void SoundManager::ClearSoundChannelGroups() noexcept
{
	SoundChannelGroupBase::Clear();
}

bool SoundManager::RemoveSoundChannelGroup(SoundChannelGroup &sound_channel_group) noexcept
{
	return SoundChannelGroupBase::Remove(sound_channel_group);
}

bool SoundManager::RemoveSoundChannelGroup(std::string_view name) noexcept
{
	return SoundChannelGroupBase::Remove(name);
}


/*
	Sound listeners
	Creating
*/

NonOwningPtr<SoundListener> SoundManager::CreateSoundListener(std::string name)
{
	return SoundListenerBase::Create(std::move(name));
}


/*
	Sound listeners
	Retrieving
*/

NonOwningPtr<SoundListener> SoundManager::GetSoundListener(std::string_view name) noexcept
{
	return SoundListenerBase::Get(name);
}

NonOwningPtr<const SoundListener> SoundManager::GetSoundListener(std::string_view name) const noexcept
{
	return SoundListenerBase::Get(name);
}


/*
	Sound listeners
	Removing
*/

void SoundManager::ClearSoundListeners() noexcept
{
	SoundListenerBase::Clear();
}

bool SoundManager::RemoveSoundListener(SoundListener &sound_listener) noexcept
{
	return SoundListenerBase::Remove(sound_listener);
}

bool SoundManager::RemoveSoundListener(std::string_view name) noexcept
{
	return SoundListenerBase::Remove(name);
}

} //ion::sounds