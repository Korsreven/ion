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
#include "managed/IonObjectObserver.h"

namespace ion::graphics::textures
{
	struct FrameSequenceManager; //Forward declaration

	namespace frame_sequence::detail
	{
		using container_type = std::vector<Texture*>; //Non-owning
	} //frame_sequence::detail


	//A frame sequence class that contains one or more frames (textures)
	class FrameSequence final : public managed::ManagedObject<FrameSequenceManager>
	{
		private:

			frame_sequence::detail::container_type frames_;
			managed::ObjectObserver<Texture> observed_frames_;	
			int total_observed_frames_ = 0;


			bool AddFrame(Texture &frame);
			bool AddFrames(const frame_sequence::detail::container_type &frames);
			void ClearFrames() noexcept;

		public:

			//Default constructor
			FrameSequence() = default;

			//Constructs a new frame sequence with the given frames (textures)
			//Duplicate frames are allowed within a frame sequence
			FrameSequence(const frame_sequence::detail::container_type &frames);

			//Constructs a new frame sequence with the given frame references (textures)
			//Duplicate frames are allowed within a frame sequence
			template <typename... Tn, typename = std::enable_if_t<std::conjunction_v<std::is_same<Texture, Tn>...>>>
			FrameSequence(Texture &frame, Tn &...rest) :
				FrameSequence{{&frame, &rest...}}
			{
				//Empty
			}


			/*
				Operators
			*/

			//Returns true if this frame sequence has all of its initial frames
			//A frame sequence is considered invalid if one or more frames are missing
			inline operator bool() const noexcept
			{
				return !std::empty(frames_) &&
					total_observed_frames_ == std::ssize(observed_frames_.Objects());
					//All initial frames are still being observed
			}

			//Returns a pointer to a mutable frame at the given offset
			[[nodiscard]] inline auto operator[](int off) noexcept
			{
				assert(off >= 0 && off < std::ssize(frames_));
				return *this ? frames_[off] : nullptr;
			}

			//Returns a pointer to an immutable frame at the given offset
			[[nodiscard]] inline auto operator[](int off) const noexcept
			{
				assert(off >= 0 && off < std::ssize(frames_));
				return *this ? frames_[off] : nullptr;
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
			[[nodiscard]] inline const auto Frames() const noexcept
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

			//Returns true if this frame sequence has all of its initial frames (textures)
			//A frame sequence is considered invalid if one or more frames are missing
			[[nodiscard]] inline auto HasAllInitialFrames() const noexcept
			{
				return !!*this;
			}
	};
} //ion::graphics::textures

#endif