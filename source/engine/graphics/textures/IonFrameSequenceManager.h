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
			return this->Objects();
		}

		//Returns an immutable range of all frame sequences in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline const auto FrameSequences() const noexcept
		{
			return this->Objects();
		}


		/*
			Frame sequences
			Creating
		*/

		//Create a frame sequence with the given frames (textures)
		FrameSequence& CreateFrameSequence(const frame_sequence::detail::container_type &frames);

		//Create a frame sequence with the given frame references (textures)
		template <typename... Tn, typename = std::enable_if_t<std::conjunction_v<std::is_same<Texture, Tn>...>>>
		FrameSequence& CreateFrameSequence(Texture &frame, Tn &...rest)
		{
			return CreateFrameSequence({&frame, &rest...});
		}


		/*
			Frame sequences
			Removing
		*/

		//Clear all removable frame sequences from this manager
		void ClearFrameSequences() noexcept;

		//Remove a removable frame sequence from this manager
		bool RemoveFrameSequence(FrameSequence &frame_sequences) noexcept;
	};
} //ion::graphics::textures

#endif