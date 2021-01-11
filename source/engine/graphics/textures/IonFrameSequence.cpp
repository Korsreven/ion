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

#include <algorithm>
#include <string_view>

#include "utilities/IonConvert.h"
#include "utilities/IonMath.h"

namespace ion::graphics::textures
{

using namespace frame_sequence;

namespace frame_sequence::detail
{

container_type get_frames_from_first_frame(Texture &first_frame, int total_frames)
{
	using namespace ion::utilities;

	if (!first_frame.Owner())
		return {};

	container_type frames;
	frames.reserve(total_frames);
	frames.push_back(&first_frame);

	//Multiple frames
	if (total_frames > 1)
	{
		auto &name = *first_frame.Name();
		
		if (auto iter = std::find_if_not(std::rbegin(name), std::rend(name),
			[](auto c) noexcept
			{
				return convert::detail::is_digit(c);
			}).base(); iter != std::end(name))
		{
			auto off = iter - std::begin(name);
			auto count = std::ssize(name) - off;
			
			if (auto number = convert::To<int>({std::data(name) + off, static_cast<size_t>(count)}); number)
			{
				auto name_prefix = name.substr(0, off);	
				std::string next_name;

				for (auto i = 1; i < total_frames; ++i)
				{
					auto digits = math::Log10(++*number) + 1;
					next_name = name_prefix;

					//Add leading zeroes
					if (count - digits > 0)
						next_name.append(count - digits, '0');

					next_name += convert::ToString(*number);

					if (auto next_frame = first_frame.Owner()->GetTexture(next_name); next_frame)
						frames.push_back(next_frame);
					else
						return {};
				}
			}
		}
	}

	return frames;
}

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

	if (!std::empty(frames_))
	{
		//Set up callbacks
		observed_frames_.OnRemoved({&FrameSequence::FrameRemoved, this});
		observed_frames_.OnRemovedAll({&FrameSequence::AllFramesRemoved, this});
		return true;
	}
	else
		return false;
}

void FrameSequence::ClearFrames() noexcept
{
	frames_.clear();
	frames_.shrink_to_fit();
	
	if (observed_frames_.ReleaseAll())
	{
		//Release callbacks
		observed_frames_.OnRemoved(std::nullopt);
		observed_frames_.OnRemovedAll(std::nullopt);
	}
}


void FrameSequence::FrameRemoved(Texture&) noexcept
{
	//A frame sequence is considered invalid if one or more frames are missing
	//All frames or no frames
	ClearFrames();
}

void FrameSequence::AllFramesRemoved() noexcept
{
	//A frame sequence is considered invalid if one or more frames are missing
	//All frames or no frames
	ClearFrames();
}


//Public

FrameSequence::FrameSequence(std::string name, const detail::container_type &frames) :
	managed::ManagedObject<FrameSequenceManager>{std::move(name)}
{
	AddFrames(frames);
}

FrameSequence::FrameSequence(std::string name, Texture &first_frame, int total_frames)  :
	managed::ManagedObject<FrameSequenceManager>{std::move(name)}
{
	if (total_frames > 0)
		AddFrames(detail::get_frames_from_first_frame(first_frame, total_frames));
}

} //ion::graphics::textures