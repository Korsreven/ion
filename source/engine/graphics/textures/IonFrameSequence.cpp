/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonFrameSequence.cpp
-------------------------------------------
*/

#include "IonFrameSequence.h"

namespace ion::graphics::textures
{

using namespace frame_sequence;

namespace frame_sequence::detail
{
} //frame_sequence::detail


//Private

bool FrameSequence::AddFrame(Texture &frame)
{
	if (observed_frames_.Observe(frame))
	{
		frames_.push_back(&frame);
		return true;
	}
	else
		return false;
}

bool FrameSequence::AddFrames(const detail::container_type &frames)
{
	for (auto &frame : frames)
	{
		if (!frame || !AddFrame(*frame))
		{
			ClearFrames(); //Missing a frame
			return false;
		}
	}

	total_observed_frames_ = std::ssize(observed_frames_.Objects());
	return !std::empty(frames_);
}

void FrameSequence::ClearFrames() noexcept
{
	frames_.clear();
	frames_.shrink_to_fit();
	observed_frames_.ReleaseAll();	
}


//Public

FrameSequence::FrameSequence(const detail::container_type &frames)
{
	AddFrames(frames);
}

} //ion::graphics::textures