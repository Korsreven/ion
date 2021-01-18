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

container_type get_frames_from_first_frame(NonOwningPtr<Texture> first_frame, int total_frames)
{
	using namespace ion::utilities;

	if (!first_frame->Owner())
		return {};

	container_type frames;
	frames.reserve(total_frames);
	frames.push_back(first_frame);

	//Multiple frames
	if (total_frames > 1)
	{
		auto &name = *first_frame->Name();
		
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

					if (auto next_frame = first_frame->Owner()->GetTexture(next_name); next_frame)
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


//Public

FrameSequence::FrameSequence(std::string name, const detail::container_type &frames) :

	managed::ManagedObject<FrameSequenceManager>{std::move(name)},
	frames_{frames}
{
	//Empty
}

FrameSequence::FrameSequence(std::string name, NonOwningPtr<Texture> first_frame, int total_frames)  :

	managed::ManagedObject<FrameSequenceManager>{std::move(name)},
	frames_{first_frame && total_frames > 0 ? detail::get_frames_from_first_frame(first_frame, total_frames) : decltype(frames_){}}
{
	//Empty
}

} //ion::graphics::textures