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

namespace ion::graphics::textures
{
	namespace frame_sequence_manager::detail
	{
	} //frame_sequence_manager::detail


	struct FrameSequenceManager final :
		managed::ObjectManager<FrameSequence, FrameSequenceManager>
	{
		//Default constructor
		FrameSequenceManager() = default;

		//Deleted copy constructor
		FrameSequenceManager(const FrameSequenceManager&) = delete;

		//Default move constructor
		FrameSequenceManager(FrameSequenceManager&&) = default;

		//Destructor
		~FrameSequenceManager() = default;


		/*
			Operators
		*/

		//Deleted copy assignment
		FrameSequenceManager& operator=(const FrameSequenceManager&) = delete;

		//Move assignment
		FrameSequenceManager& operator=(FrameSequenceManager&&) = default;


		/*
			Ranges
		*/

		//Returns a mutable range of all frame sequences in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline auto FrameSequences() noexcept
		{
			return Objects();
		}

		//Returns an immutable range of all frame sequences in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline const auto FrameSequences() const noexcept
		{
			return Objects();
		}


		/*
			Frame sequences
			Creating
		*/

		//Create a frame sequence with the given name and frames (textures)
		FrameSequence& CreateFrameSequence(std::string name, const frame_sequence::detail::container_type &frames);

		//Create a frame sequence with the given name and frame references (textures)
		template <typename... Tn, typename = std::enable_if_t<std::conjunction_v<std::is_same<Texture, Tn>...>>>
		FrameSequence& CreateFrameSequence(std::string name, Texture &frame, Tn &...rest)
		{
			return CreateFrameSequence(std::move(name), {&frame, &rest...});
		}


		//Create a frame sequence as a copy of the given frame sequence
		FrameSequence& CreateFrameSequence(const FrameSequence &frame_sequence);

		//Create a frame sequence by moving the given frame sequence
		FrameSequence& CreateFrameSequence(FrameSequence &&frame_sequence);


		/*
			Frame sequences
			Retrieving
		*/

		//Gets a pointer to a mutable frame sequence with the given name
		//Returns nullptr if frame sequence could not be found
		[[nodiscard]] FrameSequence* GetFrameSequence(std::string_view name) noexcept;

		//Gets a pointer to an immutable frame sequence with the given name
		//Returns nullptr if frame sequence could not be found
		[[nodiscard]] const FrameSequence* GetFrameSequence(std::string_view name) const noexcept;


		/*
			Frame sequences
			Removing
		*/

		//Clear all removable frame sequences from this manager
		void ClearFrameSequences() noexcept;

		//Remove a removable frame sequence from this manager
		bool RemoveFrameSequence(FrameSequence &frame_sequences) noexcept;

		//Remove a removable frame sequence with the given name from this manager
		bool RemoveFrameSequence(std::string_view name) noexcept;
	};
} //ion::graphics::textures

#endif