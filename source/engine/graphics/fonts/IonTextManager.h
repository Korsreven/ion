/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonTextManager.h
-------------------------------------------
*/

#ifndef ION_TEXT_MANAGER_H
#define ION_TEXT_MANAGER_H

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "IonText.h"
#include "managed/IonObjectManager.h"

namespace ion::graphics::fonts
{
	namespace text_manager::detail
	{
	} //text_manager::detail


	struct TextManager final :
		managed::ObjectManager<Text, TextManager>
	{
		//Default constructor
		TextManager() = default;

		//Deleted copy constructor
		TextManager(const TextManager&) = delete;

		//Default move constructor
		TextManager(TextManager&&) = default;

		//Destructor
		~TextManager() = default;


		/*
			Operators
		*/

		//Deleted copy assignment
		TextManager& operator=(const TextManager&) = delete;

		//Move assignment
		TextManager& operator=(TextManager&&) = default;


		/*
			Ranges
		*/

		//Returns a mutable range of all texts in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline auto Texts() noexcept
		{
			return Objects();
		}

		//Returns an immutable range of all texts in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline const auto Texts() const noexcept
		{
			return Objects();
		}


		/*
			Texts
			Creating
		*/

		//Create a (unbounded) text with the given name, content, alignment and a type face
		Text& CreateText(std::string name, std::string content, text::TextAlignment alignment, TypeFace &type_face);

		//Create a (unbounded) text with the given name, content and a type face
		Text& CreateText(std::string name, std::string content, TypeFace &type_face);

		//Create a text with the given name, content, formatting,
		//horizontal and vertical alignment, area size, padding, line spacing and a type face
		Text& CreateText(std::string name, std::string content, text::TextFormatting formatting,
			text::TextAlignment alignment, text::TextVerticalAlignment vertical_alignment,
			const std::optional<Vector2> &area_size, const std::optional<Vector2> &padding,
			std::optional<int> line_spacing, TypeFace &type_face);

		//Create a text (area) with the given name, content,
		//horizontal and vertical alignment, area size, padding, line spacing and a type face
		Text& CreateText(std::string name, std::string content,
			text::TextAlignment alignment, text::TextVerticalAlignment vertical_alignment,
			const std::optional<Vector2> &area_size, const std::optional<Vector2> &padding,
			std::optional<int> line_spacing, TypeFace &type_face);

		//Create a text (area) with the given name, content, area size, padding, line spacing and a type face
		Text& CreateText(std::string name, std::string content,
			const std::optional<Vector2> &area_size, const std::optional<Vector2> &padding,
			std::optional<int> line_spacing, TypeFace &type_face);


		//Create a text as a copy of the given text
		Text& CreateText(const Text &text);

		//Create a text by moving the given text
		Text& CreateText(Text &&text);


		/*
			Texts
			Retrieving
		*/

		//Gets a pointer to a mutable text with the given name
		//Returns nullptr if text could not be found
		[[nodiscard]] Text* GetText(std::string_view name) noexcept;

		//Gets a pointer to an immutable text with the given name
		//Returns nullptr if text could not be found
		[[nodiscard]] const Text* GetText(std::string_view name) const noexcept;


		/*
			Texts
			Removing
		*/

		//Clear all removable texts from this manager
		void ClearTexts() noexcept;

		//Remove a removable text from this manager
		bool RemoveText(Text &text) noexcept;

		//Remove a removable text with the given name from this manager
		bool RemoveText(std::string_view name) noexcept;
	};
} //ion::graphics::fonts

#endif