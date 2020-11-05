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

#include <optional>
#include <string>
#include <string_view>

#include "IonText.h"
#include "graphics/utilities/IonVector2.h"
#include "managed/IonObjectManager.h"

namespace ion::graphics::fonts
{
	class Font; //Forward declaration

	using utilities::Color;
	using utilities::Vector2;

	namespace text_manager::detail
	{
		int character_width(char c, Font &font) noexcept;
		int character_height(char c, Font &font) noexcept;

		int string_width(std::string_view str, Font &font) noexcept;
		int string_height(std::string_view str, Font &font) noexcept;
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

		//Create a text with the given name, string and a type face
		Text& CreateText(std::string name, std::string str, TypeFace &type_face);


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


		/*
			Measuring
		*/

		//Returns the size, in pixels, of the given character when rendered with the given font
		static std::optional<Vector2> MeasureCharacter(char c, Font &font) noexcept;

		//Returns the size, in pixels, of the given string when rendered with the given font
		static std::optional<Vector2> MeasureString(std::string_view str, Font &font) noexcept;
	};
} //ion::graphics::fonts

#endif