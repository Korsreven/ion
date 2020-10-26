/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonFrameSequenceManager.cpp
-------------------------------------------
*/

#include "IonFrameSequenceManager.h"
#include <type_traits>

namespace ion::graphics::textures
{

using namespace frame_sequence_manager;

namespace frame_sequence_manager::detail
{

} //frame_sequence_manager::detail


//Public


/*
	Frame sequences
	Creating
*/

FrameSequence& FrameSequenceManager::CreateFrameSequence(std::string name, const frame_sequence::detail::container_type &frames)
{
	return this->Create(std::move(name), frames);
}


/*
	Frame sequences
	Removing
*/

void FrameSequenceManager::ClearFrameSequences() noexcept
{
	this->Clear();
}

bool FrameSequenceManager::RemoveFrameSequence(FrameSequence &frame_sequence) noexcept
{
	return this->Remove(frame_sequence);
}

} //ion::graphics::textures