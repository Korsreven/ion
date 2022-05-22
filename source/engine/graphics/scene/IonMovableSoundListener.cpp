/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableSoundListener.cpp
-------------------------------------------
*/

#include "IonMovableSoundListener.h"

#include "graph/IonSceneNode.h"
#include "query/IonSceneQuery.h"
#include "sounds/IonSoundListener.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene
{

using namespace movable_sound_listener;
using namespace types::type_literals;

namespace movable_sound_listener::detail
{
} //movable_sound_listener::detail


MovableSoundListener::MovableSoundListener(std::optional<std::string> name,
	NonOwningPtr<sounds::SoundListener> sound_listener) :

	MovableObject{std::move(name)},
	sound_listener_{sound_listener}
{
	query_type_flags_ |= query::scene_query::QueryType::SoundListener;
}

MovableSoundListener::MovableSoundListener(std::optional<std::string> name, const Vector3 &position,
	NonOwningPtr<sounds::SoundListener> sound_listener) :

	MovableObject{std::move(name)},
	position_{position},
	sound_listener_{sound_listener}
{
	query_type_flags_ |= query::scene_query::QueryType::SoundListener;
}


/*
	Modifiers
*/




/*
	Elapse time
*/

void MovableSoundListener::Elapse(duration time) noexcept
{
	if (auto parent_node = ParentNode(); parent_node)
	{
		auto world_position = position_ + parent_node->DerivedPosition();

		if (sound_listener_)
		{
			auto velocity = world_position - previous_world_position_.value_or(world_position);
			auto velocity_units_sec = velocity * (1.0_r / time.count());
			sound_listener_->Attributes(world_position, velocity_units_sec);
		}

		previous_world_position_ = world_position;
	}
	else
		previous_world_position_ = {};
}

} //ion::graphics::scene