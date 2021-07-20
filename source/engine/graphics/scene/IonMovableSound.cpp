/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableSound.cpp
-------------------------------------------
*/

#include "IonMovableSound.h"

#include "graph/IonSceneNode.h"
#include "query/IonSceneQuery.h"
#include "sounds/IonSound.h"

namespace ion::graphics::scene
{

using namespace movable_sound;
using namespace graphics::utilities;

namespace movable_sound::detail
{
} //movable_sound::detail


MovableSound::MovableSound(NonOwningPtr<sounds::Sound> sound, bool paused) :
	MovableSound{vector3::Zero, sound, paused}
{
	//Empty
}

MovableSound::MovableSound(const Vector3 &position, NonOwningPtr<sounds::Sound> sound, bool paused) :

	position_{position},
	sound_channel_{sound ? sound->Play(paused) : nullptr},
	initial_sound_{sound}
{
	query_type_flags_ |= query::scene_query::QueryType::Sound;
}


MovableSound::MovableSound(NonOwningPtr<sounds::Sound> sound, NonOwningPtr<sounds::SoundChannelGroup> sound_channel_group, bool paused) :
	MovableSound{vector3::Zero, sound, sound_channel_group, paused}
{
	//Empty
}

MovableSound::MovableSound(const Vector3 &position, NonOwningPtr<sounds::Sound> sound, NonOwningPtr<sounds::SoundChannelGroup> sound_channel_group, bool paused) :

	position_{position},
	sound_channel_{sound ? sound->Play(sound_channel_group, paused) : nullptr},
	initial_sound_{sound}
{
	query_type_flags_ |= query::scene_query::QueryType::Sound;
}


/*
	Modifiers
*/

void MovableSound::Revert()
{
	if (initial_sound_)
		sound_channel_ = initial_sound_->Play(sound_channel_, !sound_channel_->IsPlaying());
}


/*
	Elapse time
*/

void MovableSound::Elapse(duration time) noexcept
{
	if (auto parent_node = ParentNode(); parent_node)
	{
		auto world_position = position_ + parent_node->DerivedPosition();

		if (sound_channel_)
		{
			auto velocity = world_position - previous_world_position_.value_or(world_position);
			auto velocity_units_sec = velocity * (1.0_r / time.count());
			sound_channel_->Attributes(world_position, velocity_units_sec);
		}

		previous_world_position_ = world_position;
	}
	else
		previous_world_position_ = {};
}

} //ion::graphics::scene