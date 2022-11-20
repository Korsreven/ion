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
	Frame sequences - Creating
*/

NonOwningPtr<FrameSequence> FrameSequenceManager::CreateFrameSequence(std::string name, const frame_sequence::detail::container_type &frames)
{
	return Create(std::move(name), frames);
}

NonOwningPtr<FrameSequence> FrameSequenceManager::CreateFrameSequence(std::string name, NonOwningPtr<Texture> first_frame, int total_frames)
{
	return Create(std::move(name), first_frame, total_frames);
}


NonOwningPtr<FrameSequence> FrameSequenceManager::CreateFrameSequence(const FrameSequence &frame_sequence)
{
	return Create(frame_sequence);
}

NonOwningPtr<FrameSequence> FrameSequenceManager::CreateFrameSequence(FrameSequence &&frame_sequence)
{
	return Create(std::move(frame_sequence));
}


/*
	Frame sequences - Retrieving
*/

NonOwningPtr<FrameSequence> FrameSequenceManager::GetFrameSequence(std::string_view name) noexcept
{
	return Get(name);
}

NonOwningPtr<const FrameSequence> FrameSequenceManager::GetFrameSequence(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Frame sequences - Removing
*/

void FrameSequenceManager::ClearFrameSequences() noexcept
{
	Clear();
}

bool FrameSequenceManager::RemoveFrameSequence(FrameSequence &frame_sequence) noexcept
{
	return Remove(frame_sequence);
}

bool FrameSequenceManager::RemoveFrameSequence(std::string_view name) noexcept
{
	return Remove(name);
}

} //ion::graphics::textures