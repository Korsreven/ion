/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonFrameSequenceManager.h
-------------------------------------------
*/

#ifndef ION_FRAME_SEQUENCE_MANAGER_H
#define ION_FRAME_SEQUENCE_MANAGER_H

#include <type_traits>
#include <string>
#include <string_view>

#include "IonFrameSequence.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::textures
{
	namespace frame_sequence_manager::detail
	{
	} //frame_sequence_manager::detail


	///@brief A class that manages and stores frame sequences
	class FrameSequenceManager final :
		public managed::ObjectManager<FrameSequence, FrameSequenceManager>
	{
		public:

			///@brief Default constructor
			FrameSequenceManager() = default;

			///@brief Deleted copy constructor
			FrameSequenceManager(const FrameSequenceManager&) = delete;

			///@brief Default move constructor
			FrameSequenceManager(FrameSequenceManager&&) = default;

			///@brief Destructor
			~FrameSequenceManager() = default;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			FrameSequenceManager& operator=(const FrameSequenceManager&) = delete;

			///@brief Default move assignment
			FrameSequenceManager& operator=(FrameSequenceManager&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all frame sequences in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto FrameSequences() noexcept
			{
				return Objects();
			}

			///@brief Returns an immutable range of all frame sequences in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto FrameSequences() const noexcept
			{
				return Objects();
			}

			///@}

			/**
				@name Frame sequences
				Creating
				@{
			*/

			///@brief Creates a frame sequence with the given name and frames (textures)
			NonOwningPtr<FrameSequence> CreateFrameSequence(std::string name, const frame_sequence::detail::container_type &frames);

			///@brief Creates a frame sequence with the given name and frame references (textures)
			template <typename... Tn, typename = std::enable_if_t<std::conjunction_v<std::is_same<NonOwningPtr<Texture>, Tn>...>>>
			NonOwningPtr<FrameSequence> CreateFrameSequence(std::string name, NonOwningPtr<Texture> frame, Tn ...rest)
			{
				return CreateFrameSequence(std::move(name), {frame, rest...});
			}

			///@brief Creates a frame sequence with the given name, first frame (texture) and total frames
			///@details Takes the name of the first frame with a numeric suffix and increases it in range [first, total frames).
			///The generated names must be actual frames located in the same owner as the first frame.
			///First example: frame_0 and 4, produces the sequence: frame_0, frame_1, frame_2, frame_3.
			///Second example: frame_01 and 4, produces the sequence: frame_01, frame_02, frame_03, frame_04.
			///Third example: frame09 and 3, produces the sequence: frame09, frame10, frame11
			NonOwningPtr<FrameSequence> CreateFrameSequence(std::string name, NonOwningPtr<Texture> first_frame, int total_frames);


			///@brief Creates a frame sequence as a copy of the given frame sequence
			NonOwningPtr<FrameSequence> CreateFrameSequence(const FrameSequence &frame_sequence);

			///@brief Creates a frame sequence by moving the given frame sequence
			NonOwningPtr<FrameSequence> CreateFrameSequence(FrameSequence &&frame_sequence);

			///@}

			/**
				@name Frame sequences
				Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable frame sequence with the given name
			///@details Returns nullptr if frame sequence could not be found
			[[nodiscard]] NonOwningPtr<FrameSequence> GetFrameSequence(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable frame sequence with the given name
			///@details Returns nullptr if frame sequence could not be found
			[[nodiscard]] NonOwningPtr<const FrameSequence> GetFrameSequence(std::string_view name) const noexcept;

			///@}

			/**
				@name Frame sequences
				Removing
				@{
			*/

			///@brief Clears all removable frame sequences from this manager
			void ClearFrameSequences() noexcept;

			///@brief Removes a removable frame sequence from this manager
			bool RemoveFrameSequence(FrameSequence &frame_sequences) noexcept;

			///@brief Removes a removable frame sequence with the given name from this manager
			bool RemoveFrameSequence(std::string_view name) noexcept;

			///@}
	};
} //ion::graphics::textures

#endif