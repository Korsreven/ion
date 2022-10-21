/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonFrameSequence.h
-------------------------------------------
*/

#ifndef ION_FRAME_SEQUENCE_H
#define ION_FRAME_SEQUENCE_H

#include <type_traits>
#include <vector>

#include "IonTextureManager.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::textures
{
	class FrameSequenceManager; //Forward declaration

	namespace frame_sequence::detail
	{
		using container_type = std::vector<NonOwningPtr<Texture>>;

		container_type get_frames_from_first_frame(NonOwningPtr<Texture> first_frame, int total_frames);
	} //frame_sequence::detail


	//A class representing a frame sequence that contains one or more frames (textures)
	class FrameSequence final : public managed::ManagedObject<FrameSequenceManager>
	{
		private:

			frame_sequence::detail::container_type frames_;

		public:
			
			//Constructs a new frame sequence with the given name and frames (textures)
			//Duplicate frames are allowed within a frame sequence
			FrameSequence(std::string name, frame_sequence::detail::container_type frames) noexcept;

			//Constructs a new frame sequence with the given name and frame references (textures)
			//Duplicate frames are allowed within a frame sequence
			template <typename... Tn, typename = std::enable_if_t<std::conjunction_v<std::is_same<NonOwningPtr<Texture>, Tn>...>>>
			FrameSequence(std::string name, NonOwningPtr<Texture> frame, Tn ...rest) :
				FrameSequence{std::move(name), {frame, rest...}}
			{
				//Empty
			}

			//Constructs a new frame sequence with the given name, first frame (texture) and total frames
			//Takes the name of the first frame with a numeric suffix and increases it in range [first, total frames)
			//The generated names must be actual frames located in the same owner as the first frame
			//First example: frame_0 and 4, produces the sequence: frame_0, frame_1, frame_2, frame_3
			//Second example: frame_01 and 4, produces the sequence: frame_01, frame_02, frame_03, frame_04
			//Third example: frame09 and 3, produces the sequence: frame09, frame10, frame11
			FrameSequence(std::string name, NonOwningPtr<Texture> first_frame, int total_frames);


			/*
				Operators
			*/

			//Returns a pointer to the frame at the given offset
			[[nodiscard]] inline auto operator[](int off) const noexcept
			{
				assert(off >= 0 && off < std::ssize(frames_));
				return frames_[off];
			}

			
			/*
				Ranges
			*/

			//Returns a mutable range of all frames (textures) in this frame sequence
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Frames() noexcept
			{
				return adaptors::ranges::DereferenceIterable<frame_sequence::detail::container_type&>{frames_};
			}

			//Returns an immutable range of all frames (textures) in this frame sequence
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Frames() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const frame_sequence::detail::container_type&>{frames_};
			}


			/*
				Observers
			*/

			//Returns the count of all frames in this frame sequence
			[[nodiscard]] inline auto FrameCount() const noexcept
			{
				return std::ssize(frames_);
			}

			//Returns true if this frame sequence is empty (has no frames)
			[[nodiscard]] inline auto IsEmpty() const noexcept
			{
				return std::empty(frames_);
			}


			/*
				Frames
			*/

			//Returns a pointer to the first frame in this frame sequence
			//Returns nullptr if frame sequence has no frames
			[[nodiscard]] inline auto FirstFrame() const noexcept
			{
				return !std::empty(frames_) ? frames_.front() : nullptr;
			}

			//Returns a pointer to the last frame in this frame sequence
			//Returns nullptr if frame sequence has no frames
			[[nodiscard]] inline auto LastFrame() const noexcept
			{
				return !std::empty(frames_) ? frames_.back() : nullptr;
			}
	};
} //ion::graphics::textures

#endif